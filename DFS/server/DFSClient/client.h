#ifndef _client_h
#define _client_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <ctype.h>

void *filladdrinfoc(char *, struct addrinfo *);

long recvf(int, long, char *, char *);

//int runclient(int);
//int runclient(int sockfd, int action, char* argList[]);
char* runclient(int sockfd, int action, char* argList[], int *strLen);

//int client(char *);
//int client(char* host_ip, int action, char* argList[]);
char* client(char* host_ip, int action, char* argList[], int* strLen);


#endif
