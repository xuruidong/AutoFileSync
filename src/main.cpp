#include <pthread.h>
#include <iostream>
#include <unistd.h>

#include "file_sync_contex.h"
#include "monitor.h"
#include "sftpUpload.h"
#include "util.h"

#define CMD_LINE_MAX_SIZE	200

void *threadSynchronise(void *arg)
{
    FileSyncPipe *ctx = (FileSyncPipe *)arg;
    std::string dst_addr = "sftp://192.168.56.102";
	//std::string dst_path = "/tmp/";
	char sftp_cmd_line[CMD_LINE_MAX_SIZE] = {0};
	std::string base_path;
	size_t pos = 0;
	for(int i=0; i<3; ++i) {
		pos = ctx->get_remote_path().find('/', pos);
		++pos;
	}
	std::cout<<"threadSynchronise pos= "<<pos<<std::endl;
	if(pos < 3) {
		// if pos < 3, that means the third '/' is not exist
		std::cout<<"threadSynchronise can not find remote dir "<<std::endl;
		exit(1);
	}
	--pos;
	dst_addr = ctx->get_remote_path().substr(0, pos);
	base_path = ctx->get_remote_path().c_str() + pos;
	std::cout<<"threadSynchronise dst_addr= "<<dst_addr<<std::endl;
	std::cout<<"threadSynchronise base_path= "<<base_path<<std::endl;
    while(1) {
        std::string s;
		std::string old_filename;
        int type = 0;
        //ctx->actionGet(s, type);
		ctx->get_action(type, s, old_filename);
        std::cout<<"[threadSynchronise:19] actionGet return. "<<s<<"  type:"<<type<<std::endl;
        if(s.rfind(ctx->getDirector(), 0) == 0) {
			if (type == 4) {
				// rename
				std::string new_file = base_path + (s.c_str() + ctx->getDirector().size());
				std::string old_file = base_path + (old_filename.c_str() + ctx->getDirector().size());
				snprintf(sftp_cmd_line, sizeof(sftp_cmd_line), "rename %s %s", old_file.c_str(), new_file.c_str());
				#if 1
				if(sftpCMD2(sftp_cmd_line, dst_addr.c_str()) < 0) {
                    printf("[%s:%d] rename %s\n", __FUNCTION__, __LINE__, sftp_cmd_line);
				}
				#endif
				continue;
			}
			if(type == 2) {
				// delete
				std::string dst_file = base_path + (s.c_str() + ctx->getDirector().size());
				if(sftpCMD("rm", dst_file.c_str(), dst_addr.c_str()) < 0) {
					sftpCMD("rmdir", dst_file.c_str(), dst_addr.c_str());
                    printf("[%s:%d] rmdir %s\n", __FUNCTION__, __LINE__, dst_file.c_str());
				}
				continue;
			}
			if(is_dir(s.c_str())) {
				std::cout<<"==== dir change "<<s<<std::endl;
				std::cout<<"==== dir name "<<s.c_str() + ctx->getDirector().size()<<std::endl;
				#if 1
				std::string dst_file = base_path + (s.c_str() + ctx->getDirector().size());
				sftpMkdir(dst_file.c_str(), dst_addr.c_str());
				#else
				// wrong
					sftpMkdir(s.c_str() + ctx->getDirector().size(), (dst_addr+dst_path).c_str());

				#endif
			}
            std::string remote = dst_addr + base_path+(s.c_str() + ctx->getDirector().size());
            std::cout<<"remote "<<remote<<std::endl;
			sftpUpload(s.c_str(), remote.c_str());
			//std::string dst_file = dst_path + (s.c_str() + ctx->getDirector().size());
            //sftpUpload(dst_file.c_str(), dst_addr.c_str());
        }
    }
    return NULL;
}


int main(int argc, char * argv[])
{
    
	char pszDirectory[100] = "./testpath/";  //  something wrong with "E:\\fstest"  "E:/fstest"
											//  "E:/fstest/" is ok
    if (argc < 3) {
		printf("usage: %s local_path remote_addr\n", argv[0]);
		printf("e.g. %s sftp://username:password@192.168.56.102/tmp/\n", argv[0]);
		return 0;
	}
	FileSyncPipe ctx(argv[1], argv[2]);

    sftpUploadInit(argv[1]);
   // return 0;

    
	pthread_t tid;
	if(pthread_create(&tid, NULL, threadSynchronise, &ctx) < 0) {

        perror("pthread_create 1");
	}
	
    if(pthread_create(&tid, NULL, monitorFileProc, &ctx) < 0) {

        perror("pthread_create 2");
	}
	
    //sftpUploadInit(argv[0]);
    //curl_global_init(CURL_GLOBAL_NOTHING);
	system("pause");
    for(;;) {
        sleep(1000);
    }
    printf("main thread exit\n");

    sftpUploadDeinit();
	//HANDLE hThread = CreateThread(NULL, 0, MonitorFileThreadProc, pszDirectory, 0, NULL);
	//WaitForSingleObject(hThread, INFINITE);
	//CloseHandle(hThread);
	return 0;
}


