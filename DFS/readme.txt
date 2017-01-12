编译：根目录下执行make


server: 服务端程序
    DFSClient       对应服务端程序的客户端程序
        client.c    客户端
    src
        server.c    服务端


middle: 中间层功能
	DFS.c			提供的功能接口，包括初始化、上传、下载、删除


client: FastDFS的存储层客户端
	fdfsClient.c, fdfsClient.h						FastDFS对外提供的上传、下载和删除功能
	testcase.c										测试上传、下载
	fdfs_upload_file.h, fdfs_upload_file.c:			上传的接口和实现
	fdfs_download_file.h, fdfs_download_file.c:		下载的接口和实现
	fdfs_delete_file.h, fdfs_delete_file.c:			 删除的接口和实现


tools: 包装了系统所需要的工具功能


common: FastDFS所用到的公共模块


conf:	系统配置文件所在路径
	client.conf:	FastDFS存储客户端的配置文件(配置访问的tracker)
	DFS.conf:		分布式数据库的配置文件
	twisted.ini		iniparser能解析的样例配置文件
	

data:	系统临时存放数据的目录
	compressed:		压缩后的文件
	download:		下载文件默认的存放路径
	terrain.jpg		测试的数据文件
	
	
DES: DES算法加密模块
	des.h, des.c 加密算法的实现
	run_des.h, run_des.c 加密实例
	DES.h, DES.c 对外提供的接口（调用加密的实例）
	系统初始化：
	生成加密解密链接所需要的密钥，内存中常驻(目前是每次加密解密都需要读取DES.key,可优化)；
	

design:	系统的设计图


init:	系统初始化工作(功能已经合并进入./middle/DFS/init()函数中)
	parseConfig.c:	解析配置文件


note:	开发过程的笔记


storage:	FastDFS的storage节点代码


test:		FastDFS性能测试


third:		第三方库
	fastdfs-5.05:		FastDFS编译安装内容
	iniparser-master:	配置文件解析的源代码
	jpeg-9				JPEG-9的源代码（系统链接的是jpeg-8）
	jpegoptim:			图片压缩编译安装内容
	jpegoptim-master:	图片压缩的源代码
	libfastcommon-1.0.7:FastDFS引用的公有库


tools:		系统公共功能模块
	compress			测试数据位于内存的压缩功能，调用./third/jpegoptim-master/jpegoptim.c(jpegoptimMain函数)
	lib:				编译后的库文件
	compressed:			临时文件夹，用于存放压缩后的图片
	watermark:			图片增加水印功能的测试
	commonTools.c		从磁盘读取文件的压缩功能
	test.c				测试从磁盘读取文件的压缩功能
	
	
tracker:	FastDFS的tracker服务器的代码








