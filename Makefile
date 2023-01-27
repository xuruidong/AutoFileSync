CC = g++
#CC = /mingw64/bin/g++
VPATH = .:./tinyxml:./common
INCLUDE_PATH = -I./src/curl/include
LIBRARY_PATH = -L./src/curl/lib
CFLAGS = -Wall -O0 -g -DCURL_STATICLIB -I./tinyxml -I./common -I./include ${INCLUDE_PATH}
LIBS = -lcurl.dll -lcurl
OUTPUT = fsync

SRCS1 := main.cpp monitor.cpp file_sync_contex.cpp sftpUpload.cpp util.cpp
OBJS1 := $(addsuffix .o,$(basename ${SRCS}))

SRCS := $(wildcard src/*.cpp)
BASENAMES := $(basename $(notdir ${SRCS}))
#OBJS := $(addprefix objs/,$(addsuffix .o,$(BASENAMES)))
OBJS := $(addsuffix .o,$(basename ${SRCS}))

all:${OUTPUT}

%.o:%.cpp
	${CC} -c ${CFLAGS} $< -o $@
	
${OUTPUT}:${OBJS}
	${CC} -o $@ ${OBJS} ${LIBRARY_PATH} ${LIBS}

clean:
	rm -f ${OBJS} ${OUTPUT}



