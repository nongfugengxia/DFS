/*
 * DFS.c
 *
 *  Created on: 2016年12月28日
 *      Author: ubuntu
 */

#include "DFS.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "../client/fdfsClient.h"		// 在Makefile文件中的gcc参数中加入-I{}
#include "../DES/DES.h"
#include "../third/iniparser-master/src/iniparser.c"
#include "../third/jpegoptim-master/jpegoptim.h"



/*
 * 根据配置文件调用公共模块和存储模块的上传功能
 * 上传的是保存在缓存fileBuffer长度为fileSize的文件
 */
char* uploadToFastDFS(char* userName, char *fileBuffer, int fileSize, int quality) {
	if (ini == NULL) {
		return NULL;
	}

	// 经过公共模块
	int isCompress = -1;
	if (quality < 0 || quality > 100) {		// 用户的请求不满足条件就用系统的配置文件DFS中的配置进行压缩
		isCompress = iniparser_getboolean(ini, "common:uploadCompress", -1);	// 根据配置文件，是否需要压缩
		quality = iniparser_getint(ini, "common:uploadCompressQuality", -1);
	} else {
		isCompress = 1;
	}
	if (isCompress == 1 && quality >=0 && quality <= 100) {
		printf("进行图片压缩\n");
		fileBuffer = jpegoptimMain(fileBuffer, &fileSize, quality);
	}

	printf("上传文件大小: %d\n", fileSize);
	char* fileID = fdfsUploadByBuffer(userName, fileBuffer, fileSize);
	printf("上传返回的fileID: %s\n", fileID);

	int isEncrypt = iniparser_getboolean(ini, "common:encrypt", -1);	// 是否需要对链接进行加密
	if (isEncrypt == 1) {
		printf("需要加密\n");
		char* encryptedStr = encrypt(fileID);
		return encryptedStr;
	} else {
		return fileID;
	}
}


/*
 * 根据用户名和fileID下载文件，成功返回0，失败返回1
 * quality: 传递<0或者>100的值表示用户不进行压缩，利用DFS配置文件的值（用户>配置）
 */
int downloadFromFastDFS(char* userName, char* fileID, char **downloadBuffer, int64_t* fileLen, int quality) {
	// 解密，提取用户名，并判断是否合法，数据库中是否存在
	if (ini == NULL) {
		return EXIT_FAILURE;
	}

	int isEncrypt = iniparser_getboolean(ini, "common:encrypt", -1);	// 是否需要对链接进行解密
	char* decryptedFileID = NULL;
	if (isEncrypt == 1) {
		// TODO 内存中解密用户请求的链接
		decryptedFileID = decrypt(fileID);
		printf("解密后的字符串为: %s\n", decryptedFileID);
	} else {
		decryptedFileID = (char*)malloc(sizeof(char*)*strlen(fileID+1));
		strcpy(decryptedFileID, fileID);
	}

	char delims[] = "***";									// 判断删除用户名应该放在业务层，添加用户名还是应该放在存储层
	char *result = NULL;
	char* p;
	decryptedFileID = strtok_r(decryptedFileID, delims, &p);
	char * decryptedUserName = strtok_r(NULL, delims, &p);

	if (strcmp(userName, decryptedUserName) == 0) {			// 验证所所请求下载文件的用户是否合法(用户名是否包含在加密后的fileID中)
		fdfsdownloadReturnBuffer(userName, decryptedFileID, downloadBuffer, fileLen);
		printf("从FastDFS下载文件的长度为：%d\n", *fileLen);

		// 经过公共压缩模块
		int isCompress = -1;
		if (quality < 0 || quality > 100) {
			isCompress = iniparser_getboolean(ini, "common:downloadCompress", -1);
			quality = iniparser_getint(ini, "common:downloadCompressQuality", -1);
		} else {
			isCompress = 1;
//			quality = 100;
		}
		if (isCompress == 1 && quality >=0 && quality <= 100) {
			printf("进行图片压缩\n");
			char* retDownloadBuffer = jpegoptimMain(*downloadBuffer, fileLen, quality);
			unsigned int compressedFileLen = *fileLen;
			memcpy(*downloadBuffer, retDownloadBuffer, *fileLen);	// 将压缩后的数据（在retDownloadBuffer中，长度为*fileLen）复制回downloadBuffer (改变了decryptedFileID？？)
		}

		printf("下载: %s......\n", decryptedFileID);
		free(decryptedFileID);
		return EXIT_SUCCESS;
	} else {
		free(decryptedFileID);
		return EXIT_FAILURE;
	}
}


/*
 * 根据用户名和fileID删除FastDFS中的文件，成功返回0，失败返回1
 */
int delete(char* userName, char* fileID) {
	int isEncrypt = iniparser_getboolean(ini, "common:encrypt", -1);	// 是否需要对链接进行加密，加密了就需要解密
	if (isEncrypt == 1) {
		fileID = decrypt(fileID);
	}

	char delims[] = "***";
	char *result = NULL;
	fileID = strtok( fileID, delims );	// 解密后真正的fileID
	char * decryptedUserName = strtok(NULL, delims);		// 解密后真正的userName
	int ret = -1;
	if (strcmp(userName, decryptedUserName) == 0) {
		ret = fdfsDelete(decryptedUserName, fileID);		// 返回等于0表示删除成功，否则删除失败
		if (ret == 0) {
			printf("删除 %s 完成.\n", fileID);
		} else {
			printf("删除 %s 失败.\n", fileID);
		}
	}
	return ret;
}


/*
 * 系统初始化:
 * 1. 读取配置文件
 * 2. 构建内存池，为下载文件提前申请好内存
 */
int initDFS(char* ini_name) {

	printf("init DFS system...\n");
	ini = iniparser_load(ini_name);
	if (ini == NULL) {
		fprintf(stderr, "cannot parse file: %s\n", ini_name);
		return EXIT_FAILURE;
	}

	// TODO 构建内存池
	downloadBuffer = (char*)malloc(sizeof(char)*(100*1024*1024));

	// TODO 不存在就生成密钥，存在就将密钥读入内存

	return EXIT_SUCCESS;
}


int destroyDFS() {
	free(downloadBuffer);
	iniparser_freedict(ini);	// 释放配置文件的信息
	printf("释放内存!\n");
	return EXIT_SUCCESS;
}


/*
 * 每次调用上传、下载、删除时候都需要经过这个，根据判断是否需要进行某些公共的功能
 */


/*
int main(int argc, char *argv[]) {
	char * ini_name	= "./conf/DFS.conf";
	initDFS(ini_name);
	int isCompress = iniparser_getboolean(ini, "common:compress", -1);
//	printf("compress:	[%d]\n", isCompress);
	char* userName = "haidong.wang";
	char* localFileName = "./data/terrain.jpg";
	printf("\n");


	// 测试从buffer中上传
	printf("测试从buffer中上传\n");
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

	char* fileIDReturnByBuffer = uploadToFastDFS(userName, fileBuffer, fileSize, -1);
	printf("测试根据buffer上传文件返回的文件ID: %s\n", fileIDReturnByBuffer);
	printf("\n");


	// 测试下载文件到buffer
	printf("测试下载到buffer\n");
	int64_t returnFileLen = 0;
	downloadFromFastDFS(userName, fileIDReturnByBuffer, &downloadBuffer, &returnFileLen, 10);	// 程序崩溃
	printf("返回文件的长度：%d\n", returnFileLen);
	FILE *fpReturnBuffer = fopen("./data/download/testDownloadFromDFS.jpg","w");	// malloc(): memory corruption: 0x0000000000623010 ***
	fwrite(downloadBuffer, returnFileLen, 1, fpReturnBuffer);
	fclose(fpReturnBuffer);
	printf("\n");


//	 测试删除: group1/M01/05/5A/ChMTFlhkd8OAOutqAAcV_BSXtbY301.jpg\\haidong.wang
//	char* testDelFileID = "group1/M01/05/5A/ChMTFlhkepiACO8eAAcV_A9KqhY487.jpg";
	delete(userName, fileIDReturnByBuffer);
	printf("\n");


	// 测试DES加密
//	char* str = "G1/M01/05/7E/ChMTF1hFNZmANa82AAATmaFVJSI297.jpg//haidong.wang";	printf("最初的字符串为: %s\n", str);
//	char* encryptedStr = encrypt(str);								printf("加密后的字符串为: %s\n", encryptedStr);
//	char* decryptedStr = decrypt(encryptedStr);						printf("解密后的字符串为: %s\n", decryptedStr);



	printf("完成测试!\n");
	destroyDFS();

	return EXIT_SUCCESS;	// 使用 return，则 main 函数的栈帧将回收并失效;调用 exit() 时，此时 main() 函数的栈帧仍然是有效的
}
*/




















