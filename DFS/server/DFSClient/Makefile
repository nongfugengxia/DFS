#CC = clang
CC = gcc
CFLAGS = -Wall -g -O0
LDFLAGS = -pthread -include /usr/include/errno.h

SOURCES = $(wildcard ./*.c)
OBJECTS = $(patsubst %.c,%.o,$(SOURCES))

TARGET = bin/client

all: $(TARGET)

$(TARGET): build $(OBJECTS) 
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET) $(LDFLAGS) -D_FORTIFY_SOURCE=2

clean:
	rm -rf bin
	rm *.o

build:
	@mkdir -p bin
