CFLAGS =  -g -Wall
CC = gcc

INSTALL_DIR = /home/john/bin
MANPATH=/usr/local/man/man1
BUILD	=	../build/

SRCS	= dumbterm.c serial_test.c 
BINS	= dumbterm serial_test 
OBJS	= 

MANSRC  = 
MANFILE	= 
HEAD	= 
LIBS	= 
INCS 	:= $(wildcard *.h)

all: $(BINS)

serial_test: $(SRCS)
	$(CC) -o serial_test $(CFLAGS) serial_test.c 


dumbterm: $(SRCS)
	$(CC) -o dumbterm $(CFLAGS) dumbterm.c 


# %.o: %.c $(INCS)
# 	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	rm -rf $(BINS) $(OBJS)	

install:
	cp $(BINS) $(INSTALL_DIR)
#	cp $(MANSRC) $(MANPATH)/$(MANFILE)

uninstall:
	rm $(INSTALL_DIR)/$(BINS)
#	rm $(MANPATH)/$(MANFILE)


# MAKEFILE NOTES
# $@  -- the file named on the left side of the :
# $<  -- the first item in the dependencies list


