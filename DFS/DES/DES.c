/*
 * DES.c
 *
 *  Created on: 2016年12月26日
 *      Author: ubuntu
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>


void generateKey() {
//	char argNum = 3;
//	char* argList[] = {(char*)(argNum), "-g", "/home/ubuntu/install/eclipse/workspace/DFS/DES/data/keyfile.key"};
//	runDESMain(argNum, argList);
	execl("./DES/run_des.o", "run_des.o", "-g", "./DES/data/keyfile.key", (char *)0);
}


/*
 * 将字符串str用./DES/data/keyfile.key中的密钥加密并返回
 * ./run_des.o -e /home/ubuntu/install/eclipse/workspace/DFS/DES/data/keyfile.key /home/ubuntu/install/eclipse/workspace/DFS/DES/data/data.txt /home/ubuntu/install/eclipse/workspace/DFS/DES/data/data.enc
 * 相对路径不行
 */
char* encrypt(char* str) {
	char cwd[80];
	getcwd(cwd, sizeof(cwd));	// 当前路径：/home/ubuntu/install/eclipse/workspace/DFS
//	printf("当前路径：%s\n", cwd);

	FILE *fp = fopen("./DES/data/data.txt","w");	// 打开只写文件，若文件存在则长度清为0，即该文件内容消失，若不存在则创建该文件。
	fwrite(str, strlen(str),1, fp);	// 写入需要加密的字符串
	fclose(fp);

	int pid = 1;
	if ((pid = fork()) < 0) {	// execl是覆盖进程的代码段，所以如果你原来的程序还需要正常退出的话，就要fork一个子进程
		perror("fork()\n");
	} else if (pid == 0) {
//		char exeFilePath[80];  strcpy(exeFilePath, cwd);  strcat(exeFilePath, "/DES/DES/run_des.o");
//		char keyFilePath[80];  strcpy(keyFilePath, cwd);  strcat(keyFilePath, "/DES/data/keyfile.key");
//		char srcDataFilePaht[80];  strcpy(srcDataFilePaht, cwd);	strcat(srcDataFilePaht, "/DES/data/data.txt");
//		char encDataFilePath[80];  strcpy(encDataFilePath, cwd);  strcat(encDataFilePath, "/DES/data/data.enc");
		execl("./DES/run_des.o", "./run_des.o", "-e", "./DES/data/keyfile.key", "./DES/data/data.txt", "./DES/data/data.enc", (char *)0);
		exit(0);
	}
	wait(NULL);	//等子进程，防止它成为僵尸

	FILE *encFp;
	if ((encFp = fopen("./DES/data/data.enc", "rb")) == NULL) {
		printf("打开加密后的文件data.enc失败！\n");
		exit(0);
	}
	fseek(encFp, 0, SEEK_END);
	int fileLen = ftell(encFp);		// 文件大小
	char* encStr = (char*)malloc(sizeof(char)*(fileLen+1));	// 必须得+1，为了存下字符串末尾的\0结束符
//	char encStr[fileLen];
	fseek(encFp, 0, SEEK_SET);
//	printf("file length : %d\n", fileLen);
	fread(encStr, fileLen, sizeof(char), encFp);	// 读取已经加密的字符串
	encStr[fileLen] = '\0';	//必须加字符串结束符，不然会大于文件原来的长度!!!
	fclose(encFp);
//	printf("encStr len : %d\n", strlen(encStr));

	return encStr;
}


/*
 * 将字符串str解密
 */
char* decrypt(char* str) {
//	printf("%d\n", strlen(str));
	FILE *encFp = fopen("./DES/data/data.enc","w");
	fwrite(str, strlen(str),1, encFp);	// 写入需要解密的字符串
	fclose(encFp);

	int pid = 1;
	if ((pid = fork()) < 0) {	// execl是覆盖进程的代码段，所以如果你原来的程序还需要正常退出的话，就要fork一个子进程
		perror("fork()\n");
	} else if (pid == 0) {
		execl("./DES/run_des.o", "run_des.o", "-d", "./DES/data/keyfile.key", "./DES/data/data.enc", "./DES/data/data.txt", (char *)0);
//		printf("解密成功！\n");
		exit(0);		// 完成后子线程注意要退出
	}
	wait(NULL);	//等子进程，防止它成为僵尸

	FILE *decFp;
	if ((decFp = fopen("./DES/data/data.txt", "rb")) == NULL) {
		printf("open failed\n");
		exit(0);
	}
	fseek(decFp, 0, SEEK_END);
	int fileLen = ftell(decFp);
	char* decStr = (char*)malloc(sizeof(char)*(fileLen+1));
	fseek(decFp, 0, SEEK_SET);
	fread(decStr, fileLen, sizeof(char), decFp);	// 读取已经加密的字符串
	decStr[fileLen] = '\0';	// 加上字符串结束符号
	fclose(decFp);

	return decStr;
}


//int main(int argc, char *argv[]) {
//
//	clock_t start, finish;
//	double time_taken;
//	start = clock();
//
////	generateKey();
//	char* str = "G1/M01/05/7E/ChMTF1hFNZmANa82AAATmaFVJSI297.jpg";
//	printf("最初的字符串为: %s\n", str);
//	char* encryptedStr = encrypt(str);
//	printf("加密后的字符串为: %s\n", encryptedStr);
//
//	char* decryptedStr = decrypt(encryptedStr);
//	printf("解密后的字符串为: %s\n", decryptedStr);
//
//	finish = clock();
//	time_taken = (double)(finish - start)/(double)CLOCKS_PER_SEC;
//	printf("完成加密和解密. 共耗时: %lf seconds.\n", time_taken);	// 0.000498 seconds.
//
//	return 1;
//}


