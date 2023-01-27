#ifndef __SFTP_UPLOAD_H__
#define __SFTP_UPLOAD_H__


int sftpUploadInit(const char *localpath);

int sftpUploadDeinit();

int sftpUpload(const char *localpath, const char *remotepath);
int sftpMkdir(const char *dir, const char *remote);
int sftpCMD(const char *cmd, const char *dst, const char *remote);
int sftpCMD2(const char *cmd, const char *remote);


#endif

