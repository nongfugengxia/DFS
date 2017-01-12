/*
 * fdfs_upload_file.h
 *
 *  Created on: 2016年12月24日
 *      Author: ubuntu
 */

#ifndef CLIENT_TEST_FDFS_UPLOAD_FILE_H_
#define CLIENT_TEST_FDFS_UPLOAD_FILE_H_

char* uploadMain(int argc, char *argv[]);
char* uploadByBuffer(char* confFileName, char *fileBuffer, int fileSize);

#endif /* CLIENT_TEST_FDFS_UPLOAD_FILE_H_ */
