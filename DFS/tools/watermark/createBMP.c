/*******************************************************
* Name：       creat_bmp.c
* Function:   Creat a bmp picture as 320*240
* Input parameters:	 
* Output parameters:  
* Author: Valerian
* Version :	1.0
* Date :  2012-1-16
*******************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char byte;
typedef unsigned short dbyte;
typedef unsigned long int dword;
typedef unsigned short word;

/*******************************************
*定义bmp文件的头部数据结构
********************************************/
#pragma pack(push,2)  //保持2字节对齐

struct tagBITMAPFILEHEADER {
    //bmp file header
    dbyte bfType;        //文件类型
    dword bfSize;            //文件大小，字节为单位
    word bfReserved1;   //保留，必须为0
    word bfReserved2;   //保留，必须为0
    dword bfOffBits;         //从文件头开始的偏移量

    //bmp info head
    dword  biSize;            //该结构的大小
    dword  biWidth;           //图像的宽度，以像素为单位
    dword  biHeight;          //图像的高度，以像素为单位
    word biPlanes;          //为目标设备说明位面数，其值总是设为1
    word biBitCount;        //说明比特数/像素
    dword biCompression;     //图像数据压缩类型
    dword biSizeImage;       //图像大小，以字节为单位
    dword biXPelsPerMeter;   //水平分辨率，像素/米
    dword biYPelsPerMeter;   //垂直分辨率，同上
    dword biClrUsed;         //位图实际使用的彩色表中的颜色索引数
    dword biClrImportant;    //对图像显示有重要影响的颜色索引的数目
      
    //bmp rgb quad
     //对于16位，24位，32位的位图不需要色彩表
    //unsigned char rgbBlue;    //指定蓝色强度
    //unsigned char rgbGreen;   //指定绿色强度
    //unsigned char rgbRed;     //指定红色强度
    //unsigned char rgbReserved; //保留，设置为0
}BMPFILEHEADER;
#pragma (pop)

struct tagBITMAPFILEHEADER *bmp_p;  //定义bmp文件头结构体指针
FILE *fd;    //定义一个文件类型的指针

/*************************************************************
*初始化bmp文件头部，设置bmp图片
**************************************************************/

void Init_bmp_head(void)
{
    bmp_p = &BMPFILEHEADER;
    bmp_p-> bfType = 0x4D42;    //文件类型
    bmp_p-> bfSize = 0x25836;   //文件大小，字节为单位
    bmp_p-> bfReserved1 = 0x0;   //保留，必须为0
    bmp_p-> bfReserved2 = 0x0;   //保留，必须为0
    bmp_p-> bfOffBits = 0x36;         //从文件头开始的偏移量
 
    //bmp info head
    bmp_p-> biSize = 0x28;            //该结构的大小
    bmp_p-> biWidth = 320;           //图像的宽度，以像素为单位
    bmp_p-> biHeight = 240;          //图像的高度，以像素为单位
    bmp_p-> biPlanes = 0x01;          //为目标设备说明位面数，其值总是设为1
    bmp_p-> biBitCount = 16;        //说明比特数/像素
    bmp_p-> biCompression = 0;     //图像数据压缩类型
    bmp_p-> biSizeImage = 0x25800;//0x09f8;       //图像大小，以字节为单位
    bmp_p-> biXPelsPerMeter = 0x60;//0x60;   //水平分辨率，像素/米
    bmp_p-> biYPelsPerMeter = 0x60;   //垂直分辨率，同上
    bmp_p-> biClrUsed = 0;         //位图实际使用的彩色表中的颜色索引数
    bmp_p-> biClrImportant = 0;    //对图像显示有重要影响的颜色索引的数目
     
}

int main(void)
{
    static char *file_name =NULL;    //保存文件名的指针
    static long file_length = 0x25836;  //文件的大小（整个文件）
    unsigned char *file_p = NULL;       //写入数据指针
    unsigned char *file_p_tmp = NULL;    //写入数据临时指针
    unsigned char *byte_copy_p = NULL;   //文件头部传递指针
    unsigned char byte_copy = 0;        //文件头部数据拷贝变量
    int i = 0;         
    file_name = "test1.bmp";
    Init_bmp_head(); 
    file_p = (unsigned char *)malloc(sizeof(char)*153654);   //申请一段内存
    file_p_tmp = file_p;
    for(i = 0;i < 153654;i++ )
    {
        if(i%2 ==0)
        {
            *file_p_tmp = 0x00;    //图像前8位值
        }
        else  
        { 
            *file_p_tmp = 0xf0;    //图像后8位值
        }
       file_p_tmp++;
    } 
    byte_copy_p = (unsigned char *)bmp_p;
    file_p_tmp = file_p;
    for(i = 0;i < 54;i++)
    {
        *file_p_tmp = *byte_copy_p; 
        file_p_tmp++;
        byte_copy_p++;
    }
   fd = fopen(file_name, "w");
   fwrite(file_p, file_length, 1,fd);
   free(file_p);                        //释放申请的内存（重要）
   fclose(fd); 
   printf("Done success!!!\n");
   getchar();
   return (0);
}
