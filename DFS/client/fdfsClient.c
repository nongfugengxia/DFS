/*
 * fdfs_upload.c
 *
 *  Created on: 2016年12月24日
 *      Author: ubuntu
 */

#include <stdio.h>
#include <string.h>
#include <sys/types.h>

#include "fdfs_delete_file.h"
#include "fdfs_download_file.h"
#include "fdfs_upload_file.h"


/*
 * userName: 上传文件的用户名
 * localFileName: 上传本地文件的路径名
 *
 * return: 用户名||fileID
 */
char* fdfsUpload(char* userName, char* locaFileName) {
	char* configFile = "./conf/client.conf";

	char argNum = 3;
	char* argList[] = {&argNum, configFile, locaFileName};
	char* fileID = uploadMain(argNum, argList);
	strcat(fileID, "***");
	strcat(fileID, userName);
	return fileID;
}

char *fdfsUploadByBuffer(char* userName, char *fileBuffer, int fileSize) {
	// 测试根据buffer上传
	// char* uploadByBuffer(char* confFileName, const char *fileBuffer, const int64_t fileSize)
	char* configFile = "./conf/client.conf";
	char* fileID = uploadByBuffer(configFile, fileBuffer, fileSize);
	strcat(fileID, "***");
	strcat(fileID, userName);
	return fileID;
}


/*
 * 文件下载
 * userName: 用户名
 * fileID: 文件的ID
 * 自定义下载文件的路径
 */
int fdfsdownload(char* userName, char* fileID) {
	char* configFile = "./conf/client.conf";

	char argNum = 4;
	char local_filename[128];
	char* fileIDLast = strrchr(fileID, '/');
	fileIDLast++;	// 跳过 /
	snprintf(local_filename, 70, "%s%s", "./data/download/", fileIDLast);
	printf("下载保存的本地文件名: %s\n", local_filename);
	char* argList[] = {&argNum, configFile, fileID, local_filename};
	downloadMain(argNum, argList);

	return 1;
}


/*
 * 根据fileID下载文件，保存到char*的缓存中而不是保存到本地文件
 */
void fdfsdownloadReturnBuffer(char* userName, char* fileID, char **downloadBuffer, int64_t* fileLen) {
	char* configFile = "./conf/client.conf";

//	char argNum = 4;
//	char local_filename[80];
//	char* fileIDLast = strrchr(fileID, '/');
//	fileIDLast++;	// 跳过 /
//	snprintf(local_filename, 70, "%s%s", "./data/download/", fileIDLast);
//	printf("下载保存的本地文件名: %s\n", local_filename);
//	char* argList[] = {&argNum, configFile, fileID, local_filename};
//	downloadMain(argNum, argList);
	downloadReturnBuffer(configFile, fileID, downloadBuffer, fileLen);

//	FILE *fpReturnBuffer = fopen("./data/download/testRet.jpg","w");
//	fwrite(retBuffer, *fileLen, 1, fpReturnBuffer);
//	fclose(fpReturnBuffer);

//	char* retBuffer = "test";
//	return retBuffer;
}


/*
 * FastDFS文件删除
 * userName: 用户名
 * fileID: 需要删除文件的fileID
 */
int fdfsDelete(char* userName, char* fileID) {
	char* configFile = "./conf/client.conf";

	char argNum = 3;
//	char* argList[] = {(char*)(argNum), configFile, fileID};
	char* argList[] = {&argNum, configFile, fileID};
	int deleteRet = deleteMain(argNum, argList);
	printf("删除文件！\n");

	return deleteRet;
}


/*
int main(int argc, char *argv[]) {
	// 命令行执行：./fdfs_upload ./client.conf client.conf
	// Eclipse run arguments: /home/ubuntu/install/eclipse/workspace/DFS/conf/client.conf /home/ubuntu/install/eclipse/workspace/DFS/data/libfdfsclient.so
//	uploadMain(argc, argv);

	char* userName = "haidong.wang";
	char* localFileName = "/home/ubuntu/install/eclipse/workspace/DFS/data/terrain.jpg";
	char* fileID = fdfsUpload(userName, localFileName);
	printf("return file id: %s\n", fileID);

	//测试文件下载 group1/M01/05/59/ChMTFlhiJd-A	GvIbAAcV_EQDGMw125.jpg\\haidong.wang
	char* testDownloadFileID = "group1/M01/05/5A/ChMTFlhiNC6AZKAVAAcV_GSr8Fg639.jpg";
	fdfsdownload(userName, testDownloadFileID);


	//测试文件删除 group1/M01/05/5A/ChMTFlhiLdaAIYxVAAcV_GzxcGE241.jpg\\haidong.wang
//	char* testDelFileID = "group1/M01/05/5A/ChMTFlhiM_GAKavBAAcV_H0zyuk207.jpg";
//	fdfsDelete(userName, testDelFileID);

	//测试文件下载 group1/M01/05/59/ChMTFlhiJd-A	GvIbAAcV_EQDGMw125.jpg\\haidong.wang
//	char* testDownloadFileID2 = "group1/M01/05/5A/ChMTFlhiM_GAKavBAAcV_H0zyuk207.jpg";
//	fdfsdownload(userName, testDownloadFileID2);
	return 0;
}
*/



/*
调用另一个.c文件中的函数（调用上传函数）：
1. 将int main(int argc, char *argv[])转换成uploadMain(int argc, char *argv[])
2. 增加fdfs_upload_file.h, int uploadMain(int argc, char *argv[]);
3. fdfs_upload_file.c中增加#include "fdfs_upload_file.h"
4. 在使用的地方增加头文件 #include "fdfs_upload_file.h"
5. Makefile.in中增加：FDFS_HEADER_FILES:	fdfs_upload_file.h
					FDFS_STATIC_OBJS:	fdfs_upload_file.o
					ALL_PRGS:			fdfsClient(fdfs_upload), 还需要删除fdfs_download_file（不然会出现undefined reference to `main'）

git init
git add .
git commit -m 'first commit'
git remote add master http://phint.horizon-robotics.com/diffusion/DB/distributeddb.git
git push master master
 */
