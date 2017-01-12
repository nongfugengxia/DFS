#include <stdio.h>

#include "server.h"

#define USAGE "Usage is:\ntransfer -c 'HOST IP'\ntransfer -s 'path/to/file'\n"

int main(int argc, char *argv[])
{
	char* transferFile = "/home/ubuntu/install/eclipse/workspace/DFS/data/terrain.jpg";
	printf("服务器模式...\n");
	server(transferFile);

	return 0;
}
