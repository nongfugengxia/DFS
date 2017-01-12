/*
 * DFS.h
 *
 *  Created on: 2017年1月10日
 *      Author: ubuntu
 */

#ifndef DFS_H_
#define DFS_H_

#include "../third/iniparser-master/src/dictionary.h"

dictionary *ini;		// 存放配置文件的信息（全局变量，上传、下载都需要用到）
char *downloadBuffer;

char* uploadToFastDFS(char* userName, char *fileBuffer, int fileSize, int quality);
int downloadFromFastDFS(char* userName, char* fileID, char **downloadBuffer, int64_t* fileLen, int quality);
int delete(char* userName, char* fileID);

#endif /* DFS_H_ */
