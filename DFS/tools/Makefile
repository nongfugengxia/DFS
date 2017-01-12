#########################################################################
# 
# Makefile for common tools
#

CC		= gcc
SRC		= commonTools
INCLUDE	= -I../third/jpegoptim-master

JPEGOPTIM_OBJS = ../third/jpegoptim-master/jpegoptim.o ../third/jpegoptim-master/misc.o ../third/jpegoptim-master/jpegdest.o
JPEGOPTIM_LIBS = -lm -ljpeg -L../usr/lib/x86_64-linux-gnu
#JPEGOPTIM_CFLAG = -Wl,-rpath=/home/ubuntu/install/eclipse/workspace/DFS/third/jpeg-9/jpeg/lib
JPEGOPTIM_CFLAG = -Wl,-rpath=/usr/lib/x86_64-linux-gnu


# should be no reason to modify lines below this
#########################################################################
all:
	${CC} -c -fPIC -o ${SRC}.o  ${SRC}.c 	# compile, PIC(Position Independent Code)
	${CC} -shared -o ./lib/lib${SRC}.so ${SRC}.o		# generate share library
	rm *.o
	
	${CC} -o test test.c -lcommonTools -L./lib -Wl,-rpath=./tools/lib 	# need to delete "lib"和".so"
	
	${CC} -o ./compress/compressJPEG ./compress/compressJPEG.c ${INCLUDE} ${JPEGOPTIM_OBJS} ${JPEGOPTIM_LIBS} ${JPEGOPTIM_CFLAG} 	#编译压缩JPEG主程序
	cd ../third/jpegoptim-master &&  make && cd ../../tools	#编译jpegoptim源代码，生成.o文件
	
	${CC} -g ./watermark/watermark.c -lm -o ./watermark/watermark	#编译加水印的代码
	${CC} -g ./watermark/readBMPHeader.c  -o ./watermark/readBMPHeader
	
	
clean:
	rm test
	cd compress && rm compressJPEG && cd ..
	cd watermark && rm watermark && rm readBMPHeader && cd ..
	
	