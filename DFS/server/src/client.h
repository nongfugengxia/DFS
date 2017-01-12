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

int runclient(int);

int client(char *);

#endif
