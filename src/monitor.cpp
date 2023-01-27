#include <stdio.h>
#include <Windows.h>
#include <tlhelp32.h>

#include "file_sync_contex.h"

void *monitorFileProc(void * lParam)
{
    FileSyncPipe *ctx = (FileSyncPipe *)lParam;
	const char *pszDirectory = ctx->getDirector().c_str();
	BOOL bRet = FALSE;
	BYTE Buffer[1024] = { 0 };

	FILE_NOTIFY_INFORMATION *pBuffer = (FILE_NOTIFY_INFORMATION *)Buffer;
	DWORD dwByteReturn = 0;
	HANDLE hFile = CreateFile(pszDirectory, FILE_LIST_DIRECTORY, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
	if (INVALID_HANDLE_VALUE == hFile) {
        printf("[%s:%d] INVALID_HANDLE_VALUE\n", __FUNCTION__, __LINE__);
		return nullptr;
    }

	while (TRUE)
	{
		ZeroMemory(Buffer, sizeof(Buffer));
		// 设置监控目录回调函数
		bRet = ReadDirectoryChangesW(hFile,&Buffer,sizeof(Buffer),TRUE,
			FILE_NOTIFY_CHANGE_FILE_NAME |			// 修改文件名
            FILE_NOTIFY_CHANGE_DIR_NAME |
			FILE_NOTIFY_CHANGE_ATTRIBUTES |			// 修改文件属性
			FILE_NOTIFY_CHANGE_LAST_WRITE |			// 最后一次写入
			FILE_NOTIFY_CHANGE_SIZE|FILE_NOTIFY_CHANGE_LAST_ACCESS|FILE_NOTIFY_CHANGE_CREATION|FILE_NOTIFY_CHANGE_SECURITY,
			&dwByteReturn, NULL, NULL);
		printf("============= [minitor] trigger =============\n");
		if (TRUE == bRet)
		{
			char szFileName[MAX_PATH] = { 0 };
			char old_filename[MAX_PATH] = { 0 };
			
			// 将宽字符转换成窄字符,宽字节字符串转多字节字符串
			WideCharToMultiByte(CP_ACP,0,pBuffer->FileName,(pBuffer->FileNameLength / 2),
				szFileName,MAX_PATH,NULL,NULL);

            for(size_t i=0; i<strlen(szFileName); ++i) {
                if(szFileName[i] == '\\') {
                    szFileName[i] = '/';
                }
            }
			// 将路径与文件连接成完整文件路径
			char FullFilePath[1024] = { 0 };
			strncpy(FullFilePath, pszDirectory, strlen(pszDirectory));
			strcat(FullFilePath, szFileName);
            printf("\n");

			switch (pBuffer->Action)
			{
				case FILE_ACTION_ADDED:
				{
					ctx->actionPut(FullFilePath, pBuffer->Action);
					printf("add: %s \n", FullFilePath); break;
				}
				case FILE_ACTION_REMOVED:
				{
					ctx->actionPut(FullFilePath, pBuffer->Action);
					printf("delete: %s \n", FullFilePath); break;
				}
				case FILE_ACTION_MODIFIED:
				{
					ctx->actionPut(FullFilePath, pBuffer->Action);
					printf("modify: %s \n", FullFilePath); break;
				}
				case FILE_ACTION_RENAMED_OLD_NAME:
				{
					strncpy(old_filename, szFileName, MAX_PATH);
					printf("[monitorFileProc:68]rename: %s\n", szFileName);
					printf("[monitorFileProc:69]pBuffer->NextEntryOffset: %d\n", pBuffer->NextEntryOffset);
					if (0 != pBuffer->NextEntryOffset)
					{
					#if 1
						char new_filename[MAX_PATH] = { 0 };
						FILE_NOTIFY_INFORMATION *tmpBuffer = (FILE_NOTIFY_INFORMATION *)
							((BYTE*)pBuffer + pBuffer->NextEntryOffset/sizeof(char));
						printf("[monitorFileProc:74]tmpBuffer->Action: %d, %d\n", tmpBuffer->Action, FILE_ACTION_RENAMED_NEW_NAME);
						
						switch (tmpBuffer->Action)
						{
							case FILE_ACTION_RENAMED_NEW_NAME:
							{
								ZeroMemory(szFileName, MAX_PATH);
								WideCharToMultiByte(CP_ACP,0,tmpBuffer->FileName,
									(tmpBuffer->FileNameLength / 2),
									new_filename,MAX_PATH,NULL,NULL);
								printf(" -> %s \n", new_filename);
								ctx->put_action(pBuffer->Action, new_filename, FullFilePath);
								break;
							}
							default:
								printf("[%s:%d] error\n", __FUNCTION__, __LINE__);
								break;
						}
					#endif
					}
					break;
				}
				case FILE_ACTION_RENAMED_NEW_NAME:
				{
					ctx->put_action(pBuffer->Action, old_filename, FullFilePath);
					printf("rename(new): %s \n", FullFilePath); break;
				}
			}
		}
	}
	CloseHandle(hFile);
    printf("[%s] thread exit\n", __FUNCTION__);
	return 	nullptr;
}


