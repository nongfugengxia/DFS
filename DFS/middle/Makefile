CC		= gcc

FDFS_STATIC_OBJS = ../client/fdfsClient.o ../client/fdfs_upload_file.o \
					../client/fdfs_download_file.o ../client/fdfs_delete_file.o  \
					../client/client_global.o ../client/storage_client.o \
					../client/tracker_client.o ../client/client_func.o \
					../tracker/fdfs_shared_func.o ../tracker/tracker_proto.o \
					../common/fdfs_global.o ../common/fdfs_http_shared.o ../common/mime_file_parser.o \
					../storage/trunk_mgr/trunk_shared.o

JPEGOPTIM_OBJS = ../third/jpegoptim-master/jpegoptim.o ../third/jpegoptim-master/misc.o ../third/jpegoptim-master/jpegdest.o 
JPEGOPTIM_LIBS = -lm -ljpeg -L../usr/lib/x86_64-linux-gnu
JPEGOPTIM 		= ${JPEGOPTIM_OBJS} ${JPEGOPTIM_LIBS}
         			
ALL_OBJS = ${JPEGOPTIM} $(FDFS_STATIC_OBJS)

SRC			= DFS.c
INC			= -I../client -I../third/iniparser-master/src -I../tools -I../DES
LIB_PATH	= -L../third/libfastcommon-1.0.7/lib -L../third/iniparser-master -L../tools/lib
LIB			= -lfastcommon -lpthread -liniparser
DEPENDENCE	= ${INC} ${ALL_OBJS} ${LIB} 


all:
#	${CC} -I../client -o DFS DFS.cB
	${CC} -g -O0 ${INC} ${ALL_OBJS} ${LIB_PATH} ${LIB} -c ${SRC}	# 编译成.o文件
	${CC} -g -O0 ${DEPENDENCE} DFS.o ../DES/DES.o ${LIB_PATH} -liniparser -lcommonTools -Wl,-rpath=./tools/lib -c DFS.c	# 链接成exe文件
	

clean:
	rm *.o
