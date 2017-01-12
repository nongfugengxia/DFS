/*
 * testcase.c
 *
 *  Created on: 2017年1月3日
 *      Author: ubuntu
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <sys/types.h>

#include "fdfsClient.h"

int main(int argc, char *argv[]) {
	char* userName = "haidong.wang";
	char* localFileName = "./data/terrain.jpg";
	char* fileID = fdfsUpload(userName, localFileName);
	printf("根据文件路径和文件名上传返回的文件ID: %s\n\n", fileID);


	// 测试根据buffer上传文件 char *fdfsUploadByBuffer(char* userName, char *fileBuffer, int fileSize)
	FILE *fp;
	if ((fp = fopen("./data/terrain.jpg", "rb")) == NULL) {
		printf("打开文件失败\n");
		exit(0);
	}
	fseek(fp, 0, SEEK_END);
	int fileSize = ftell(fp);
	char* fileBuffer = (char*)malloc(sizeof(char)*(fileSize+1));
	fseek(fp, 0, SEEK_SET);
	fread(fileBuffer, fileSize, sizeof(char), fp);	// 读取已经加密的字符串
	fileBuffer[fileSize] = '\0';	// 加上字符串结束符号
	fclose(fp);

	char* fileIDReturnByBuffer = fdfsUploadByBuffer(userName, fileBuffer, fileSize);
	printf("测试根据buffer上传文件返回的文件ID: %s\n\n", fileIDReturnByBuffer);



	//测试文件下载 group1/M01/05/5F/ChMTFlhspNSADAHZAAcV_Ha0SRA3946051
	char* testDownloadFileID = "group1/M01/05/5F/ChMTFlhspNSADAHZAAcV_Ha0SRA3946051";
//	fdfsdownload(userName, testDownloadFileID);

	int64_t returnFileLen = 0;
	char *downloadBuffer = (char*)malloc(sizeof(char)*(100*1024*1024));
	fdfsdownloadReturnBuffer(userName, testDownloadFileID, &downloadBuffer, &returnFileLen);
	printf("返回文件的长度：%d\n", returnFileLen);
	FILE *fpReturnBuffer = fopen("./data/download/testRet.jpg","w");
	fwrite(downloadBuffer, returnFileLen, 1, fpReturnBuffer);
	fclose(fpReturnBuffer);


	//测试文件删除 group1/M01/05/5A/ChMTFlhiLdaAIYxVAAcV_GzxcGE241.jpg\\haidong.wang
//	char* testDelFileID = "group1/M01/05/5A/ChMTFlhiM_GAKavBAAcV_H0zyuk207.jpg";
//	fdfsDelete(userName, testDelFileID);

	//测试文件下载 group1/M01/05/59/ChMTFlhiJd-A	GvIbAAcV_EQDGMw125.jpg\\haidong.wang
//	char* testDownloadFileID2 = "group1/M01/05/5A/ChMTFlhiM_GAKavBAAcV_H0zyuk207.jpg";
//	fdfsdownload(userName, testDownloadFileID2);
	return 0;
}
