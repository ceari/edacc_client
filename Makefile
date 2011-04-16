# EDACC Client Makefile

# dynamically linked / BWGRID Ulm:
CFLAGS=-ggdb -g -W -Wall -Wextra `mysql_config --cflags`
LDFLAGS=`mysql_config --libs` -lpthread


CC = g++
COMPILE= $(CC) $(CFLAGS) -c

OBJ_FILES=host_info.o client.o database.o log.o file_routines.o md5sum.o signals.o LzmaDec.o lzma.o Alloc.o 7zStream.o 7zFile.o

all: client

client: $(OBJ_FILES)
	$(CC) $(CFLAGS) $(OBJ_FILES) -o client $(LDFLAGS)

client.o: client.cc *.h
	$(COMPILE) client.cc

host_info.o: host_info.cc host_info.h
	$(COMPILE) host_info.cc

database.o: database.cc database.h
	$(COMPILE) database.cc
  
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
	
clean:
	rm -f *.o
	rm -f client

tags: *.cc
	ctags *.cc
