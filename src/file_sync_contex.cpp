#include "file_sync_contex.h"

#include <iostream>

int FileSyncPipe::actionPut(const std::string &filePath, int type)
{
    {
        std::lock_guard<std::mutex> guard(mtx);
        q.emplace(type, filePath);
    }
    cv.notify_one();
    return 0;
}

int FileSyncPipe::actionGet(std::string &filePath, int &type)
{
    // Wait until main() sends data
    std::unique_lock lk(mtx);
    cv.wait(lk, [this]{return !this->q.empty();});
 
    // after the wait, we own the lock.
    
    //std::cout << "Worker thread is processing data\n";
    //data += " after processing";
 
    // Send data back to main()
    filePath = q.front().filePath;
    type = q.front().type;
    q.pop();

    //std::cout << "Worker thread signals data processing completed\n";
 
    // Manual unlocking is done before notifying, to avoid waking up
    // the waiting thread only to block again (see notify_one for details)
    lk.unlock();
    //cv.notify_one();

    return 0;
}

int FileSyncPipe::put_action(int action, const std::string &new_filename, const std::string &old_filename)
{
    {
        std::lock_guard<std::mutex> guard(mtx);
        q.emplace(action, new_filename, old_filename);
    }
    cv.notify_one();
    return 0;
}

int FileSyncPipe::get_action(int &action, std::string &new_filename, std::string &old_filename)
{
	// Wait until main() sends data
    std::unique_lock lk(mtx);
    cv.wait(lk, [this]{return !this->q.empty();});
 
    // after the wait, we own the lock.
    
    //std::cout << "Worker thread is processing data\n";
    //data += " after processing";
 
    // Send data back to main()
    new_filename = q.front().filePath;
    action = q.front().type;
	old_filename = q.front().old_filename;
    q.pop();

    //std::cout << "Worker thread signals data processing completed\n";
 
    // Manual unlocking is done before notifying, to avoid waking up
    // the waiting thread only to block again (see notify_one for details)
    lk.unlock();
    //cv.notify_one();

    return 0;
}



