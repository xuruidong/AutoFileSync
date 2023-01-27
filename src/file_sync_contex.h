#ifndef __FILE_SYNC_CONTEX_H__
#define __FILE_SYNC_CONTEX_H__

#include <mutex>
#include <queue>
#include <condition_variable>

struct FileSyncAction {
    int type;
    std::string filePath;
	std::string old_filename;

    FileSyncAction(int tp, const std::string &filename) :type(tp), filePath(filename){}
	FileSyncAction(int tp, const std::string &new_filename, const std::string &old_filename) :type(tp), filePath(new_filename), old_filename(old_filename){}
};

//FileSyncer,  FileSyncPipe 
class FileSyncPipe {
public:
    FileSyncPipe(const std::string &dir, const std::string &remote_path): directory2watch(dir), remote_path(remote_path) {}

    int actionPut(const std::string &filePath, int type);
    int actionGet(std::string &filePath, int &type);
	int put_action(int action, const std::string &new_filename, const std::string &old_filename="");
	int get_action(int &action, std::string &filename, std::string &filename2);
    const std::string &getDirector() const {
        return directory2watch; 
    }

	const std::string &get_remote_path() const {
        return remote_path; 
    }

private:
    std::string directory2watch;
	std::string remote_path;
    std::queue<struct FileSyncAction> q;
    std::mutex mtx;
    std::condition_variable cv;
};


#endif

