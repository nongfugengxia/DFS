#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <unistd.h>

#include "client.h"

const int port = 1234;
const int kb_size = 1024;
extern int errno; // 用于产生一般错误代码 Used for general error codes

//int client(char *host_ip)	// 连接服务端的IP地址
/*
 * host_ip: 服务器的IP地址
 * action:	执行所需要的动作, 1表示上传，2表示下载，3表示删除
 * argList:
 * 1上传：argList[0]为用户名，argList[1]为fileBuffer, argList[2]fileBuffer(文件)长度, argList[3]为图片压缩质量
 * 2下载: argList[0]为用户名，argList[1]为下载文件的FileID，argList[2]为下载图片所需的图片质量
 * 3删除：argList[0]为用户名，argList[1]为所要删除文件的fileID
 *
 * 返回：
 * 上传：char* fileID, int strLen(可以'\0'结束，失败-1)
 * 下载：char* buffer, int strLen(失败-1)
 * 删除：char* success/failed, int 0表示删除成功，1表示删除失败
 * 合并，return char* buffer, (int * strLen)
 */
char* client(char* host_ip, int action, char* argList[], int* strLen)
{
	struct addrinfo *servinfo = 0;
	int sockfd = 0;

	int connect_status = 0;

	servinfo = filladdrinfoc(host_ip, servinfo);
/*
	打开套接字；Open socket (domain, type, protocol)
	domain——套接字通信域——的取值：AF_INET,AF_INET6，AF_UNIX,AF_UNSPEC
	type————套接字类型——--的取值：SOCK_DGRAM,SOCK_RAW,SOCK_SEQPACKET,SOCK_STREAM
	protocal——————————的取值：通常为0，表示按给定的域或套接字类型选择默认协议。当对同一域和套接字类型支持多个协议时，
							可以使用protocol参数选择一个特定协议
	返回一个套接字描述符
*/	
	if((sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol))==-1){
		fprintf(stderr, "Socket error: %i\n", errno);
		exit(5);
	}

	if((connect_status = connect(sockfd, servinfo->ai_addr, servinfo->ai_addrlen))==0){
		printf("Connected on port: %i \n",port);
		char* retBuffer = runclient(sockfd, action, argList, strLen);
		return retBuffer;
	}
	else{
		fprintf(stderr, "%i, %i\n", connect_status, errno);
	}

	freeaddrinfo(servinfo);

	return 0;
}

/*
 * 填充服务器的地址信息
 */
void *filladdrinfoc(char *host_ip, struct addrinfo *servinfo)
{
	int status = 0;
	struct addrinfo hints;	// 服务提供者的地址信息
	char port_a[8] = {0};

	snprintf(port_a, 8, "%d",port); 
	// 开始之前确保服务提供者的地址信息为空(Make sure hints is empty before we begin)
	memset(&hints, 0, sizeof hints);
	// 填充一些初始化信息(Fill hints with some initial 'hint' info)
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
/*
	Make a call to getaddrinfo with partially filled struct hints, ip, port
	and struct to fill with addrinfo.
*/	
	if((status = getaddrinfo(host_ip, port_a, &hints, &servinfo))){
		fprintf(stderr, "Addrinfo error: %s\n", gai_strerror(status));
	}
	return servinfo;
}

/*
 * 在套接字sockfd中的rec_buffer中接收fileSize大小的文件，保存到file_buffer中
 * 返回接收到的整个文件的大小 d rec len tot
 */
long recvf(int sockfd, long filesize, char *rec_buffer, char *file_buffer)
{
	long dreclentot = 0;	// 接收到的整个文件的大小
	int dreclen = 0;
	long remaining_bytes = 0;	// 文件还有多大没有接收
	int packet = 0;			// 统计共接收到多少个数据包
	int i = 0;

	while(dreclentot!=filesize){
		// rec_buffer为接收数据的缓冲区，每次接收kb_size(1024)的大小(Write to rec buffer to account for TCP, then write all to file_buffer)
		if((dreclen = recv(sockfd, rec_buffer, kb_size, 0))==-1){
			fprintf(stderr, "接收数据失败(Receive error: %i)\n", errno);
		}
		else if (!dreclen){
			fprintf(stderr, "服务器关闭了连接(Host closed the connection).\n");
			exit(5);
		}

		dreclentot += dreclen;
		packet++;
		remaining_bytes = (filesize - dreclentot);
		printf("%g%%\n", ((float)dreclentot/(float)filesize) * 100);

		for (i = 0; i < dreclen; ++i){
			file_buffer[(dreclentot-dreclen)+i] = rec_buffer[i]; // 将接收到的数据拷贝到最后的文件缓存中(copy recieved this iteration to final file buffer)
		}
	}
	printf("接收 %i 个数据包(packets).\n", packet);

	printf("共接收到 %li 字节, %g%%\n",dreclentot, ((float)dreclentot/(float)filesize)*100);

	return dreclentot;
}


/*
 * 运行客户端的功能
 */
char* runclient(int sockfd, int action, char* argList[], int *strLen)
{
	switch (action) {
	case 1:
		printf("进行文件上传，用户名：%s, 上传文件大小：%s，上传文件压缩质量：%s\n", argList[0], argList[2], argList[3]);
		char uploadReqmsg[256] = {0};
		char uploadActionStr[1] = {0};
		memcpy(uploadActionStr, &action, sizeof(int));
		strncat(uploadReqmsg, uploadActionStr, strlen(uploadActionStr));
		strncat(uploadReqmsg, "###", strlen("###")+1);
		strncat(uploadReqmsg, argList[0], strlen(argList[0]));
		strncat(uploadReqmsg, "###", strlen("###")+1);
		strncat(uploadReqmsg, argList[2], strlen(argList[2]));
		strncat(uploadReqmsg, "###", strlen("###")+1);
		strncat(uploadReqmsg, argList[3], strlen(argList[3]));
		strncat(uploadReqmsg, "###", strlen("###")+1);
		printf("发送客户端给服务器的请求上传信息...\n");
		int uploadSendLength = send(sockfd, uploadReqmsg, sizeof uploadReqmsg, 0);
		printf("给服务器发送了 %d 字节的上传请求数据\n", uploadSendLength);

		printf("开始发送文件....\n");
		int uploadSendLen = atoi(argList[2]);
		int uploadSendedLen = sendf(sockfd, argList[1], uploadSendLen);
		printf("发送文件到服务器缓冲完成, 发送 %i 字节, %g%%\n",uploadSendedLen, ((float)uploadSendedLen/(float)uploadSendLen)*100);

		// 接收返回的fileID
		int recvUploadFileIDLen = 0;
		recv(sockfd, &recvUploadFileIDLen, sizeof(int), 0);
		*strLen = recvUploadFileIDLen;
		char* recvUploadFileID = (char*)malloc(sizeof(char)*(recvUploadFileIDLen+1));
		recv(sockfd, recvUploadFileID, recvUploadFileIDLen, 0);
		recvUploadFileID[recvUploadFileIDLen] = '\0';
		printf("接收到上传返回的fileID: %s\n", recvUploadFileID);
		return recvUploadFileID;
		break;
	//************************************************************************************************************

	case 2:
		printf("进行文件下载，用户名：%s, 文件ID：%s，文件质量：%s\n", argList[0], argList[1], argList[2]);
		// 先发送action和argList信息给server，再接收文件
		long filesize = 0;
		char *filename;
		char reqmsg[256] = {0};
		char response[2];
		long dreclentot = 0;

		char *rec_buffer;
		char *file_buffer;

		// 拼接发送的客户端发送给服务端的请求信息
		char actionStr[1] = {0};
		memcpy(actionStr, &action, sizeof(int));
		strncat(reqmsg, actionStr, strlen(actionStr));	// 请求信息中加入请求类型(action:1上传、2下载、3删除)
		strncat(reqmsg, "###", strlen("###")+1);
		strncat(reqmsg, argList[0], strlen(argList[0]));
		strncat(reqmsg, "###", strlen("###")+1);
		strncat(reqmsg, argList[1], strlen(argList[1]));
		strncat(reqmsg, "###", strlen("###")+1);
		strncat(reqmsg, argList[2], strlen(argList[2]));
		strncat(reqmsg, "###", strlen("###")+1);
		printf("发送客户端给服务器的下载请求信息...\n");
		int sendLength = send(sockfd, reqmsg, sizeof reqmsg, 0);
		printf("给服务器发送了 %d 字节的下载请求数据\n", sendLength);
		response[0] = 'Y';		// 用这个来发送请求的fileID
		// 接收文件数据
		int64_t recvFileLen = 0;
		recv(sockfd, &recvFileLen, sizeof(int64_t), 0);	// 发送的大小一定要和接收的大小一致，不然后面接收的数据就不一定是所需要的大小，服务器就关闭了连接
		printf("接收的文件长度为:%d\n", recvFileLen);
		if((rec_buffer = malloc(kb_size))==NULL){
			fprintf(stderr, "客户端接收缓冲区分配内存失败(Receive buffer malloc failed).\n");
			exit(3);
		}
		if((file_buffer = malloc(recvFileLen))==NULL){
			fprintf(stderr, "文件缓冲分配内存失败(File buffer malloc failed).\n");
			exit(4);
		}
		dreclentot = recvf(sockfd, recvFileLen, rec_buffer, file_buffer);
		if (dreclentot!=recvFileLen){
			fprintf(stderr, "Warning: 不是所有的字节都收到(Not all bytes received).\n");
		}
		free(rec_buffer);
		close(sockfd);
		*strLen = recvFileLen;
		return file_buffer;
		break;
	//************************************************************************************************************
	case 3:
		printf("进行文件下载删除，用户名：%s, 文件fileID：%s\n", argList[0], argList[1]);
		char deleteReqmsg[256] = {0};
		char deleteActionStr[1] = {0};
		memcpy(deleteActionStr, &action, sizeof(int));
		strncat(deleteReqmsg, deleteActionStr, strlen(deleteActionStr));
		strncat(deleteReqmsg, "###", strlen("###")+1);
		strncat(deleteReqmsg, argList[0], strlen(argList[0]));
		strncat(deleteReqmsg, "###", strlen("###")+1);
		strncat(deleteReqmsg, argList[1], strlen(argList[1]));
		strncat(deleteReqmsg, "###", strlen("###")+1);
		printf("发送客户端给服务器的请求删除信息...\n");
		int deleteSendLength = send(sockfd, deleteReqmsg, sizeof deleteReqmsg, 0);
		printf("给服务器发送了 %d 字节的删除请求数据\n", deleteSendLength);

		int deleteIsSuccess = -1;
		recv(sockfd, &deleteIsSuccess, sizeof(int), 0);
		if (deleteIsSuccess == 0) {
			*strLen = 0;
			return "delete success";
		} else {
			*strLen = 1;
			return "delete failed";
		}
		break;
	}
}


























