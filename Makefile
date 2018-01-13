#
#  makefile for 6502 monitor/emulator/dissassembler
#
#

#CFLAGS =  -g -DDEBUG
CFLAGS =  -g
CC = gcc

INSTALL_DIR = /home/john/bin
MANPATH=/usr/local/man/man1

SRCS	= disassembler.c emulator.c utils.c monitor.c console.c
BINS	= m6502
OBJS	= disassembler.o emulator.o utils.o monitor.o console.o
MANSRC  = 
MANFILE	= 
HEAD	= 
LIBS	= -lreadline
INCS 	:= $(wildcard *.h)

all: $(BINS)

m6502: $(SRCS) $(OBJS) $(INCS) 
	$(CC) -o m6502 $(CFLAGS) $(OBJS) $(LIBS)


%.o: %.c $(INCS)
	$(CC) -c -o $@ $< $(CFLAGS)

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

