# EDACC Client Makefile

# dynamically linked / BWGRID Ulm:
CFLAGS=-ggdb -g -W -Wall -Wextra `mysql_config --cflags` -O2
LDFLAGS=`mysql_config --libs` -lpthread -lz

ifeq ($(USE_HWLOC),1)
CFLAGS += -Duse_hwloc
ifeq ($(HWLOC_INCLUDE),)
CFLAGS += `pkg-config --cflags hwloc`
else
CFLAGS += -I$(HWLOC_INCLUDE)
endif
ifeq ($(HWLOC_LIB),)
LDFLAGS += $(LDFLAGS_) `pkg-config --libs hwloc`
else
LDFLAGS += -L$(HWLOC_LIB) -lhwloc
endif
endif

ifeq ($(STATIC),1)
LDFLAGS += -static
endif

CC = g++
COMPILE= $(CC) $(CFLAGS) -c

OBJ_FILES=host_info.o client.o database.o database_fs_locking.o log.o file_routines.o md5sum.o signals.o LzmaDec.o lzma.o Alloc.o 7zStream.o 7zFile.o messages.o ioapi.o miniunz.o unzip.o process.o simulate.o jobserver.o

.PHONY: all clean

all: ../bin/client

../bin/client: $(OBJ_FILES)
	$(CC) $(CFLAGS) $(OBJ_FILES) -o ../bin/client $(LDFLAGS)

client.o: client.cc *.h
	$(COMPILE) client.cc

host_info.o: host_info.cc host_info.h
	$(COMPILE) host_info.cc

database.o: database.cc database.h
	$(COMPILE) database.cc

database_fs_locking.o: database_fs_locking.cc database_fs_locking.h
	$(COMPILE) database_fs_locking.cc
	
log.o: log.cc log.h
	$(COMPILE) log.cc

signals.o: signals.cc signals.h
	$(COMPILE) signals.cc

file_routines.o: file_routines.cc file_routines.h
	$(COMPILE) file_routines.cc

md5sum.o: md5sum.c md5sum.h
	$(COMPILE) md5sum.c
	
LzmaDec.o:
	$(COMPILE) lzma/C/LzmaDec.c
	
lzma.o: lzma.cc lzma.h
	$(COMPILE) lzma.cc
	
Alloc.o:
	$(COMPILE) lzma/C/Alloc.c
	
7zStream.o:
	$(COMPILE) lzma/C/7zStream.c
	
7zFile.o:
	$(COMPILE) lzma/C/7zFile.c

ioapi.o: unzip/ioapi.c unzip/ioapi.h
	$(COMPILE) unzip/ioapi.c

miniunz.o: unzip/miniunz.c unzip/miniunz.h
	$(COMPILE) unzip/miniunz.c

unzip.o: unzip/unzip.c unzip/unzip.h
	$(COMPILE) unzip/unzip.c
	
messages.o: messages.cc messages.h
	$(COMPILE) messages.cc
	
process.o: process.cc process.h
	$(COMPILE) process.cc
	
simulate.o: simulate.cc simulate.h
	$(COMPILE) simulate.cc
	
jobserver.o: jobserver.cc jobserver.h
	$(COMPILE) jobserver.cc
	
clean:
	rm -f *.o
	rm -f ../bin/client

tags: *.cc
	ctags *.cc
