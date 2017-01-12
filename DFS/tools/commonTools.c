/*
 * compressPicture.c
 *
 *  Created on: 2016年12月27日
 *      Author: ubuntu
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>


/*
 * filePath: 需要压缩图片的文件路径
 * quality: 压缩后的图片质量（质量数范围0到100。(0是最差质量，99是好差质量)）
 * 可转化为调用jpegoptim.c中的main函数
 */
int compress(char* filePath, int quality) {
//	int qualityInt = (int)(quality*100);
	if (quality < 0 || quality > 100) {
		perror("quality illegal\n");
	}
	char *__restrict compressQuality = (char *)malloc(sizeof(char) * 1);
	compressQuality[0] = '-';	compressQuality[1] = 'm';
	char *__restrict qualityChar = (char*)malloc(sizeof(char) * 3);		// 参数const 不是必须的对于检查warning
	sprintf(qualityChar, "%d", quality);	//	itoa(quality, qualityChar, 10); Linux没有itoa函数
//	strcat(compressQuality, qualityChar);	// 类型限定词restrict, 表明指针是访问一个数据对象的惟一且初始的方式;__THROW __nonnull:不抛出异常和不让参数为空
//	char* joinStr = join3(compressQuality, qualityChar);
	if (quality < 10) {
		compressQuality[2] = qualityChar[0];
		compressQuality[3] = '\0';
	} else if (quality == 100) {
		compressQuality[2] = qualityChar[0];
		compressQuality[3] = qualityChar[1];
		compressQuality[4] = qualityChar[2];
		compressQuality[5] = '\0';
	} else {
		compressQuality[2] = qualityChar[0];
		compressQuality[3] = qualityChar[1];
		compressQuality[4] = '\0';
	}

	char cwd[80];
	getcwd(cwd, sizeof(cwd));
	int pid = 1;
	if ((pid = fork()) < 0) {	// execl是覆盖进程的代码段，所以如果你原来的程序还需要正常退出的话，就要fork一个子进程
		perror("fork()\n");
	} else if (pid == 0) {
		char exeFilePath[80];  strcpy(exeFilePath, cwd);  strcat(exeFilePath, "/third/jpegoptim/bin/jpegoptim");
		char compressedFilePath[80];  strcpy(compressedFilePath, cwd);  strcat(compressedFilePath, "/data/compressed");
//		execl(exeFilePath, "jpegoptim ", "-d", compressedFilePath, compressQuality, "-p", filePath, (char *)0);	// 压缩放置到另一个路径
		execl(exeFilePath, "jpegoptim", compressQuality, "-p", filePath, (char *)0);	// 原地压缩（覆盖原来的文件）jpegoptim -m1 -p /home/ubuntu/install/eclipse/workspace/DFS/data/terrain.jpg
		exit(EXIT_SUCCESS);		// There will be more portability using variable define by system. #include <stdlib.h>
	}
	wait(NULL);	//等子进程，防止它成为僵尸

	return 1;
}


//int main() {
//	char* filePath = "/home/ubuntu/install/eclipse/workspace/DFS/data/terrain.jpg";
//	compress(filePath, 0);	// 压缩后的图片保存到/data/compressed目录下
//	return 0;
//}






//#include <opencv2/opencv.hpp>
//#include<iostream>
//
//#include <stdio.h>
//using namespace cv;
//
//int main(int argc, char **argv)
//{
//    int p[3];
//    IplImage *img = cvLoadImage("/home/ubuntu/install/eclipse/workspace/DFS/data/terrain.jpg");
//
//    p[0] = CV_IMWRITE_JPEG_QUALITY;
//    p[1] = 10;
//    p[2] = 0;
//    cvSaveImage("out1.jpg", img, p);
//
//    p[0] = CV_IMWRITE_JPEG_QUALITY;
//    p[1] = 100;
//    p[2] = 0;
//    cvSaveImage("out2.jpg", img, p);
//
//    exit(0);
//
//}


/*
#define CV_IMWRITE_JPEG_QUALITY 1				图片质量
  #define CV_IMWRITE_PNG_COMPRESSION 16
  #define CV_IMWRITE_PXM_BINARY 32

  CVAPI(int) cvSaveImage( const char* filename, const CvArr* image, const int* params CV_DEFAULT(0) );

  int p[3];
  p[0] = CV_IMWRITE_JPEG_QUALITY;
  p[1] = desired_quality_value;
  p[2] = 0;
*/
