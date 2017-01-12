#ifndef _server_h
#define _server_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <unistd.h>

void *filladdrinfos(struct addrinfo *);

int sendf(int, char *, int); // Already an implementation in socket.h

void *runserv(void *arg);

int makeserv(struct addrinfo *);

int server(char *);


#endif
