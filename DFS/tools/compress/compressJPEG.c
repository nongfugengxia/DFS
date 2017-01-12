/*
 * compressJPEG.c
 *
 *  Created on: 2017年1月5日
 *      Author: ubuntu
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../../third/jpegoptim-master/jpegoptim.h"

/*
 * 测试压缩文件接口(位于./thrid/jpegoptim-master目录下，修改jpegoptim.c)
 * 从本地读取文件 压缩后 再写入本地磁盘
 */
int main() {
//	char* filePath = "/home/ubuntu/install/eclipse/workspace/DFS/data/terrain.jpg";
	FILE *fp;
	if ((fp = fopen("./data/terrain.jpg", "rb")) == NULL) {
		printf("打开文件失败\n");
		exit(0);
	}
	fseek(fp, 0, SEEK_END);
	unsigned long fileSize = ftell(fp);
	unsigned char * fileBuffer = (char*)malloc(sizeof(char)*(fileSize+1));
	fseek(fp, 0, SEEK_SET);
	fread(fileBuffer, fileSize, sizeof(char), fp);	// 读取已经加密的字符串
	fileBuffer[fileSize] = '\0';	// 加上字符串结束符号
	fclose(fp);
	printf("图片文件大小: %ul\n", fileSize);


//	char argNum = 2;
//	char* argList[] = {(char*)(argNum), "--stdin"};
	// jpegoptim -d ./compressed -m50 -p terrain.jpg
	int quality = 2;
	printf("压缩函数开始\n");
	clock_t start = clock();
	char* compressedJPEG = jpegoptimMain(fileBuffer, &fileSize, quality);
	printf("压缩所用时间为：%f 秒\n", (double)(clock()-start) / CLOCKS_PER_SEC);
//	char* compressedJPEG = jpegoptimMain(fileBuffer, fileSize, quality);
	printf("压缩后文件大小: %ul\n", fileSize);


	// 写入压缩后的图片
	FILE *fpCompressedPic = fopen("./tools/compressed/compressed.jpg","w");
	fwrite(compressedJPEG, fileSize, 1, fpCompressedPic);
	fclose(fpCompressedPic);


	printf("写入压缩文件完成.\n");
	return 0;
}

