.SUFFIXES: .c .o .lo

COMPILE = $(CC) $(CFLAGS) -g -Wall -O -D_FILE_OFFSET_BITS=64 -DDEBUG
ENABLE_STATIC_LIB = $(ENABLE_STATIC_LIB)
ENABLE_SHARED_LIB = $(ENABLE_SHARED_LIB)
INC_PATH = -I../third/fastdfs-5.05/include/fastdfs -I../third/libfastcommon-1.0.7/include
LIB_PATH = -lfdfsclient -lfastcommon -L../third/fastdfs-5.05/lib64 -L../third/libfastcommon-1.0.7/lib 
TARGET_PATH = $(TARGET_PREFIX)/bin

#SHARED_OBJS = common_func.o dfs_func.o
SHARED_OBJS = common_func.o dfs_func_pc.o

ALL_OBJS = $(SHARED_OBJS)

ALL_PRGS = gen_files test_upload test_download test_delete combine_result

all: $(ALL_OBJS) $(ALL_PRGS)
.o:
	$(COMPILE) -o $@ $<  $(SHARED_OBJS) $(LIB_PATH) $(INC_PATH)
.c:
	$(COMPILE) -o $@ $<  $(SHARED_OBJS) $(LIB_PATH) $(INC_PATH)
.c.o:
	$(COMPILE) -c -o $@ $<  $(INC_PATH)
.c.lo:
	$(COMPILE) -c -fPIC -o $@ $<  $(INC_PATH)
install:
	mkdir -p $(TARGET_PATH)
	cp -f $(ALL_PRGS) $(TARGET_PATH)
clean:
	rm -f $(ALL_OBJS) $(ALL_PRGS)
