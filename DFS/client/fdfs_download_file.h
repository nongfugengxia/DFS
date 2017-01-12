/*
 * fdfs_download_file.h
 *
 *  Created on: 2016年12月27日
 *      Author: ubuntu
 */

#ifndef CLIENT_FDFS_DOWNLOAD_FILE_H_
#define CLIENT_FDFS_DOWNLOAD_FILE_H_

int downloadMain(int argc, char *argv[]);
void downloadReturnBuffer(char* configFile, char* fileID, char** downloadBuffer, int64_t* fileLen);

#endif /* CLIENT_FDFS_DOWNLOAD_FILE_H_ */
