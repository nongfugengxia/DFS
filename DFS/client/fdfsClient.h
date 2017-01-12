/*
 * fdfsClient.h
 *
 *  Created on: 2016年12月28日
 *      Author: ubuntu
 */

#ifndef CLIENT_FDFSCLIENT_H_
#define CLIENT_FDFSCLIENT_H_

char* fdfsUpload(char* userName, char* locaFileName);
int fdfsdownload(char* userName, char* fileID);
int fdfsDelete(char* userName, char* fileID);

char *fdfsUploadByBuffer(char* userName, char *fileBuffer, int fileSize);
void fdfsdownloadReturnBuffer(char* userName, char* fileID, char **downloadBuffer, int64_t* fileLen);

#endif /* CLIENT_FDFSCLIENT_H_ */
