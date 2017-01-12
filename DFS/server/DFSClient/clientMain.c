#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "client.h"


/*
 * 1.上传: userName, fileBuffer, fileLen, quality, 返回 fileID
 * 2.下载: userName, fileID, quality; 返回fileBuffer, fileLen
 * 3.删除: userName, fileID
 */
int main(int argc, char *argv[])
{
	char* userName = "haidong.wang";
	char* hostIP = "127.0.0.1";	// 作为配置文件
//	char* localFileName = "./data/terrain.jpg";

	//************************************************************************************************************
	printf("测试从buffer中上传\n");
	FILE *fp;
	if ((fp = fopen("/home/ubuntu/install/eclipse/workspace/DFS/data/terrain.jpg", "rb")) == NULL) {
		printf("打开文件失败\n");
		exit(0);
	}
	fseek(fp, 0, SEEK_END);
	int uploadFileSize = ftell(fp);
	char* uploadFileBuffer = (char*)malloc(sizeof(char)*(uploadFileSize+1));
	fseek(fp, 0, SEEK_SET);
	fread(uploadFileBuffer, uploadFileSize, sizeof(char), fp);	// 读取已经加密的字符串
	uploadFileBuffer[uploadFileSize] = '\0';	// 加上字符串结束符号
	fclose(fp);

	int uploadAction = 1;
	int uploadQuality = -1;
	char uploadFileSizeStr[20];
	char uploadQualityStr[3];
	sprintf(uploadFileSizeStr,"%d",uploadFileSize);
	sprintf(uploadQualityStr,"%d",uploadQuality);
	char* uploadArgList[] = {userName, uploadFileBuffer, uploadFileSizeStr, uploadQualityStr};
	int uploadStrLen = 0;
	char* testUploadRetFileID = client(hostIP, uploadAction, uploadArgList, &uploadStrLen);
	printf("上传完成, 返回的fileID为：%s\n", testUploadRetFileID);
	printf("fileID长度为: %d\n\n", uploadStrLen);


	//************************************************************************************************************
	printf("测试下载到Buffer\n");
	int action = 2;	// 2表示下载文件
//	char* testDownloadFileID = "group1/M01/05/5F/ChMTFlhspNSADAHZAAcV_Ha0SRA3946051***haidong.wang";
	char* quality = "8";
//	char* argList[] = {userName, testDownloadFileID, quality};
	char* argList[] = {userName, testUploadRetFileID, quality};
	time_t begin = clock();
	int strLen = -1;
	char* buffer = client(hostIP, action, argList, &strLen);
	printf("下载所用的时间为：%f 秒.\n", (double)(clock()-begin)/1000);

	FILE* file;
	char* filename = "/home/ubuntu/install/eclipse/workspace/DFS/data/download/terrainSaveInClient.jpg";	// 临时改变客户端保存文件的路径
	if((file = fopen(filename, "wb"))==NULL){
		fprintf(stderr, "打开文件失败.\n");
		exit(2);
	}
	fwrite (buffer , 1 , strLen, file); // 收到的文件大小必须和原来的文件大小一致(This MUST be the same size of the original file)
	printf("文件传输完成，关闭文件....\n\n");
	free(buffer);


	//************************************************************************************************************

	printf("测试删除\n");
//	char *testDeleteFileID = "group1/M01/05/68/ChMTFlh16GqAF5zWAAHTy3xWxUs0113526***haidong.wang";
//	char* deleteArgList[] = {userName, testDeleteFileID};	// 删除失败
	char* deleteArgList[] = {userName, testUploadRetFileID};	// 删除成功
	int deleteAction = 3;
	int deleteStrLen = -1;
	char* isDeleteSuccess = client(hostIP, deleteAction, deleteArgList, &deleteStrLen);
	printf("返回删除的长度(0表示删除成功，1表示删除失败)：%d\n", deleteStrLen);
	printf("请求删除返回的信息：%s\n", isDeleteSuccess);

	return 0;
}






















