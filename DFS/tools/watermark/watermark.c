//本文算法实现的C语言代码，Windows XP平台DEV-CPP(4.9.9.2)环境下编译通过//
#include <stdio.h>                       //
#include <stdlib.h>                      //
//#include <conio.h>                      //头文件
#include <string.h>                      //头文件
#include <malloc.h>                     //
#include <math.h>                       //
#define  MAX_ITERA     60           //常量宏定义
#define  MIN_DOUBLE    (1e-30)       //常量宏定义
#define  F_EXTPAD       4            //常量宏定义
#define  D_EXTPAD       2            //常量宏定义

//函数原型预定义区域
void RecoverWaterMark3(char *InVector, char *OutWater, double a);
void InsertWaterMark3(char *InVector, char *OutVector, char *InWater,
		char *OutWater, double a);
void RecoverWaterMark2(char *InVector, char *OutWater, double a);
void InsertWaterMark2(char *InVector, char *OutVector, char *InWater,
		char *OutWater, double a);
void RecoverWaterMark1(char *InVector, char *OutWater, double a);
void InsertWaterMark1(char *InVector, char *OutVector, char *InWater,
		char *OutWater, double a);
void Arnold256(char *in, char *out, unsigned char *q, int N, int k1, int k21,
		int k22, int k23, int k24, int type);
int SVD(double a[], int m, int n, double u[], double v[], double eps);
void Matrices(double a[], double b[], int m, int n, int k, double c[]);
void ppp(double a[], double e[], double s[], double v[], int m, int n);
void sss(double fg[2], double cs[2]);
void YIQ2RGB(char *out, double *Y, double *I, double *Q, int N);
void RGB2YIQ(char *in, char *out, double *Y, double *I, double *Q, int N);
void DWT2D(double **rows, int width, int height, int levels, int inverse);
void IDWT1D(double *x, int N);
void DWT1D(double *x_in, int N);
void CheckPSNR(double *p0, double *p1, int m, int n, double a);
unsigned char Double2Char(double d);
void Array2D1D(double **aa, double *a, int m, int n);
void Array1D2D(double *a, double **aa, int m, int n);
void errorNexit(char *s, char *d);
void MsgBox(char *s, char *a, char *b);
double MAX(double, double, double);

static double *x_alloc = NULL;            //静态内存指针

//////////////////////////////////////主函数////////////////////////////////////////////////////////////////////////////
//载体图片默认为lmk.bmp-----------------必须为640*640*24bit
//水印图片默认为W.bmp-------------------必须为64*64*8bit
//W_a.bmp为水印图片加密后输出-----------大小为64*64*8bit
//lmk_w10.bmp为嵌入水印后的载体图片------大小为640*640*24bit
//lmk_w20.bmp为嵌入水印后的载体图片------大小为640*640*24bit
//---a----------------------------------水印嵌入强度，初始默认为0.05
//临时文件SLL3.dat,SLH3.dat,SHL3.dat,SHH3.dat,W.dat,Uw.dat,Vw.dat，分别保存了
//载体图片LL3子图分解后的奇异值矩阵(double)------用于提取水印使用
//载体图片LH3子图分解后的奇异值矩阵(double)------用于提取水印使用
//载体图片HL3子图分解后的奇异值矩阵(double)------用于提取水印使用
//载体图片HH3子图分解后的奇异值矩阵(double)------用于提取水印使用
//原始水印加密后的数据矩阵(double)---------------用于评价NC
//水印奇异值分解后的左奇异向量(double)-----------用于提取水印时合成水印图像
//水印奇异值分解后的右奇异向量(double)-----------用于提取水印时合成水印图像
//////////////////////////////////////主函数////////////////////////////////////////////////////////////////////////////
int main() {
	chdir("./tools/watermark");
//	printf("current working directory: %s\n", getcwd(NULL, NULL));

	double a = 0.05;			//初始嵌入强度
	int type = 6, t;
	char *s;
	void (*insert)(), (*recover)();   //函数指针

	for (;;) {
		printf("请选择嵌入方式：\n");
		printf(" 1:方式一       \n");
		printf("  2:方式二      \n");
		printf("   3:方式三     \n");
		printf("    4:退出程序  \n\n");
		scanf("%d", &t);
		if (t == 1)                                 //选择嵌入方式1
				{
			insert = InsertWaterMark1;
			recover = RecoverWaterMark1;
			s = "方式一";
			printf("已选中方式一，当前方式将把水印信息全部嵌入到LL3子图中。\n\n");
		} else if (t == 2)                             //选择嵌入方式2
				{
			insert = InsertWaterMark2;
			recover = RecoverWaterMark2;
			s = "方式二";
			printf("已选中方式二，当前方式将把水印信息分开嵌入到LL3,LH3,HL3三个子图中。\n\n");
		} else if (t == 3)                                      //选择嵌入方式3
				{
			insert = InsertWaterMark3;
			recover = RecoverWaterMark3;
			s = "方式三";
			printf("已选中方式三，当前方式将把水印信息重复嵌入到LH3,HL3,HH3三个子图中。\n\n");
		} else if (t == 4)
			break;
		else
			continue;

		while (type) {
			if (type == 1)
				(*insert)("lmk.bmp", "lmk_w0.bmp", "W.bmp", "W_a.bmp", a);
			else if (type == 2)
				(*recover)("lmk_w0.bmp", "W0.bmp", a);
			else if (type == 3) {
				printf("当前阀值为:%f\n改变后变为:%f\n", a, a + 0.05);
				a = a + 0.05;
			} else if (type == 4) {
				printf("当前阀值为:%f\n改变后变为:%f\n", a, a - 0.05);
				a = a - 0.05;
			} else if (type == 5) {
				type = 6;
				break;
			}
			printf("-------%s--------\n\n", s);
			printf("请选择您需要的服务：\n");
			printf("   1:嵌入水印       \n");
			printf("   2:提取水印       \n");
			printf("   3:增大阀值       \n");
			printf("   4:减小阀值       \n");
			printf("   5:  后退         \n");
			printf("-------%s-------\n\n", s);
			scanf("%d", &type);
		}
	}
	system("PAUSE");
	return 0;
}

///////////////////////////////小功能函数////////////////////////////////
void MsgBox(char *s, char *a, char *b) /*信息窗*/
{
	printf("-----------%s----------\n", s);
	printf(" %s %s\n\n", a, b);
}
void errorNexit(char *s, char *d) /*报错窗*/
{
	printf("%s %s\n", s, d);
	system("stty -echo");
//	getch();
	getchar();
	system("stty -echo");
	exit(1);
}
void Array1D2D(double *a, double **aa, int m, int n) /*一维实数矩阵转二维矩阵*/
{
	int i, j;
	for (i = 0; i < m; i++)
		for (j = 0; j < n; j++)
			aa[i][j] = *(a + j + i * n);
}
void Array2D1D(double **aa, double *a, int m, int n) /*二维实数矩阵转一维矩阵*/
{
	int i, j;
	for (i = 0; i < m; i++)
		for (j = 0; j < n; j++)
			*(a + j + i * n) = aa[i][j];
}
unsigned char Double2Char(double d) /*double转unsigned char*/
{
	unsigned char a;
	d = d + 0.5;
	if (d > 255.0)
		a = 255;
	else if (d < 0.0)
		a = 0;
	else
		a = (unsigned char) (d);
	return a;
}
double MAX(double A, double B, double C) /*3个浮点数取最大值返回*/
{
	double M;
	M = (A > B) * A + (A < B) * B + (A == B) * A;
	M = (C > M) * C + (C < M) * M + (C == M) * C;
	return M;
}

void CheckPSNR(double *p0, double *p1, int m, int n, double a)/*检查PSNR,SNR,NC*/
{
	int i, j;
	double snr, psnr, nc, dsum = 0, sum0 = 0, sum1 = 0, sum2 = 0, t;
	for (i = 0; i < m; i++)
		for (j = 0; j < n; j++) {
			dsum += (*(p1 + j + i * n) - *(p0 + j + i * n))
					* (*(p1 + j + i * n) - *(p0 + j + i * n));
			sum0 += (*(p0 + j + i * n)) * (*(p0 + j + i * n));
			sum1 += (*(p1 + j + i * n)) * (*(p1 + j + i * n));
			sum2 += (*(p0 + j + i * n)) * (*(p1 + j + i * n));
		}
	snr = 10 * log10(sum0 / dsum);                                //计算信噪比
	psnr = 10 * log10(m * n * 255 * 255 / dsum);                       //计算峰值信噪比（PSNR（Peak Signalto Noise Ratio），峰值信噪比）
	nc = sum2 / (sqrt(sum0) * sqrt(sum1));                           //计算归一化相关系数
	printf("水印嵌入强度为:%f\n  SNR=%f dB\n PSNR=%f dB\n   NC=%f   \n\n", a, snr,
			psnr, nc);
}

/////////////////////////////////小波变换////////////////////////////////////////////////////////////////////////////
//DWT1D（double *x_in, int N）   一维小波正变换 ，X_IN为一维实矩阵，N为长度
//IDWT1D（double *x_in, int N）   一维小波反变换 ，X_IN为一维实矩阵，N为长度
//DWT2D(double **rows, int width, int height, int levels,int inverse) 
//二维小波变换，width和height为矩阵宽与高 ，
//levels为分解层次，inverse为操作类型：0时为正变换，1时为反变换
//采用 97小波基 双正交滤波器组
//////////////////////////////////小波变换/////////////////////////////////////////////////////////////////////////
void DWT1D(double *x_in, int N)/*一维小波正变换*/
{
	int i, n, half;
	double *x, *r, *d;                       //临时内存指针
	x = x_alloc + F_EXTPAD;               //开辟变换数据内存
	memcpy(x, x_in, sizeof(double) * N);        //将变换数据copy到x中
	for (i = 1; i <= F_EXTPAD; i++) /*镜象对称边界延拓，以x[0]和x[N-1]为中心对称点*/
	{
		x[-i] = x[i];
		x[(N - 1) + i] = x[(N - 1) - i];
	}
	half = N >> 1;                          //取数据长度一般
	r = x_in;
	d = x_in + half;                 //r存放平滑系数，d存放细节系数
	for (n = half; n--;)                        //变换开始，滤波并抽取数据
			{
		*r++ = 0.0378285 * (x[4] + x[-4]) - 0.0238495 * (x[3] + x[-3])
				- 0.110624 * (x[2] + x[-2]) + 0.377403 * (x[1] + x[-1])
				+ 0.852699 * x[0];
		x++;
		*d++ = -0.064539 * (x[3] + x[-3]) + 0.040689 * (x[2] + x[-2])
				+ 0.418092 * (x[1] + x[-1]) - 0.788486 * x[0];
		x++;
	}
}
void IDWT1D(double *x, int N)/*一维小波反变换*/
{
	int i, n, half;
	double *r, *d;
	half = N / 2;                                             //获取数据长度中点
	r = x_alloc + D_EXTPAD;                                //开辟空间存放平滑系数
	d = x_alloc + D_EXTPAD + half + D_EXTPAD + D_EXTPAD;    //开辟空间存放细节系数
	memcpy(r, x, half * sizeof(double));                           //copy平滑系数
	memcpy(d, x + half, half * sizeof(double));                      //copy细节系数
	for (i = 1; i <= D_EXTPAD; i++)                               //镜像延拓
			{
		r[-i] = r[i];
		r[(half - 1) + i] = r[half - i];
		d[-i] = d[i - 1];
		d[(half - 1) + i] = d[(half - 1) - i];
	}
	for (n = half; n--;)                                       //逆变换开始，滤波并上采样
			{
		*x++ = 0.788486 * r[0] - 0.0406894 * (r[1] + r[-1])
				- 0.023849 * (d[1] + d[-2]) + 0.377403 * (d[0] + d[-1]);
		*x++ = 0.418092 * (r[1] + r[0]) - 0.0645389 * (r[2] + r[-1])
				- 0.037829 * (d[2] + d[-2]) + 0.110624 * (d[1] + d[-1])
				- 0.852699 * d[0];
		d++;
		r++;
	}
}
void DWT2D(double **rows, int width, int height, int levels, int inverse)/*二维小波变换*/
{
	int x, y, w, h, l;
	double *buffer;
	if (width % (1 << levels) || height % (1 << levels)) /*检查矩阵长宽是否符合要求*/
		errorNexit("矩阵的快读和高度", "必须是2^level的倍数");
	/*为计算开辟临时内存空间*/
	if ((x_alloc = malloc(
			(width + height + F_EXTPAD + F_EXTPAD) * sizeof(double))) == NULL)
		errorNexit("分配内存", "失败");
	if ((buffer = malloc(
			(width + height + F_EXTPAD + F_EXTPAD) * sizeof(double))) == NULL)
		errorNexit("分配内存", "失败");

	/*小波变换计算*/
	if (!inverse)					//选择2维正变换
	{
		for (l = 0; l < levels; l++)            //连续分解levels层
				{
			w = width >> l;
			h = height >> l;
			for (y = 0; y < h; y++) /* 行变换*/
				DWT1D(rows[y], w);
			for (x = 0; x < w; x++) /*列变换*/
			{
				for (y = 0; y < h; y++)          //获取列数据
					buffer[y] = rows[y][x];
				DWT1D(buffer, h);
				for (y = 0; y < h; y++)
					rows[y][x] = buffer[y];
			}
		}
	} else					//选择二维反变换
	{
		for (l = levels - 1; l >= 0; l--)           //连续分解levels层
				{
			w = width >> l;
			h = height >> l;
			for (x = 0; x < w; x++) /*列反变换*/
			{
				for (y = 0; y < h; y++)          //获取列数据
					buffer[y] = rows[y][x];
				IDWT1D(buffer, h);
				for (y = 0; y < h; y++)
					rows[y][x] = buffer[y];
			}
			for (y = 0; y < h; y++) /*行反变换*/
				IDWT1D(rows[y], w);
		}
	}
	/*释放内存空间*/
	free(x_alloc);
	free(buffer);
}

/////////////////////////////////////RGB转YIQ////////////////////////////////////
//in        RGB载体图片文件名
//out       载体图片灰度化输出文件
//Y,I,Q     RGB空间转YIQ空间后保存Y,I,Q三个分解矩阵（Y表示亮度，I、Q是两个彩色分量）;
//				彩色电视一般用YUV和YIQ模型来表示彩色图像，因为眼对彩色图像细节的分辨本领比黑白图像低，故对色差信号UV，可以采用“大面积着色原理”；
//				人眼分辨红、黄之间颜色变化能力最强，而分辨蓝紫之间变化能力最弱
//N         图片尺寸
/////////////////////////////////////RGB转YIQ////////////////////////////////////
void RGB2YIQ(char *in, char *out, double *Y, double *I, double *Q, int N) {
	FILE *f;
	unsigned char *bmphead, *DIB, *H;
	int i, s;
	bmphead = malloc(54 + 256 * 4 * sizeof(unsigned char));       //bmp文件头(大小54+256*4 B)
	DIB = malloc(3 * N * N * sizeof(unsigned char));             //DIB数据（Device-Independent Bitmaps;DDB：Device-independent bitmaps）
	H = malloc(N * N * sizeof(unsigned char));
	if ((f = fopen("bmp256.HD", "rb")) == NULL)              //读取BMP文件头信息
		errorNexit("打开BMP文件头信息文件失败:", "bmp256.HD");
	fread(bmphead, 54 + 256 * 4 * sizeof(unsigned char), 1, f);	// 读取1项，每项54+254*4大小,bmphead接收数据的内存地址
	fclose(f);
	*(bmphead + 18) = N % 256;                             //把图片尺寸写入bmp文件头
	*(bmphead + 19) = N / 256;
	*(bmphead + 22) = N % 256;
	*(bmphead + 23) = N / 256;
	if ((f = fopen(in, "rb")) == NULL)                        //打开彩色图片读取DIB数据（s设备无关位图）图像在硬盘上被称为BMP图，在内存中则被叫做DDB或DIB
		errorNexit("打开文件失败:", in);
	fseek(f, 54, 0);
	fread(DIB, 3 * N * N * sizeof(unsigned char), 1, f);
	fclose(f);
	for (i = 0; i < N * N; i++)                                  //RGB-YIQ色彩空间转换
			{
		s = 3 * i;
		*(H + i) = (unsigned char) ((*(DIB + s + 2) * 0.299
				+ *(DIB + s + 1) * 0.587 + *(DIB + s) * 0.114) + 0.5);
		*(Y + i) = (*(DIB + s + 2) * 0.299 + *(DIB + s + 1) * 0.587
				+ *(DIB + s) * 0.114);
		*(I + i) = (*(DIB + s + 2) * 0.596 - *(DIB + s + 1) * 0.275
				- *(DIB + s) * 0.321);
		*(Q + i) = (*(DIB + s + 2) * 0.212 - *(DIB + s + 1) * 0.523
				+ *(DIB + s) * 0.311);
	}
	if (strlen(out))                                       //保存转换后图片
			{
		if ((f = fopen(out, "wb")) == NULL)
			errorNexit("打开文件失败:", out);
		fwrite(bmphead, 54 + 256 * 4, 1, f);
		fwrite(H, N * N * sizeof(unsigned char), 1, f);
		fclose(f);
	}
	free(bmphead);
	free(DIB);
	free(H);
}

////////////////////////////////YIQ转RGB//////////////////////////////////////////
// out        输出RGB图片文件名
// Y,I,Q      YIQ空间的三个分解矩阵
// N          图片尺寸大小
///////////////////////////////YIQ转RGB///////////////////////////////////////////
void YIQ2RGB(char *out, double *Y, double *I, double *Q, int N) {
	FILE *f;
	unsigned char *bmphead, *R, *G, *B;
	double **SUM;
	int i, s;
	bmphead = malloc(54 * sizeof(unsigned char));
	R = malloc(N * N * sizeof(unsigned char));                 //分配空间保存R分量
	G = malloc(N * N * sizeof(unsigned char));                 //分配空间保存G分量
	B = malloc(N * N * sizeof(unsigned char));                 //分配空间保存B分量
	if ((f = fopen("bmp24bit.HD", "rb")) == NULL)            //读取24bitBMP文件头
		errorNexit("打开24bitBMP文件头失败:", "bmp24bit.HD");
	fread(bmphead, 54 * sizeof(unsigned char), 1, f);
	fclose(f);
	*(bmphead + 18) = N % 256;                            //写入图片尺寸到bmp文件头
	*(bmphead + 19) = N / 256;
	*(bmphead + 22) = N % 256;
	*(bmphead + 23) = N / 256;
	if ((f = fopen(out, "wb")) == NULL)
		errorNexit("打开文件失败:", out);
	fwrite(bmphead, 54, 1, f);
	free(bmphead);
	for (i = 0; i < N * N; i++)                                 // YIQ-RGB色彩空间转换
			{
		*(R + i) = Double2Char(
				(*(Y + i) + *(I + i) * 0.956 + *(Q + i) * 0.620));
		*(G + i) = Double2Char(
				(*(Y + i) - *(I + i) * 0.272 - *(Q + i) * 0.647));
		*(B + i) = Double2Char(
				(*(Y + i) - *(I + i) * 1.108 + *(Q + i) * 1.705));
		fwrite(B + i, 1, 1, f);
		fwrite(G + i, 1, 1, f);
		fwrite(R + i, 1, 1, f);
	}
	fclose(f);
	free(R);
	free(G);
	free(B);
}
///////////////////////////////实矩阵奇异值分解///////////////////////////////////////////////
//a        m*n的实矩阵，返回时其对角线给出奇异值（递减），其余元素为0
//m,n      矩阵A的行数和列数
//u        m*m的矩阵，存放左奇异向量
//v        n*n的矩阵，存放右奇异向量
//eps     双精度实型变量，给定精度要求
//返回值：如果返回标志小于0，则说明出现了迭代60次还未求得某个奇异值的情况
//此时矩阵A的分解式为UAV，如果返回标志大于0，则说明程序正常运行
////////////////////////////////实矩阵奇异值分解/////////////////////////////////////////////
int SVD(double a[], int m, int n, double u[], double v[], double eps) {
	int i, j, k, l, it, ll, kk, ix, iy, mm, nn, iz, ml, ks, ka;
	double d, dd, t, sm, sml, eml, sk, ek, b, c, shh, fg[2], cs[2];
	double *s, *e, *w;
	ka = m + 1;
	if (n > m)
		ka = n + 1;
	s = (double*) malloc(ka * sizeof(double));
	e = (double*) malloc(ka * sizeof(double));
	w = (double*) malloc(ka * sizeof(double));
	for (i = 1; i <= m; i++) {
		ix = (i - 1) * m + i - 1;
		u[ix] = 0;
	}
	for (i = 1; i <= n; i++) {
		iy = (i - 1) * n + i - 1;
		v[iy] = 0;
	}
	it = MAX_ITERA;
	k = n;
	if (m - 1 < n)
		k = m - 1;
	l = m;
	if (n - 2 < m)
		l = n - 2;
	if (l < 0)
		l = 0;
	ll = k;
	if (l > k)
		ll = l;
	if (ll >= 1) {
		for (kk = 1; kk <= ll; kk++) {
			if (kk <= k) {
				d = 0.0;
				for (i = kk; i <= m; i++) {
					ix = (i - 1) * n + kk - 1;
					d = d + a[ix] * a[ix];
				}
				s[kk - 1] = sqrt(d);
				if (fabs(s[kk - 1]) > MIN_DOUBLE) {
					ix = (kk - 1) * n + kk - 1;
					if (fabs(a[ix]) > MIN_DOUBLE) {
						s[kk - 1] = fabs(s[kk - 1]);
						if (a[ix] < 0.0)
							s[kk - 1] = -s[kk - 1];
					}
					for (i = kk; i <= m; i++) {
						iy = (i - 1) * n + kk - 1;
						a[iy] = a[iy] / s[kk - 1];
					}
					a[ix] = 1.0 + a[ix];
				}
				s[kk - 1] = -s[kk - 1];
			}
			if (n >= kk + 1) {
				for (j = kk + 1; j <= n; j++) {
					if ((kk <= k) && (fabs(s[kk - 1]) > MIN_DOUBLE)) {
						d = 0.0;
						for (i = kk; i <= m; i++) {
							ix = (i - 1) * n + kk - 1;
							iy = (i - 1) * n + j - 1;
							d = d + a[ix] * a[iy];
						}
						d = -d / a[(kk - 1) * n + kk - 1];
						for (i = kk; i <= m; i++) {
							ix = (i - 1) * n + j - 1;
							iy = (i - 1) * n + kk - 1;
							a[ix] = a[ix] + d * a[iy];
						}
					}
					e[j - 1] = a[(kk - 1) * n + j - 1];
				}
			}
			if (kk <= k) {
				for (i = kk; i <= m; i++) {
					ix = (i - 1) * m + kk - 1;
					iy = (i - 1) * n + kk - 1;
					u[ix] = a[iy];
				}
			}
			if (kk <= l) {
				d = 0.0;
				for (i = kk + 1; i <= n; i++)
					d = d + e[i - 1] * e[i - 1];
				e[kk - 1] = sqrt(d);
				if (fabs(e[kk - 1]) > MIN_DOUBLE) {
					if (fabs(e[kk]) > MIN_DOUBLE) {
						e[kk - 1] = fabs(e[kk - 1]);
						if (e[kk] < 0.0)
							e[kk - 1] = -e[kk - 1];
					}
					for (i = kk + 1; i <= n; i++)
						e[i - 1] = e[i - 1] / e[kk - 1];
					e[kk] = 1.0 + e[kk];
				}
				e[kk - 1] = -e[kk - 1];
				if ((kk + 1 <= m) && (fabs(e[kk - 1]) > MIN_DOUBLE)) {
					for (i = kk + 1; i <= m; i++)
						w[i - 1] = 0.0;
					for (j = kk + 1; j <= n; j++)
						for (i = kk + 1; i <= m; i++)
							w[i - 1] = w[i - 1]
									+ e[j - 1] * a[(i - 1) * n + j - 1];
					for (j = kk + 1; j <= n; j++)
						for (i = kk + 1; i <= m; i++) {
							ix = (i - 1) * n + j - 1;
							a[ix] = a[ix] - w[i - 1] * e[j - 1] / e[kk];
						}
				}
				for (i = kk + 1; i <= n; i++)
					v[(i - 1) * n + kk - 1] = e[i - 1];
			}
		}
	}
	mm = n;
	if (m + 1 < n)
		mm = m + 1;
	if (k < n)
		s[k] = a[k * n + k];
	if (m < mm)
		s[mm - 1] = 0.0;
	if (l + 1 < mm)
		e[l] = a[l * n + mm - 1];
	e[mm - 1] = 0.0;
	nn = m;
	if (m > n)
		nn = n;
	if (nn >= k + 1) {
		for (j = k + 1; j <= nn; j++) {
			for (i = 1; i <= m; i++)
				u[(i - 1) * m + j - 1] = 0.0;
			u[(j - 1) * m + j - 1] = 1.0;
		}
	}
	if (k >= 1) {
		for (ll = 1; ll <= k; ll++) {
			kk = k - ll + 1;
			iz = (kk - 1) * m + kk - 1;
			if (fabs(s[kk - 1]) > MIN_DOUBLE) {
				if (nn >= kk + 1)
					for (j = kk + 1; j <= nn; j++) {
						d = 0.0;
						for (i = kk; i <= m; i++) {
							ix = (i - 1) * m + kk - 1;
							iy = (i - 1) * m + j - 1;
							d = d + u[ix] * u[iy] / u[iz];
						}
						d = -d;
						for (i = kk; i <= m; i++) {
							ix = (i - 1) * m + j - 1;
							iy = (i - 1) * m + kk - 1;
							u[ix] = u[ix] + d * u[iy];
						}
					}
				for (i = kk; i <= m; i++) {
					ix = (i - 1) * m + kk - 1;
					u[ix] = -u[ix];
				}
				u[iz] = 1.0 + u[iz];
				if (kk - 1 >= 1)
					for (i = 1; i <= kk - 1; i++)
						u[(i - 1) * m + kk - 1] = 0.0;
			} else {
				for (i = 1; i <= m; i++)
					u[(i - 1) * m + kk - 1] = 0.0;
				u[(kk - 1) * m + kk - 1] = 1.0;
			}
		}
	}
	for (ll = 1; ll <= n; ll++) {
		kk = n - ll + 1;
		iz = kk * n + kk - 1;
		if ((kk <= l) && (fabs(e[kk - 1]) > MIN_DOUBLE)) {
			for (j = kk + 1; j <= n; j++) {
				d = 0.0;
				for (i = kk + 1; i <= n; i++) {
					ix = (i - 1) * n + kk - 1;
					iy = (i - 1) * n + j - 1;
					d = d + v[ix] * v[iy] / v[iz];
				}
				d = -d;
				for (i = kk + 1; i <= n; i++) {
					ix = (i - 1) * n + j - 1;
					iy = (i - 1) * n + kk - 1;
					v[ix] = v[ix] + d * v[iy];
				}
			}
		}
		for (i = 1; i <= n; i++)
			v[(i - 1) * n + kk - 1] = 0.0;
		v[iz - n] = 1.0;
	}
	for (i = 1; i <= m; i++)
		for (j = 1; j <= n; j++)
			a[(i - 1) * n + j - 1] = 0.0;
	ml = mm;
	it = MAX_ITERA;
	while (1 == 1) {
		if (mm == 0) {
			ppp(a, e, s, v, m, n);
			free(s);
			free(e);
			free(w);
			return l;
		}
		if (it == 0) {
			ppp(a, e, s, v, m, n);
			free(s);
			free(e);
			free(w);
			return -1;
		}
		kk = mm - 1;
		while ((kk != 0) && (fabs(e[kk - 1]) > MIN_DOUBLE)) {
			d = fabs(s[kk - 1]) + fabs(s[kk]);
			dd = fabs(e[kk - 1]);
			if (dd > eps * d)
				kk = kk - 1;
			else
				e[kk - 1] = 0.0;
		}
		if (kk == mm - 1) {
			kk = kk + 1;
			if (s[kk - 1] < 0.0) {
				s[kk - 1] = -s[kk - 1];
				for (i = 1; i <= n; i++) {
					ix = (i - 1) * n + kk - 1;
					v[ix] = -v[ix];
				}
			}
			while ((kk != ml) && (s[kk - 1] < s[kk])) {
				d = s[kk - 1];
				s[kk - 1] = s[kk];
				s[kk] = d;
				if (kk < n)
					for (i = 1; i <= n; i++) {
						ix = (i - 1) * n + kk - 1;
						iy = (i - 1) * n + kk;
						d = v[ix];
						v[ix] = v[iy];
						v[iy] = d;
					}
				if (kk < m)
					for (i = 1; i <= m; i++) {
						ix = (i - 1) * m + kk - 1;
						iy = (i - 1) * m + kk;
						d = u[ix];
						u[ix] = u[iy];
						u[iy] = d;
					}
				kk = kk + 1;
			}
			it = MAX_ITERA;
			mm = mm - 1;
		} else {
			ks = mm;
			while ((ks > kk) && (fabs(s[ks - 1]) > MIN_DOUBLE)) {
				d = 0.0;
				if (ks != mm)
					d = d + fabs(e[ks - 1]);
				if (ks != kk + 1)
					d = d + fabs(e[ks - 2]);
				dd = fabs(s[ks - 1]);
				if (dd > eps * d)
					ks = ks - 1;
				else
					s[ks - 1] = 0.0;
			}
			if (ks == kk) {
				kk = kk + 1;
				d = fabs(s[mm - 1]);
				t = fabs(s[mm - 2]);
				if (t > d)
					d = t;
				t = fabs(e[mm - 2]);
				if (t > d)
					d = t;
				t = fabs(s[kk - 1]);
				if (t > d)
					d = t;
				t = fabs(e[kk - 1]);
				if (t > d)
					d = t;
				sm = s[mm - 1] / d;
				sml = s[mm - 2] / d;
				eml = e[mm - 2] / d;
				sk = s[kk - 1] / d;
				ek = e[kk - 1] / d;
				b = ((sml + sm) * (sml - sm) + eml * eml) / 2.0;
				c = sm * eml;
				c = c * c;
				shh = 0.0;
				if ((fabs(b) > MIN_DOUBLE) || (fabs(c) > MIN_DOUBLE)) {
					shh = sqrt(b * b + c);
					if (b < 0.0)
						shh = -shh;
					shh = c / (b + shh);
				}
				fg[0] = (sk + sm) * (sk - sm) - shh;
				fg[1] = sk * ek;
				for (i = kk; i <= mm - 1; i++) {
					sss(fg, cs);
					if (i != kk)
						e[i - 2] = fg[0];
					fg[0] = cs[0] * s[i - 1] + cs[1] * e[i - 1];
					e[i - 1] = cs[0] * e[i - 1] - cs[1] * s[i - 1];
					fg[1] = cs[1] * s[i];
					s[i] = cs[0] * s[i];
					if ((fabs(cs[0] - 1.0) > MIN_DOUBLE)
							|| (fabs(cs[1]) > MIN_DOUBLE))
						for (j = 1; j <= n; j++) {
							ix = (j - 1) * n + i - 1;
							iy = (j - 1) * n + i;
							d = cs[0] * v[ix] + cs[1] * v[iy];
							v[iy] = -cs[1] * v[ix] + cs[0] * v[iy];
							v[ix] = d;
						}
					sss(fg, cs);
					s[i - 1] = fg[0];
					fg[0] = cs[0] * e[i - 1] + cs[1] * s[i];
					s[i] = -cs[1] * e[i - 1] + cs[0] * s[i];
					fg[1] = cs[1] * e[i];
					e[i] = cs[0] * e[i];
					if (i < m)
						if ((fabs(cs[0] - 1.0) > MIN_DOUBLE)
								|| (fabs(cs[1]) > MIN_DOUBLE))
							for (j = 1; j <= m; j++) {
								ix = (j - 1) * m + i - 1;
								iy = (j - 1) * m + i;
								d = cs[0] * u[ix] + cs[1] * u[iy];
								u[iy] = -cs[1] * u[ix] + cs[0] * u[iy];
								u[ix] = d;
							}
				}
				e[mm - 2] = fg[0];
				it = it - 1;
			} else {
				if (ks == mm) {
					kk = kk + 1;
					fg[1] = e[mm - 2];
					e[mm - 2] = 0.0;
					for (ll = kk; ll <= mm - 1; ll++) {
						i = mm + kk - ll - 1;
						fg[0] = s[i - 1];
						sss(fg, cs);
						s[i - 1] = fg[0];
						if (i != kk) {
							fg[1] = -cs[1] * e[i - 2];
							e[i - 2] = cs[0] * e[i - 2];
						}
						if ((fabs(cs[0] - 1.0) > MIN_DOUBLE)
								|| (fabs(cs[1]) > MIN_DOUBLE))
							for (j = 1; j <= n; j++) {
								ix = (j - 1) * n + i - 1;
								iy = (j - 1) * n + mm - 1;
								d = cs[0] * v[ix] + cs[1] * v[iy];
								v[iy] = -cs[1] * v[ix] + cs[0] * v[iy];
								v[ix] = d;
							}
					}
				} else {
					kk = ks + 1;
					fg[1] = e[kk - 2];
					e[kk - 2] = 0.0;
					for (i = kk; i <= mm; i++) {
						fg[0] = s[i - 1];
						sss(fg, cs);
						s[i - 1] = fg[0];
						fg[1] = -cs[1] * e[i - 1];
						e[i - 1] = cs[0] * e[i - 1];
						if ((fabs(cs[0] - 1.0) > MIN_DOUBLE)
								|| (fabs(cs[1]) > MIN_DOUBLE))
							for (j = 1; j <= m; j++) {
								ix = (j - 1) * m + i - 1;
								iy = (j - 1) * m + kk - 2;
								d = cs[0] * u[ix] + cs[1] * u[iy];
								u[iy] = -cs[1] * u[ix] + cs[0] * u[iy];
								u[ix] = d;
							}
					}
				}
			}
		}
	}
	free(s);
	free(e);
	free(w);
	return l;
}
void ppp(double a[], double e[], double s[], double v[], int m, int n) {
	int i, j, p, q;
	double d;
	if (m >= n)
		i = n;
	else
		i = m;
	for (j = 1; j <= i - 1; j++) {
		a[(j - 1) * n + j - 1] = s[j - 1];
		a[(j - 1) * n + j] = e[j - 1];
	}
	a[(i - 1) * n + i - 1] = s[i - 1];
	if (m < n)
		a[(i - 1) * n + i] = e[i - 1];
	for (i = 1; i <= n - 1; i++)
		for (j = i + 1; j <= n; j++) {
			p = (i - 1) * n + j - 1;
			q = (j - 1) * n + i - 1;
			d = v[p];
			v[p] = v[q];
			v[q] = d;
		}
	return;
}
void sss(double fg[2], double cs[2]) {
	double r, d;
	if ((fabs(fg[0]) + fabs(fg[1])) < MIN_DOUBLE) {
		cs[0] = 1.0;
		cs[1] = 0.0;
		d = 0.0;
	} else {
		d = sqrt(fg[0] * fg[0] + fg[1] * fg[1]);
		if (fabs(fg[0]) > fabs(fg[1])) {
			d = fabs(d);
			if (fg[0] < 0.0)
				d = -d;
		}
		if (fabs(fg[1]) >= fabs(fg[0])) {
			d = fabs(d);
			if (fg[1] < 0.0)
				d = -d;
		}
		cs[0] = fg[0] / d;
		cs[1] = fg[1] / d;
	}
	r = 1.0;
	if (fabs(fg[0]) > fabs(fg[1]))
		r = cs[1];
	else if (fabs(cs[0]) > MIN_DOUBLE)
		r = 1.0 / cs[0];
	fg[0] = d;
	fg[1] = r;
	return;
}

/////////////////////////////////////////////////*两个实数矩阵相乘算法*//////////////////////////////////
void Matrices(double a[], double b[], int m, int n, int k, double c[]) {
	int i, j, l, u;
	for (i = 0; i <= m - 1; i++)
		for (j = 0; j <= k - 1; j++) {
			u = i * k + j;
			c[u] = 0;
			for (l = 0; l <= n - 1; l++)
				c[u] = c[u] + a[i * n + l] * b[l * k + j];
		}
	return;
}
///////////////////////Arnold变换////////////////////////////////////////////////////////////////////////
//in         水印图片名称
//out        加密后水印图片名称
//*q         水印数据矩阵
//N          水印图片尺寸
//type       操作类型，0为加密（正变换），1为解密（逆变换）
//k1，k21，k22，k23，k24   水印加密密匙
//////////////////////Arnold变换///////////////////////////////////////////////////////////////////////////
void Arnold256(char *in, char *out, unsigned char *q, int N, int k1, int k21,
		int k22, int k23, int k24, int type) {
	unsigned char *p, *bmphead;
	int x1, y1, x, y, nn, i;
	FILE *f;
	nn = N / 2;                                          //图像分割后大小
	bmphead = (unsigned char *) malloc((54 + 256 * 4) * sizeof(unsigned char));
	p = (unsigned char *) malloc(N * N * sizeof(unsigned char));   //分配内存，读取图像矩阵
	if (!type)                                          //打开并读取水印图片
	{
		if ((f = fopen(in, "rb")) == NULL)
			errorNexit("打开文件失败:", in);
		fread(bmphead, (54 + 256 * 4) * sizeof(unsigned char), 1, f);
		fread(p, N * N * sizeof(unsigned char), 1, f);
		fclose(f);
	} else                                              //获取水印图片文件头信息
	{
		if ((f = fopen("bmp256.hd", "rb")) == NULL)
			errorNexit("打开文件失败:", "bmp256.hd");
		fread(bmphead, (54 + 256 * 4) * sizeof(unsigned char), 1, f);
		fclose(f);
		*(bmphead + 18) = N % 256;
		*(bmphead + 19) = N / 256;
		*(bmphead + 22) = N % 256;
		*(bmphead + 23) = N / 256;
		memcpy(p, q, N * N);
	}
//----------------------第一层加密-----------------------------//
	if (!type)
		for (i = 0; i < k1; i++)                             //进行k次的Arnold迭代置乱
				{
			for (y = 0; y < N; y++)
				for (x = 0; x < N; x++) {
					x1 = (71 * x + 65 * y) % N;              //坐标计算
					y1 = (83 * x + 76 * y) % N;              //坐标计算
					*(q + x1 + y1 * N) = *(p + x + y * N);        //移动像素值到新坐标
				}
			for (y1 = 0; y1 < N; y1++)
				for (x1 = 0; x1 < N; x1++)
					*(p + x1 + y1 * N) = *(q + x1 + y1 * N);
		}
//-----------------------第一个分割子图加密/解密----------------------------//
	for (i = 0; i < k21; i++)                                //进行k21次的Arnold迭代置乱
			{
		for (y = 0; y < nn; y++)
			for (x = 0; x < nn; x++) {
				if (!type)                        //正变换坐标计算
				{
					x1 = (21 * x + 10 * y) % nn;
					y1 = (23 * x + 11 * y) % nn;
				} else                    //逆变换坐标计算
				{
					x1 = (11 * x - 10 * y) % nn;
					y1 = (21 * y - 23 * x) % nn;
					if (x1 < 0)
						x1 += nn;
					if (y1 < 0)
						y1 += nn;
				}
				*(q + x1 + y1 * N) = *(p + x + y * N);              //交换像素值
			}
		for (y1 = 0; y1 < nn; y1++)
			for (x1 = 0; x1 < nn; x1++)
				*(p + x1 + y1 * N) = *(q + x1 + y1 * N);
	}

//------------------------第二个分割子图加密/解密---------------------------//
	for (i = 0; i < k22; i++)                                //进行k22次的Arnold迭代置乱
			{
		for (y = 0; y < nn; y++)
			for (x = 0; x < nn; x++) {
				if (!type)                          //正变换坐标计算
				{
					x1 = (5 * x + 4 * y) % nn;
					y1 = (6 * x + 5 * y) % nn;
				} else                     //逆变换坐标计算
				{
					x1 = (5 * x - 4 * y) % nn;
					y1 = (5 * y - 6 * x) % nn;
					if (x1 < 0)
						x1 += nn;
					if (y1 < 0)
						y1 += nn;
				}
				*(q + x1 + nn + y1 * N) = *(p + x + nn + y * N);    //交换像素值
			}
		for (y1 = 0; y1 < nn; y1++)
			for (x1 = 0; x1 < nn; x1++)
				*(p + x1 + nn + y1 * N) = *(q + x1 + nn + y1 * N);
	}
//-------------------------第三个分割子图加密/解密--------------------------//
	for (i = 0; i < k23; i++)                                //进行k23次的Arnold迭代置乱
			{
		for (y = 0; y < nn; y++)
			for (x = 0; x < nn; x++) {
				if (!type)                              //正变换坐标计算
				{
					x1 = (x + y) % nn;
					y1 = (x + 2 * y) % nn;
				} else                          //逆变换坐标计算
				{
					x1 = (2 * x - y) % nn;
					y1 = (y - x) % nn;
					if (x1 < 0)
						x1 += nn;
					if (y1 < 0)
						y1 += nn;
				}
				*(q + x1 + (y1 + nn) * N) = *(p + x + (y + nn) * N);     //交换像素值
			}
		for (y1 = 0; y1 < nn; y1++)
			for (x1 = 0; x1 < nn; x1++)
				*(p + x1 + (y1 + nn) * N) = *(q + x1 + (y1 + nn) * N);
	}
//--------------------------第四个分割子图加密/解密-------------------------//
	for (i = 0; i < k24; i++)                                //进行k24次的Arnold迭代置乱
			{
		for (y = 0; y < nn; y++)
			for (x = 0; x < nn; x++) {
				if (type)                           //正变换坐标计算
				{
					x1 = (2 * x - y) % nn;
					y1 = (y - x) % nn;
					if (x1 < 0)
						x1 += nn;
					if (y1 < 0)
						y1 += nn;
				} else                     //逆变换坐标计算
				{
					x1 = (x + y) % nn;
					y1 = (x + 2 * y) % nn;
				}
				*(q + x1 + nn + (y1 + nn) * N) = *(p + x + nn + (y + nn) * N); //交换像素值
			}
		for (y1 = 0; y1 < nn; y1++)
			for (x1 = 0; x1 < nn; x1++)
				*(p + x1 + nn + (y1 + nn) * N) = *(q + x1 + nn + (y1 + nn) * N);
	}
//------------------------第一重解密---------------------------//
	if (type)
		for (i = 0; i < k1; i++)                         //进行k1次的Arnold迭代解密
				{
			for (y = 0; y < N; y++)
				for (x = 0; x < N; x++) {
					x1 = (76 * x - 65 * y) % N;            //坐标还原计算
					y1 = (71 * y - 83 * x) % N;
					if (x1 < 0)
						x1 += N;
					if (y1 < 0)
						y1 += N;
					*(q + x1 + y1 * N) = *(p + x + y * N);     //移动像素值到新坐标
				}
			for (y1 = 0; y1 < N; y1++)
				for (x1 = 0; x1 < N; x1++)
					*(p + x1 + y1 * N) = *(q + x1 + y1 * N);
		}
	memcpy(q, p, N * N);
//---------------------------------------------------//
	if ((f = fopen(out, "wb")) == NULL)
		errorNexit("打开文件失败:", out);
	fwrite(bmphead, (54 + 256 * 4) * sizeof(unsigned char), 1, f);
	fwrite(q, N * N * sizeof(unsigned char), 1, f);
	fclose(f);
	free(bmphead);
	free(p);
}

/////////////////////水印嵌入，方式一////////////////////////////////////////////////////////////////
//InVector        载体图片名称
//OutVector       载体图片灰度化文件名称
//InWater         水印图片名称
//OutWater        水印加密后图片名称
//a               水印嵌入阀值
////////////////////水印嵌入，方式一////////////////////////////////////////////////////////////////
void InsertWaterMark1(char *InVector, char *OutVector, char *InWater,
		char *OutWater, double a) {
	int x, h, s1, s2;
	FILE *f;
	double *Y0, *Y1, **YY, *I, *Q, *LL3, *ULL3, *VLL3, *W, *Uw, *Vw, *T;
	double eps = 0.000001, tmp;
	unsigned char *bmphead, *Wc;
//分配内存空间
	Y0 = (double *) malloc(640 * 640 * sizeof(double));
	Y1 = (double *) malloc(640 * 640 * sizeof(double));
	I = (double *) malloc(640 * 640 * sizeof(double));
	Q = (double *) malloc(640 * 640 * sizeof(double));
	LL3 = (double *) malloc(80 * 80 * sizeof(double));
	ULL3 = (double *) malloc(80 * 80 * sizeof(double));
	VLL3 = (double *) malloc(80 * 80 * sizeof(double));
	T = (double *) malloc(80 * 80 * sizeof(double));
	W = (double *) malloc(64 * 64 * sizeof(double));
	Uw = (double *) malloc(64 * 64 * sizeof(double));
	Vw = (double *) malloc(64 * 64 * sizeof(double));
	Wc = (unsigned char *) malloc(64 * 64 * sizeof(unsigned char));
	YY = malloc(640 * sizeof(double));
	for (x = 0; x < 640; x++)
		YY[x] = malloc(640 * sizeof(double));
//=============================================================
	Arnold256(InWater, OutWater, Wc, 64, 6, 3, 5, 17, 9, 0); /*水印Arnold加密*/
	for (x = 0; x < 64; x++) /*水印矩阵转为double型*/
		for (h = 0; h < 64; h++)
			*(W + h + x * 64) = (double) (*(Wc + h + x * 64));
	if ((f = fopen("W.dat", "wb")) == NULL)                //保存原始水印数据
		errorNexit("打开文件失败:", "W.dat");
	fwrite(W, 64 * 64 * sizeof(double), 1, f);
	fclose(f);
	memset(Uw, 0, 64 * 64);
	memset(Vw, 0, 64 * 64);
	SVD(W, 64, 64, Uw, Vw, eps);                        //对水印矩阵奇异值分解
	if ((f = fopen("Uw.dat", "wb")) == NULL)      //保存水印左奇异向量，待提取水印时使用
		errorNexit("打开文件失败:", "Uw.dat");
	fwrite(Uw, 64 * 64 * sizeof(double), 1, f);
	fclose(f);
	if ((f = fopen("Vw.dat", "wb")) == NULL)      //保存水印右奇异向量，待提取水印时使用
		errorNexit("打开文件失败:", "Vw.dat");
	fwrite(Vw, 64 * 64 * sizeof(double), 1, f);
	fclose(f);
	RGB2YIQ(InVector, "", Y0, I, Q, 640);                 //载体图像RGB转YIQ
	Array1D2D(Y0, YY, 640, 640);                       //载体图像矩阵一维转二维
	DWT2D(YY, 640, 640, 3, 0); /*小波分解*/
	for (x = 0; x < 80; x++) /*提取LL3子图*/
		for (h = 0; h < 80; h++)
			*(LL3 + h + x * 80) = YY[x][h];
	memset(ULL3, 0, 80 * 80);
	memset(VLL3, 0, 80 * 80);
	SVD(LL3, 80, 80, ULL3, VLL3, eps);                  //对LL3奇异值分解
	if ((f = fopen("SLL3.dat", "wb")) == NULL)             //保存SLL3序列，待提取水印时使用
		errorNexit("打开文件失败:", "SLL3.dat");
	fwrite(LL3, 80 * 80 * sizeof(double), 1, f);
	fclose(f);
	for (x = 0; x < 64; x++)                            //嵌入水印系数
			{
		s1 = (x + 1) * 81;
		s2 = x * 65;
		*(LL3 + s1) = (*(LL3 + s1)) + (*(W + s2)) * a;			//嵌入公式
	}
	memset(T, 0, 80 * 80);
	Matrices(ULL3, LL3, 80, 80, 80, T);                          //重构LL3
	Matrices(T, VLL3, 80, 80, 80, LL3);
	for (x = 0; x < 80; x++) /*嵌入LL3子图*/
		for (h = 0; h < 80; h++)
			YY[x][h] = *(LL3 + h + x * 80);
	DWT2D(YY, 640, 640, 3, 1); /*小波重构*/
	Array2D1D(YY, Y1, 640, 640);                         //载体图像二维转一维
	MsgBox("处理中", "水印嵌入完成，", "检查效果如下:");
	CheckPSNR(Y0, Y1, 640, 640, a);                        //检查PSNR等值
	YIQ2RGB(OutVector, Y1, I, Q, 640);                     //载体图像YIQ转RGB
//===========================================================
//释放内存
	free(YY);
	free(Y0);
	free(Y1);
	free(I);
	free(Q);
	free(LL3);
	free(ULL3);
	free(VLL3);
	free(W);
	free(Uw);
	free(Vw);
	free(T);
	free(Wc);
}

////////////////////水印提取，方式一/////////////////////////////////////////////////////////
//InVector        嵌入水印后载体图片名称
//Vector256       嵌入水印后载体图片灰度化文件名称
//OutWater        提取的水印图片名称
//a               水印嵌入阀值
////////////////////水印提取，方式一//////////////////////////////////////////////////////////
void RecoverWaterMark1(char *InVector, char *OutWater, double a) {
	int x, h, s1, s2;
	FILE *f;
	double *Y, **YY, *I, *Q, *LL3, *LL3_0, *ULL3, *VLL3, *W0, *W, *Uw, *Vw, *T;
	double eps = 0.000001;
	unsigned char *bmphead, *Wc;
//动态分配程序内存
	Y = (double *) malloc(640 * 640 * sizeof(double));
	I = (double *) malloc(640 * 640 * sizeof(double));
	Q = (double *) malloc(640 * 640 * sizeof(double));
	LL3 = (double *) malloc(80 * 80 * sizeof(double));
	LL3_0 = (double *) malloc(80 * 80 * sizeof(double));
	ULL3 = (double *) malloc(80 * 80 * sizeof(double));
	VLL3 = (double *) malloc(80 * 80 * sizeof(double));
	T = (double *) malloc(64 * 64 * sizeof(double));
	W0 = (double *) malloc(64 * 64 * sizeof(double));
	W = (double *) malloc(64 * 64 * sizeof(double));
	Uw = (double *) malloc(64 * 64 * sizeof(double));
	Vw = (double *) malloc(64 * 64 * sizeof(double));
	Wc = (unsigned char *) malloc(64 * 64 * sizeof(unsigned char));
	YY = malloc(640 * sizeof(double));
	for (x = 0; x < 640; x++)
		YY[x] = malloc(640 * sizeof(double));
//============================================================
	RGB2YIQ(InVector, "", Y, I, Q, 640);           //载体图像RGB转YIQ
	Array1D2D(Y, YY, 640, 640);                //载体图像矩阵一维转二维
	DWT2D(YY, 640, 640, 3, 0); /*小波分解*/
	for (x = 0; x < 80; x++) /*提取LL3子图*/
		for (h = 0; h < 80; h++)
			*(LL3 + h + x * 80) = YY[x][h];
	memset(ULL3, 0, 80 * 80);
	memset(VLL3, 0, 80 * 80);
	SVD(LL3, 80, 80, ULL3, VLL3, eps);                //对LL3奇异值分解
	if ((f = fopen("SLL3.dat", "rb")) == NULL)            //读取S序列，待提取水印时使用
		errorNexit("打开文件失败:", "SLL3.dat");
	fread(LL3_0, 80 * 80 * sizeof(double), 1, f);
	fclose(f);
	if ((f = fopen("Uw.dat", "rb")) == NULL)        //读取水印左奇异向量，待提取水印时使用
		errorNexit("打开文件失败:", "Uw.dat");
	fread(Uw, 64 * 64 * sizeof(double), 1, f);
	fclose(f);
	if ((f = fopen("Vw.dat", "rb")) == NULL)        //读取水印右奇异向量，待提取水印时使用
		errorNexit("打开文件失败:", "Vw.dat");
	fread(Vw, 64 * 64 * sizeof(double), 1, f);
	fclose(f);
	memset(W, 0, 64 * 64);                       //检测水印信息
	for (x = 0; x < 64; x++) {
		s1 = (x + 1) * 81;
		s2 = x * 65;
		*(W + s2) = ((*(LL3 + s1)) - (*(LL3_0 + s1))) / a;		 //提取公式
	}
	Matrices(Uw, W, 64, 64, 64, T);                    //重构水印
	Matrices(T, Vw, 64, 64, 64, W);
	if ((f = fopen("W.dat", "rb")) == NULL)             //读取原始水印数据
		errorNexit("打开文件失败:", "Uw.dat");
	fread(W0, 64 * 64 * sizeof(double), 1, f);
	fclose(f);
	MsgBox("处理中", "水印提取完成，", "检查效果如下:");
	CheckPSNR(W0, W, 64, 64, a);                    //检查PSNR等值
	for (x = 0; x < 64; x++) /*水印矩阵转为Unchar型*/
		for (h = 0; h < 64; h++)
			*(Wc + h + x * 64) = Double2Char(*(W + h + x * 64));
	Arnold256("", OutWater, Wc, 64, 6, 3, 5, 17, 9, 1);
//释放内存
	free(YY);
	free(Y);
	free(I);
	free(Q);
	free(LL3);
	free(LL3_0);
	free(ULL3);
	free(VLL3);
	free(W);
	free(Uw);
	free(Vw);
	free(T);
	free(Wc);
}
////////////////////////水印嵌入，方式二//////////////////////////////////////////////////////////////
//InVector        载体图片名称
//OutVector       载体图片灰度化文件名称
//InWater         水印图片名称
//OutWater        水印加密后图片名称
//a               水印嵌入阀值
///////////////////////水印嵌入，方式二//////////////////////////////////////////////////////////////
void InsertWaterMark2(char *InVector, char *OutVector, char *InWater,
		char *OutWater, double a) {
	int x, h;
	FILE *f;
	double *Y0, *Y1, **YY, *I, *Q, *LH3, *HL3, *LL3, *ULL3, *UHL3, *ULH3, *VLL3,
			*VHL3, *VLH3, *W, *Uw, *Vw, *T;
	double eps = 0.000001, tmp;
	unsigned char *bmphead, *Wc;
//分配内存空间
	Y0 = (double *) malloc(640 * 640 * sizeof(double));
	Y1 = (double *) malloc(640 * 640 * sizeof(double));
	I = (double *) malloc(640 * 640 * sizeof(double));
	Q = (double *) malloc(640 * 640 * sizeof(double));
	LL3 = (double *) malloc(80 * 80 * sizeof(double));
	HL3 = (double *) malloc(80 * 80 * sizeof(double));
	LH3 = (double *) malloc(80 * 80 * sizeof(double));
	ULL3 = (double *) malloc(80 * 80 * sizeof(double));
	UHL3 = (double *) malloc(80 * 80 * sizeof(double));
	ULH3 = (double *) malloc(80 * 80 * sizeof(double));
	VLL3 = (double *) malloc(80 * 80 * sizeof(double));
	VHL3 = (double *) malloc(80 * 80 * sizeof(double));
	VLH3 = (double *) malloc(80 * 80 * sizeof(double));
	T = (double *) malloc(80 * 80 * sizeof(double));
	W = (double *) malloc(64 * 64 * sizeof(double));
	Uw = (double *) malloc(64 * 64 * sizeof(double));
	Vw = (double *) malloc(64 * 64 * sizeof(double));
	Wc = (unsigned char *) malloc(64 * 64 * sizeof(unsigned char));
	YY = malloc(640 * sizeof(double));
	for (x = 0; x < 640; x++)
		YY[x] = malloc(640 * sizeof(double));
//===============================================================
	Arnold256(InWater, OutWater, Wc, 64, 6, 3, 5, 17, 9, 0); /*水印Arnold加密*/
	for (x = 0; x < 64; x++) /*水印矩阵转为double型*/
		for (h = 0; h < 64; h++)
			*(W + h + x * 64) = (double) (*(Wc + h + x * 64));
	if ((f = fopen("W.dat", "wb")) == NULL)              //保存原始水印数据
		errorNexit("打开文件失败:", "W.dat");
	fwrite(W, 64 * 64 * sizeof(double), 1, f);
	fclose(f);
	memset(Uw, 0, 64 * 64);
	memset(Vw, 0, 64 * 64);
	SVD(W, 64, 64, Uw, Vw, eps);                 //对水印矩阵奇异值分解
	if ((f = fopen("Uw.dat", "wb")) == NULL)       //保存水印左奇异向量，待提取水印时使用
		errorNexit("打开文件失败:", "Uw.dat");
	fwrite(Uw, 64 * 64 * sizeof(double), 1, f);
	fclose(f);
	if ((f = fopen("Vw.dat", "wb")) == NULL)       //保存水印右奇异向量，待提取水印时使用
		errorNexit("打开文件失败:", "Vw.dat");
	fwrite(Vw, 64 * 64 * sizeof(double), 1, f);
	fclose(f);
	RGB2YIQ(InVector, "", Y0, I, Q, 640);          //载体图像RGB转YIQ
	Array1D2D(Y0, YY, 640, 640);               //载体图像矩阵一维转二维
	DWT2D(YY, 640, 640, 3, 0); /*小波分解*/
	for (x = 80; x < 160; x++)
		for (h = 0; h < 80; h++) /*提取LH3子图*/
			*(LH3 + h + (x - 80) * 80) = YY[x][h];
	for (x = 0; x < 80; x++) {
		for (h = 80; h < 160; h++) /*提取HL3子图*/
			*(HL3 + h - 80 + x * 80) = YY[x][h];
		for (h = 0; h < 80; h++) /*提取LL3子图*/
			*(LL3 + h + x * 80) = YY[x][h];
	}
	memset(ULL3, 0, 80 * 80);
	memset(VLL3, 0, 80 * 80);
	memset(UHL3, 0, 80 * 80);
	memset(VHL3, 0, 80 * 80);
	memset(ULH3, 0, 80 * 80);
	memset(VLH3, 0, 80 * 80);
	SVD(LL3, 80, 80, ULL3, VLL3, eps);                    //对LL3奇异值分解
	SVD(HL3, 80, 80, UHL3, VHL3, eps);                   //对HL3奇异值分解
	SVD(LH3, 80, 80, ULH3, VLH3, eps);                   //对LH3奇异值分解
	if ((f = fopen("SLL3.dat", "wb")) == NULL)          //保存SLL3序列，待提取水印时使用
		errorNexit("打开文件失败:", "SLL3.dat");
	fwrite(LL3, 80 * 80 * sizeof(double), 1, f);
	fclose(f);
	if ((f = fopen("SHL3.dat", "wb")) == NULL)          //保存SHL3序列，待提取水印时使用
		errorNexit("打开文件失败:", "SHL3.dat");
	fwrite(HL3, 80 * 80 * sizeof(double), 1, f);
	fclose(f);
	if ((f = fopen("SLH3.dat", "wb")) == NULL)          //保存SLH3序列，待提取水印时使用
		errorNexit("打开文件失败:", "SLH3.dat");
	fwrite(LH3, 80 * 80 * sizeof(double), 1, f);
	fclose(f);
	for (x = 0; x < 64; x++)                                //嵌入水印系数
			{
		int s1 = (x + 1) * 81;
		int s2 = x * 65;
		if (x < 14)
			*(LL3 + s1) = (*(LL3 + s1)) + (*(W + s2)) * a;			//嵌入公式
		else if (x < 39)
			*(LH3 + s1) = (*(LH3 + s1)) + (*(W + s2)) * a;			//嵌入公式
		else
			*(HL3 + s1) = (*(HL3 + s1)) + (*(W + s2)) * a;			//嵌入公式
	}
	memset(T, 0, 80 * 80);
	Matrices(ULL3, LL3, 80, 80, 80, T);                          //重构LL3
	Matrices(T, VLL3, 80, 80, 80, LL3);
	memset(T, 0, 80 * 80);
	Matrices(UHL3, HL3, 80, 80, 80, T);                          //重构HL3
	Matrices(T, VHL3, 80, 80, 80, HL3);
	memset(T, 0, 80 * 80);
	Matrices(ULH3, LH3, 80, 80, 80, T);                          //重构LH3
	Matrices(T, VLH3, 80, 80, 80, LH3);
	for (x = 80; x < 160; x++)
		for (h = 0; h < 80; h++) /*嵌入LH3子图*/
			YY[x][h] = *(LH3 + h + (x - 80) * 80);
	for (x = 0; x < 80; x++) {
		for (h = 80; h < 160; h++) /*嵌入HL3子图*/
			YY[x][h] = *(HL3 + h - 80 + x * 80);
		for (h = 0; h < 80; h++) /*嵌入LL3子图*/
			YY[x][h] = *(LL3 + h + x * 80);
	}
	DWT2D(YY, 640, 640, 3, 1); /*小波重构*/
	Array2D1D(YY, Y1, 640, 640);                          //载体图像二维转一维
	MsgBox("正在处理", "水印嵌入完成，", "检查效果如下:");
	CheckPSNR(Y0, Y1, 640, 640, a);                       //检查PSNR等值
	YIQ2RGB(OutVector, Y1, I, Q, 640);                   //载体图像YIQ转RGB
//===============================================================
//释放内存
	free(YY);
	free(Y0);
	free(Y1);
	free(I);
	free(Q);
	free(LL3);
	free(HL3);
	free(LH3);
	free(ULL3);
	free(UHL3);
	free(ULH3);
	free(VLL3);
	free(VHL3);
	free(VLH3);
	free(W);
	free(Uw);
	free(Vw);
	free(T);
	free(Wc);
}

///////////////////////水印提取，方式二///////////////////////////////////////////////////////////
//InVector        嵌入水印后载体图片名称
//Vector256       嵌入水印后载体图片灰度化文件名称
//OutWater        提取的水印图片名称
//a               水印嵌入阀值
///////////////////////水印提取，方式二//////////////////////////////////////////////////////////
void RecoverWaterMark2(char *InVector, char *OutWater, double a) {
	int x, h, s1, s2;
	FILE *f;
	double *Y, **YY, *I, *Q, *LL3, *LL3_0, *HL3, *HL3_0, *LH3, *LH3_0, *ULL3,
			*VLL3, *UHL3, *VHL3, *ULH3, *VLH3, *W0, *W, *Uw, *Vw, *T;
	double eps = 0.000001;
	unsigned char *bmphead, *Wc;
//动态分配程序内存
	Y = (double *) malloc(640 * 640 * sizeof(double));
	I = (double *) malloc(640 * 640 * sizeof(double));
	Q = (double *) malloc(640 * 640 * sizeof(double));
	LL3 = (double *) malloc(80 * 80 * sizeof(double));
	LL3_0 = (double *) malloc(80 * 80 * sizeof(double));
	HL3 = (double *) malloc(80 * 80 * sizeof(double));
	HL3_0 = (double *) malloc(80 * 80 * sizeof(double));
	LH3 = (double *) malloc(80 * 80 * sizeof(double));
	LH3_0 = (double *) malloc(80 * 80 * sizeof(double));
	ULL3 = (double *) malloc(80 * 80 * sizeof(double));
	VLL3 = (double *) malloc(80 * 80 * sizeof(double));
	UHL3 = (double *) malloc(80 * 80 * sizeof(double));
	VHL3 = (double *) malloc(80 * 80 * sizeof(double));
	ULH3 = (double *) malloc(80 * 80 * sizeof(double));
	VLH3 = (double *) malloc(80 * 80 * sizeof(double));
	T = (double *) malloc(64 * 64 * sizeof(double));
	W0 = (double *) malloc(64 * 64 * sizeof(double));
	W = (double *) malloc(64 * 64 * sizeof(double));
	Uw = (double *) malloc(64 * 64 * sizeof(double));
	Vw = (double *) malloc(64 * 64 * sizeof(double));
	Wc = (unsigned char *) malloc(64 * 64 * sizeof(unsigned char));
	YY = malloc(640 * sizeof(double));
	for (x = 0; x < 640; x++)
		YY[x] = malloc(640 * sizeof(double));
//============================================================
	RGB2YIQ(InVector, "", Y, I, Q, 640);                  //载体图像RGB转YIQ
	Array1D2D(Y, YY, 640, 640);                       //载体图像矩阵一维转二维
	DWT2D(YY, 640, 640, 3, 0); /*小波分解*/
	for (x = 80; x < 160; x++)
		for (h = 0; h < 80; h++) /*提取LH3子图*/
			*(LH3 + h + (x - 80) * 80) = YY[x][h];
	for (x = 0; x < 80; x++) {
		for (h = 80; h < 160; h++) /*提取HL3子图*/
			*(HL3 + h - 80 + x * 80) = YY[x][h];
		for (h = 0; h < 80; h++) /*提取LL3子图*/
			*(LL3 + h + x * 80) = YY[x][h];
	}
	memset(ULL3, 0, 80 * 80);
	memset(VLL3, 0, 80 * 80);
	memset(UHL3, 0, 80 * 80);
	memset(VHL3, 0, 80 * 80);
	memset(ULH3, 0, 80 * 80);
	memset(VLH3, 0, 80 * 80);
	SVD(LL3, 80, 80, ULL3, VLL3, eps);                        //对LL3奇异值分解
	SVD(HL3, 80, 80, UHL3, VHL3, eps);                        //对HL3奇异值分解
	SVD(LH3, 80, 80, ULH3, VLH3, eps);                        //对LH3奇异值分解
	if ((f = fopen("SLL3.dat", "rb")) == NULL)          //读取SLL3序列，待提取水印时使用
		errorNexit("打开文件失败:", "SLL3.dat");
	fread(LL3_0, 80 * 80 * sizeof(double), 1, f);
	fclose(f);
	if ((f = fopen("SHL3.dat", "rb")) == NULL)         //读取SHL3序列，待提取水印时使用
		errorNexit("打开文件失败:", "SHL3.dat");
	fread(HL3_0, 80 * 80 * sizeof(double), 1, f);
	fclose(f);
	if ((f = fopen("SLH3.dat", "rb")) == NULL)         //读取SLH3序列，待提取水印时使用
		errorNexit("打开文件失败:", "SLH3.dat");
	fread(LH3_0, 80 * 80 * sizeof(double), 1, f);
	fclose(f);
	if ((f = fopen("Uw.dat", "rb")) == NULL)        //读取水印左奇异向量，待提取水印时使用
		errorNexit("打开文件失败:", "Uw.dat");
	fread(Uw, 64 * 64 * sizeof(double), 1, f);
	fclose(f);
	if ((f = fopen("Vw.dat", "rb")) == NULL)        //读取水印右奇异向量，待提取水印时使用
		errorNexit("打开文件失败:", "Vw.dat");
	fread(Vw, 64 * 64 * sizeof(double), 1, f);
	fclose(f);
	memset(W, 0, 64 * 64);                               //检测水印信息
	for (x = 0; x < 64; x++) {
		int s1 = (x + 1) * 81;
		int s2 = x * 65;
		if (x < 14)
			*(W + s2) = ((*(LL3 + s1)) - (*(LL3_0 + s1))) / a;			  //提取公式
		else if (x < 39)
			*(W + s2) = ((*(LH3 + s1)) - (*(LH3_0 + s1))) / a;			//提取公式
		else
			*(W + s2) = ((*(HL3 + s1)) - (*(HL3_0 + s1))) / a;			//提取公式
	}
	Matrices(Uw, W, 64, 64, 64, T);                      //重构水印
	Matrices(T, Vw, 64, 64, 64, W);
	if ((f = fopen("W.dat", "rb")) == NULL)               //读取原始水印数据
		errorNexit("打开文件失败:", "Uw.dat");
	fread(W0, 64 * 64 * sizeof(double), 1, f);
	fclose(f);
	MsgBox("正在处理", "水印提取完成，", "检查效果如下:");
	CheckPSNR(W0, W, 64, 64, a);                     //检查PSNR等值
	for (x = 0; x < 64; x++) /*水印矩阵转为Unchar型*/
		for (h = 0; h < 64; h++)
			*(Wc + h + x * 64) = Double2Char(*(W + h + x * 64));
	Arnold256("", OutWater, Wc, 64, 6, 3, 5, 17, 9, 1);
//================================================================
//释放内存
	free(YY);
	free(Y);
	free(I);
	free(Q);
	free(LL3);
	free(LL3_0);
	free(HL3);
	free(HL3_0);
	free(LH3);
	free(LH3_0);
	free(ULL3);
	free(VLL3);
	free(UHL3);
	free(VHL3);
	free(ULH3);
	free(VLH3);
	free(W);
	free(Uw);
	free(Vw);
	free(T);
	free(Wc);
}

////////////////////////水印嵌入，方式三/////////////////////////////////////////////////////
//InVector        载体图片名称
//OutVector       载体图片灰度化文件名称
//InWater         水印图片名称
//OutWater        水印加密后图片名称
//a               水印嵌入阀值
///////////////////////水印嵌入，方式三//////////////////////////////////////////////////////
void InsertWaterMark3(char *InVector, char *OutVector, char *InWater,
		char *OutWater, double a) {
	int x, h;
	FILE *f;
	double *Y0, *Y1, **YY, *I, *Q, *LH3, *HL3, *HH3, *UHH3, *UHL3, *ULH3, *VHH3,
			*VHL3, *VLH3, *W, *Uw, *Vw, *T;
	double eps = 0.000001, tmp;
	unsigned char *bmphead, *Wc;
//分配内存空间
	Y0 = (double *) malloc(640 * 640 * sizeof(double));
	Y1 = (double *) malloc(640 * 640 * sizeof(double));
	I = (double *) malloc(640 * 640 * sizeof(double));
	Q = (double *) malloc(640 * 640 * sizeof(double));
	HH3 = (double *) malloc(80 * 80 * sizeof(double));
	HL3 = (double *) malloc(80 * 80 * sizeof(double));
	LH3 = (double *) malloc(80 * 80 * sizeof(double));
	UHH3 = (double *) malloc(80 * 80 * sizeof(double));
	UHL3 = (double *) malloc(80 * 80 * sizeof(double));
	ULH3 = (double *) malloc(80 * 80 * sizeof(double));
	VHH3 = (double *) malloc(80 * 80 * sizeof(double));
	VHL3 = (double *) malloc(80 * 80 * sizeof(double));
	VLH3 = (double *) malloc(80 * 80 * sizeof(double));
	T = (double *) malloc(80 * 80 * sizeof(double));
	W = (double *) malloc(64 * 64 * sizeof(double));
	Uw = (double *) malloc(64 * 64 * sizeof(double));
	Vw = (double *) malloc(64 * 64 * sizeof(double));
	Wc = (unsigned char *) malloc(64 * 64 * sizeof(unsigned char));
	YY = malloc(640 * sizeof(double));
	for (x = 0; x < 640; x++)
		YY[x] = malloc(640 * sizeof(double));
//===============================================================
	Arnold256(InWater, OutWater, Wc, 64, 6, 3, 5, 17, 9, 0); /*水印Arnold加密*/
	for (x = 0; x < 64; x++) /*水印矩阵转为double型*/
		for (h = 0; h < 64; h++)
			*(W + h + x * 64) = (double) (*(Wc + h + x * 64));
	if ((f = fopen("W.dat", "wb")) == NULL)              //保存原始水印数据
		errorNexit("打开文件失败:", "W.dat");
	fwrite(W, 64 * 64 * sizeof(double), 1, f);
	fclose(f);
	memset(Uw, 0, 64 * 64);
	memset(Vw, 0, 64 * 64);
	SVD(W, 64, 64, Uw, Vw, eps);                      //对水印矩阵奇异值分解
	if ((f = fopen("Uw.dat", "wb")) == NULL)       //保存水印左奇异向量，待提取水印时使用
		errorNexit("打开文件失败:", "Uw.dat");
	fwrite(Uw, 64 * 64 * sizeof(double), 1, f);
	fclose(f);
	if ((f = fopen("Vw.dat", "wb")) == NULL)      //保存水印右奇异向量，待提取水印时使用
		errorNexit("打开文件失败:", "Vw.dat");
	fwrite(Vw, 64 * 64 * sizeof(double), 1, f);
	fclose(f);
	RGB2YIQ(InVector, "", Y0, I, Q, 640);         //载体图像RGB转YIQ
	Array1D2D(Y0, YY, 640, 640);              //载体图像矩阵一维转二维

	DWT2D(YY, 640, 640, 3, 0); /*小波分解*/
	for (x = 80; x < 160; x++) {
		for (h = 0; h < 80; h++) /*提取LH3子图*/
			*(LH3 + h + (x - 80) * 80) = YY[x][h];
		for (h = 80; h < 160; h++) /*提取HH3子图*/
			*(HH3 + h - 80 + (x - 80) * 80) = YY[x][h];
	}
	for (x = 0; x < 80; x++) /*提取HL3子图*/
		for (h = 80; h < 160; h++)
			*(HL3 + h - 80 + x * 80) = YY[x][h];
	memset(UHH3, 0, 80 * 80);
	memset(VHH3, 0, 80 * 80);
	memset(UHL3, 0, 80 * 80);
	memset(VHL3, 0, 80 * 80);
	memset(ULH3, 0, 80 * 80);
	memset(VLH3, 0, 80 * 80);
	SVD(HH3, 80, 80, UHH3, VHH3, eps);                  //对HH3奇异值分解
	SVD(HL3, 80, 80, UHL3, VHL3, eps);                   //对HL3奇异值分解
	SVD(LH3, 80, 80, ULH3, VLH3, eps);              //对LH3奇异值分解
	if ((f = fopen("SHH3.dat", "wb")) == NULL)         //保存SHH3序列，待提取水印时使用
		errorNexit("打开文件失败:", "SHH3.dat");
	fwrite(HH3, 80 * 80 * sizeof(double), 1, f);
	fclose(f);
	if ((f = fopen("SHL3.dat", "wb")) == NULL)         //保存SHL3序列，待提取水印时使用
		errorNexit("打开文件失败:", "SHL3.dat");
	fwrite(HL3, 80 * 80 * sizeof(double), 1, f);
	fclose(f);
	if ((f = fopen("SLH3.dat", "wb")) == NULL)         //保存SLH3序列，待提取水印时使用
		errorNexit("打开文件失败:", "SLH3.dat");
	fwrite(LH3, 80 * 80 * sizeof(double), 1, f);
	fclose(f);
	for (x = 0; x < 64; x++)                            //嵌入水印系数
			{
		int s1 = (x + 1) * 81;
		int s2 = x * 65;
		*(HH3 + s1) = (*(HH3 + s1)) + (*(W + s2)) * a;			          //嵌入公式
		*(HL3 + s1) = (*(HL3 + s1)) + (*(W + s2)) * a;			          //嵌入公式
		*(LH3 + s1) = (*(LH3 + s1)) + (*(W + s2)) * a;			          //嵌入公式
	}
	memset(T, 0, 80 * 80);
	Matrices(UHH3, HH3, 80, 80, 80, T);                          //重构HH3
	Matrices(T, VHH3, 80, 80, 80, HH3);
	memset(T, 0, 80 * 80);
	Matrices(UHL3, HL3, 80, 80, 80, T);                          //重构HL3
	Matrices(T, VHL3, 80, 80, 80, HL3);
	memset(T, 0, 80 * 80);
	Matrices(ULH3, LH3, 80, 80, 80, T);           //重构LH3
	Matrices(T, VLH3, 80, 80, 80, LH3);
	for (x = 80; x < 160; x++) {
		for (h = 0; h < 80; h++) /*嵌入LH3子图*/
			YY[x][h] = *(LH3 + h + (x - 80) * 80);
		for (h = 80; h < 160; h++) /*嵌入HH3子图*/
			YY[x][h] = *(HH3 + h - 80 + (x - 80) * 80);
	}
	for (x = 0; x < 80; x++) /*嵌入HL3子图*/
		for (h = 80; h < 160; h++)
			YY[x][h] = *(HL3 + h - 80 + x * 80);
	DWT2D(YY, 640, 640, 3, 1); /*小波重构*/
	Array2D1D(YY, Y1, 640, 640);             //载体图像二维转一维
	MsgBox("正在处理", "水印嵌入完成，", "检查效果如下:");
	CheckPSNR(Y0, Y1, 640, 640, a);            //检查PSNR等值
	YIQ2RGB(OutVector, Y1, I, Q, 640);          //载体图像YIQ转RGB
//===============================================================
//释放内存
	free(YY);
	free(Y0);
	free(Y1);
	free(I);
	free(Q);
	free(HH3);
	free(HL3);
	free(LH3);
	free(UHH3);
	free(UHL3);
	free(ULH3);
	free(VHH3);
	free(VHL3);
	free(VLH3);
	free(W);
	free(Uw);
	free(Vw);
	free(T);
	free(Wc);
}

///////////////////////水印提取，方式三//////////////////////////////////////////////////////////////
//InVector        嵌入水印后载体图片名称
//Vector256       嵌入水印后载体图片灰度化文件名称
//OutWater        提取的水印图片名称
//a               水印嵌入阀值
///////////////////////水印提取，方式三//////////////////////////////////////////////////////////////
void RecoverWaterMark3(char *InVector, char *OutWater, double a) {
	int x, h, s1, s2;
	FILE *f;
	double *Y, **YY, *I, *Q, *HH3, *HH3_0, *HL3, *HL3_0, *LH3, *LH3_0, *UHH3,
			*VHH3, *UHL3, *VHL3, *ULH3, *VLH3, *W0, *W, *Uw, *Vw, *T;
	double eps = 0.000001;
	unsigned char *bmphead, *Wc;
//动态分配程序内存
	Y = (double *) malloc(640 * 640 * sizeof(double));
	I = (double *) malloc(640 * 640 * sizeof(double));
	Q = (double *) malloc(640 * 640 * sizeof(double));
	HH3 = (double *) malloc(80 * 80 * sizeof(double));
	HH3_0 = (double *) malloc(80 * 80 * sizeof(double));
	HL3 = (double *) malloc(80 * 80 * sizeof(double));
	HL3_0 = (double *) malloc(80 * 80 * sizeof(double));
	LH3 = (double *) malloc(80 * 80 * sizeof(double));
	LH3_0 = (double *) malloc(80 * 80 * sizeof(double));
	UHH3 = (double *) malloc(80 * 80 * sizeof(double));
	VHH3 = (double *) malloc(80 * 80 * sizeof(double));
	UHL3 = (double *) malloc(80 * 80 * sizeof(double));
	VHL3 = (double *) malloc(80 * 80 * sizeof(double));
	ULH3 = (double *) malloc(80 * 80 * sizeof(double));
	VLH3 = (double *) malloc(80 * 80 * sizeof(double));
	T = (double *) malloc(64 * 64 * sizeof(double));
	W0 = (double *) malloc(64 * 64 * sizeof(double));
	W = (double *) malloc(64 * 64 * sizeof(double));
	Uw = (double *) malloc(64 * 64 * sizeof(double));
	Vw = (double *) malloc(64 * 64 * sizeof(double));
	Wc = (unsigned char *) malloc(64 * 64 * sizeof(unsigned char));
	YY = malloc(640 * sizeof(double));
	for (x = 0; x < 640; x++)
		YY[x] = malloc(640 * sizeof(double));
//============================================================
	RGB2YIQ(InVector, "", Y, I, Q, 640);                  //载体图像RGB转YIQ
	Array1D2D(Y, YY, 640, 640);                       //载体图像矩阵一维转二维
	DWT2D(YY, 640, 640, 3, 0); /*小波分解*/
	for (x = 80; x < 160; x++) {
		for (h = 0; h < 80; h++) /*提取LH3子图*/
			*(LH3 + h + (x - 80) * 80) = YY[x][h];
		for (h = 80; h < 160; h++) /*提取HH3子图*/
			*(HH3 + h - 80 + (x - 80) * 80) = YY[x][h];
	}
	for (x = 0; x < 80; x++) /*提取HL3子图*/
		for (h = 80; h < 160; h++)
			*(HL3 + h - 80 + x * 80) = YY[x][h];
	memset(UHH3, 0, 80 * 80);
	memset(VHH3, 0, 80 * 80);
	memset(UHL3, 0, 80 * 80);
	memset(VHL3, 0, 80 * 80);
	memset(ULH3, 0, 80 * 80);
	memset(VLH3, 0, 80 * 80);
	SVD(HH3, 80, 80, UHH3, VHH3, eps);                 //对HH3奇异值分解
	SVD(HL3, 80, 80, UHL3, VHL3, eps);                 //对HL3奇异值分解
	SVD(LH3, 80, 80, ULH3, VLH3, eps);                 //对LH3奇异值分解
	if ((f = fopen("SHH3.dat", "rb")) == NULL)          //读取SHH3序列，待提取水印时使用
		errorNexit("打开文件失败:", "SHH3.dat");
	fread(HH3_0, 80 * 80 * sizeof(double), 1, f);
	fclose(f);
	if ((f = fopen("SHL3.dat", "rb")) == NULL)          //读取SHL3序列，待提取水印时使用
		errorNexit("打开文件失败:", "SHL3.dat");
	fread(HL3_0, 80 * 80 * sizeof(double), 1, f);
	fclose(f);
	if ((f = fopen("SLH3.dat", "rb")) == NULL)          //读取SLH3序列，待提取水印时使用
		errorNexit("打开文件失败:", "SLH3.dat");
	fread(LH3_0, 80 * 80 * sizeof(double), 1, f);
	fclose(f);
	if ((f = fopen("Uw.dat", "rb")) == NULL)        //读取水印左奇异向量，待提取水印时使用
		errorNexit("打开文件失败:", "Uw.dat");
	fread(Uw, 64 * 64 * sizeof(double), 1, f);
	fclose(f);
	if ((f = fopen("Vw.dat", "rb")) == NULL)       //读取水印右奇异向量，待提取水印时使用
		errorNexit("打开文件失败:", "Vw.dat");
	fread(Vw, 64 * 64 * sizeof(double), 1, f);
	fclose(f);
	memset(W, 0, 64 * 64);                               //检测水印信息
	for (x = 0; x < 64; x++) {
		s1 = (x + 1) * 81;
		s2 = x * 65;
		double HH = ((*(HH3 + s1)) - (*(HH3_0 + s1))) / a;          //提取公式
		double HL = ((*(HL3 + s1)) - (*(HL3_0 + s1))) / a;           //提取公式
		double LH = ((*(LH3 + s1)) - (*(LH3_0 + s1))) / a;                //提取公式
		*(W + s2) = MAX(HH, HL, LH) * 1.5;                         //提取公式
	}
	Matrices(Uw, W, 64, 64, 64, T);                            //重构水印
	Matrices(T, Vw, 64, 64, 64, W);
	if ((f = fopen("W.dat", "rb")) == NULL)                     //读取原始水印数据
		errorNexit("打开文件失败:", "Uw.dat");
	fread(W0, 64 * 64 * sizeof(double), 1, f);
	fclose(f);
	MsgBox("正在处理", "水印提取完成，", "检查效果如下:");
	CheckPSNR(W0, W, 64, 64, a);                           //检查PSNR等值
	for (x = 0; x < 64; x++) /*水印矩阵转为Unchar型*/
		for (h = 0; h < 64; h++)
			*(Wc + h + x * 64) = Double2Char(*(W + h + x * 64));
	Arnold256("", OutWater, Wc, 64, 6, 3, 5, 17, 9, 1);
//================================================================
//释放内存
	free(YY);
	free(Y);
	free(I);
	free(Q);
	free(HH3);
	free(HH3_0);
	free(HL3);
	free(HL3_0);
	free(LH3);
	free(LH3_0);
	free(UHH3);
	free(VHH3);
	free(UHL3);
	free(VHL3);
	free(ULH3);
	free(VLH3);
	free(W);
	free(Uw);
	free(Vw);
	free(T);
	free(Wc);
}
