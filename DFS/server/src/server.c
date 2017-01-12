#include "server.h"

#include <asm-generic/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#include "../../middle/DFS.h"

const int backlog = 5; // Max 5 set by kernel ?
const int opttrue = 1;
const int nthreads = 5;		// 每次接收数据创建的线程数目

extern int errno;
extern int port;
extern int kb_size;

struct server_data
{
	int clientfd;
	int client;
	struct sockaddr_storage client_addr;
	char *buffer;
	char *filename;
	long lSize;
	int thread_flag;
};



int server(char *filename)
{
	char * ini_name	= "./conf/DFS.conf";
	initDFS(ini_name);

	struct addrinfo *servinfo = 0;
	struct sockaddr_storage client_addr;
	struct server_data thread_arg[nthreads];	// 传递给线程的参数
	socklen_t addr_size = sizeof client_addr;

	int sockfd = 0;
	int clientfd[nthreads];
	int client = 0;
	pthread_t thread[nthreads];
	long lSize = 0;
  	char * buffer;
  	int i = 0;

  	servinfo = filladdrinfos(servinfo);

	sockfd = makeserv(servinfo);

	// 监听1234端口号(Listen for incoming connections on 1234)
	if(listen(sockfd, backlog)==-1){
		fprintf(stderr, "Listen Error: %i\n", errno);
	}
	printf("监听端口(Listening on port): %i \n",port);

	client = 0;

	// 填充线程结构体参数 Fill struct with thread arguments
	for (i = 0; i < nthreads; ++i){
		memset(&thread_arg[i], 0, sizeof(thread_arg[i]));
		thread_arg[i].client_addr = client_addr;
		thread_arg[i].buffer = buffer;
		thread_arg[i].filename = filename;
		thread_arg[i].lSize = lSize;
	}

	// 接收连接，五路并行（一次性创建5个线程提供服务） Accept an incoming connection -> 5 in parallel
	while(1){
		while(client < nthreads){
			if ((clientfd[client] = accept(sockfd, (struct sockaddr *)&client_addr, &addr_size))){

				thread_arg[client].clientfd = clientfd[client];
				thread_arg[client].client = client;

				pthread_create(&thread[client], NULL, runserv, (void *)&thread_arg[client]);	// 建立子线程进行数据传输，不影响主线程的运行 buffer overflow detected
			}
			client++;
		}

		for (i = 0; i < nthreads; ++i){
			pthread_join(thread[i], NULL);		// 合并创造出来的线程 buffer overflow detected
		}
		client = 0;
		for (i = 0; i < nthreads; ++i){
			memset(&thread_arg[i], 0, sizeof(thread_arg[i]));
			thread_arg[i].client_addr = client_addr;
			thread_arg[i].buffer = buffer;
			thread_arg[i].filename = filename;
			thread_arg[i].lSize = lSize;
		}
	}

	//释放服务信息和其他各种各样分配的东西;Free our servinfo, and other assorted allocated goodies
	close(sockfd);
	freeaddrinfo(servinfo);
	free(buffer);

	return 0;
}

int sendf(int clientfd, char *buffer, int lSize)
{
	int dsentlen = 0;
	int packet = 0;
	int remaining_bytes = 0;
	while(dsentlen!=lSize){
		if((dsentlen += send(clientfd,&buffer[packet*kb_size], kb_size, 0))==-1){
			fprintf(stderr, "发送数据失败: %i\n", errno);
		}
		packet++;
		remaining_bytes = (lSize - packet*kb_size);
		if (remaining_bytes < kb_size){
			/* 发送最后小于1024字节的数据包（必须的） send the last packet  of < 1024 bytes, required */
			if((dsentlen += send(clientfd,&buffer[packet*kb_size], remaining_bytes, 0))==-1){
				fprintf(stderr, "Sending Error (last packet): %i\n", errno);
			}
			packet++;
		}
	}
	return dsentlen;
}


void *runserv(void *arg)
{
	char peeripstr[INET_ADDRSTRLEN];
	char reqmsg[256] = {0};
	char lSizea[100] = {0};
	char response[1] = {0};
	int msentlen = 0;
	int dsentlen = 0;

	struct server_data *local_data;

	local_data = (struct server_data *)arg;

	int clientfd = (int)local_data->clientfd;
	int client = (int)local_data->client;
	struct sockaddr_storage client_addr = (struct sockaddr_storage)local_data->client_addr;
	char *buffer = (char *)local_data->buffer;
	char *filename = (char *)local_data->filename;
	long lSize = (long)local_data->lSize;
	int thread_flag = (int)local_data->thread_flag;

	struct sockaddr_in* pV4Addr = (struct sockaddr_in*)&client_addr;
	int ipAddr = pV4Addr->sin_addr.s_addr;		
	inet_ntop(AF_INET, &ipAddr, peeripstr, sizeof peeripstr);	//  [将"点分十进制" -> "整数"]

	printf("Client %i connected from %s.\n",client, peeripstr);

	snprintf(lSizea, 100, "%li",lSize);

	//************************************************************************************************************
	recv(clientfd, reqmsg, sizeof(reqmsg), 0);		// 接收到客户端所发送到来的请求信息
	char* actionStr = strtok(reqmsg, "###"); // Split on ":"
	int action = (int)actionStr[0];		// 根据所传递过来action的不同进行不同的处理

	switch (action) {
	case 1:
		printf("进行文件上传，");
		char* uploadUserName = strtok(NULL, "###");
		char* uploadFileSize = strtok(NULL, "###");
		char* uploadQualityStr = strtok(NULL, "###"); int uploadQuality = atoi(uploadQualityStr);
		printf("接收到客户端的请求: action: %s, userName: %s, uploadFileSize: %s, uploadQuality: %s\n", actionStr, uploadUserName, uploadFileSize, uploadQualityStr);

		// 上传时，服务端接收文件
		char *uploadRecvBuffer;		// 上传文件所用的接收字符的缓冲区
		char *uploadFileBuffer;			// 上传文件时，所用的存储文件的缓冲区
		if((uploadRecvBuffer = malloc(kb_size))==NULL) {
			fprintf(stderr, "客户端接收缓冲区分配内存失败(Receive buffer malloc failed).\n");
			exit(3);
		}
		int uploadRecvFileLen = atoi(uploadFileSize);
		if((uploadFileBuffer = malloc(uploadRecvFileLen))==NULL){
			fprintf(stderr, "文件缓冲分配内存失败(File buffer malloc failed).\n");
			exit(4);
		}
		int uploadRecvedFileLen = recvf(clientfd, uploadRecvFileLen, uploadRecvBuffer, uploadFileBuffer);
		if (uploadRecvedFileLen != uploadRecvFileLen) {
			fprintf(stderr, "Warning: 不是所有的字节都收到(Not all bytes received).\n");
		}
		free(uploadRecvBuffer);
		printf("文件上传到服务器缓冲完成, 接收到的文件字节数目为：%d\n", uploadRecvedFileLen);

		// 将服务器缓冲中的数据上传至FastDFS
		char* fileIDReturnByUpload = uploadToFastDFS(uploadUserName, uploadFileBuffer, uploadRecvFileLen, uploadQuality);
		printf("测试根据buffer上传文件返回的文件ID: %s\n", fileIDReturnByUpload);

		// 将返回的fileID发送给客户端
		int uploadRetFileIDLen = strlen(fileIDReturnByUpload);
		send(clientfd, &uploadRetFileIDLen, sizeof(int), 0);	//发送fileID的长度给客户端
		send(clientfd, fileIDReturnByUpload, strlen(fileIDReturnByUpload), 0);
		printf("发送返回的fileID给客户端成功\n");

		break;


	case 2:
		printf("进行文件下载，");
		char* userName = strtok(NULL, "###");
		char* fileID = strtok(NULL, "###");
		char* quality = strtok(NULL, "###");
		printf("接收到客户端的请求: action: %s, userName: %s, fileID: %s, quality: %s\n", actionStr, userName, fileID, quality);

		int64_t returnFileLen = 0;
		int qualityInt = atoi(quality);
		downloadFromFastDFS(userName, fileID, &downloadBuffer, &returnFileLen, qualityInt);	// 程序崩溃
		printf("返回文件的长度：%d\n", returnFileLen);

		printf("发送文件长度给客户端，以便于构造缓冲来接收数据\n");
		send(clientfd, &returnFileLen, sizeof(int64_t), 0);

		printf("开始发送文件....\n");
		dsentlen = sendf(clientfd, downloadBuffer, returnFileLen);
		printf("发送文件完成, 发送 %i 字节, %g%%\n",dsentlen, ((float)dsentlen/(float)returnFileLen)*100);
		thread_flag = 0;
		break;


	case 3:
		printf("进行文件删除，");
		char* deleteUserName = strtok(NULL, "###");
		char* deleteFileID = strtok(NULL, "###");
		printf("接收到客户端的删除请求: action: %s, userName: %s, fileID: %s\n", actionStr, deleteUserName, deleteFileID);
		int deleteSuccess = delete(deleteUserName, deleteFileID);
		send(clientfd, &deleteSuccess, sizeof(int), 0);
		printf("发送删除是否成功的信息给客户端\n");
		break;
	}
	//************************************************************************************************************
	thread_flag = 1;
	close(clientfd);

	return NULL;
}

void *filladdrinfos(struct addrinfo *servinfo)
{
	int status = 0;
	struct addrinfo hints;
	char port_a[8] = {0};

	snprintf(port_a, 8, "%d",port);
/*
	Make sure hints is empty before we begin
*/
	memset(&hints, 0, sizeof hints);
/*
	Fill hints with some initial 'hint' info
*/
	hints.ai_family = AF_INET; // ipv4
	hints.ai_socktype = SOCK_STREAM; // TCP
	hints.ai_flags = AI_PASSIVE; // use my IP
/*
	Make a call to getaddrinfo with partially filled struct hints, ip, port
	and struct to fill with addrinfo.
*/	
	if((status = getaddrinfo(NULL, port_a, &hints, &servinfo))){
		fprintf(stderr, "Addrinfo error: %s\n", gai_strerror(status));
	}

	return servinfo; // Pointer, servinfo modified
}

int makeserv(struct addrinfo *servinfo)
{
	int sockfd;
	if((sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol))==-1){
		fprintf(stderr, "Socket error: %i\n", errno);
		exit(5);
	}

	if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opttrue, sizeof(opttrue))==-1){
		fprintf(stderr, "Socket Option Error: %i\n", errno);
	}

	if(bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen)==-1){
		fprintf(stderr, "Bind Error: %i\n", errno);
		exit(6);
	}
	return sockfd;
}
