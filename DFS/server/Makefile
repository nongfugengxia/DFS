#CC = clang
CC = gcc
CFLAGS = -Wall -g -O0
LDFLAGS = -pthread -include /usr/include/errno.h -liniparser -L../third/iniparser-master 

SOURCES = $(wildcard src/*.c)
OBJECTS = $(patsubst %.c,%.o,$(SOURCES)) 
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
FDFS_SHARE_OBJS = -L../third/libfastcommon-1.0.7/lib -lfastcommon
TOOLS_OBJS		= -L../tools/lib -lcommonTools \
					-L../DES ../DES/DES.o
ALL_OBJS = ${JPEGOPTIM} $(FDFS_STATIC_OBJS) ${FDFS_SHARE_OBJS} ${TOOLS_OBJS}

TARGET = bin/server

all: $(TARGET)

$(TARGET): build $(OBJECTS) 
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET) -D_FORTIFY_SOURCE=2 ../middle/DFS.o $(LDFLAGS) ${ALL_OBJS} -Wl,-rpath=/home/ubuntu/install/eclipse/workspace/DFS/third/libfastcommon-1.0.7/lib
	cd DFSClient && make && cd ..

clean:
	rm -rf bin
	rm src/*.o

build:
	@mkdir -p bin
