/*
 * test.c
 *
 *  Created on: 2016年12月28日
 *      Author: ubuntu
 */

#include "commonTools.h"

int main() {
	// 测试压缩
	char* filePath = "/home/ubuntu/install/eclipse/workspace/DFS/data/terrain.jpg";
	compress(filePath, 1);

	// 测试AES加密


	return 0;
}
