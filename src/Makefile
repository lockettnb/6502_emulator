#
#  makefile for 6502 monitor/emulator/dissassembler
#
#

#CFLAGS =  -g -DDEBUG
#CFLAGS =  -g -Wall
CFLAGS =  -g
CC = gcc

BUILD	= ../build
INSTALL_DIR = /home/john/bin
MANPATH=/usr/local/man/man1

SUBOBJS  = linenoise.o dumbterm.o
SRCS	= disassembler.c emulator.c utils.c monitor.c floppy.c
BIN		= $(addprefix $(BUILD)/,osi6502)
OBJS	= $(addprefix $(BUILD)/, disassembler.o emulator.o utils.o monitor.o console.o floppy.o linenoise.o filecomp.o)
MANSRC  = 
MANFILE	= 
HEAD	= 
LIBS	= -lreadline
INCS 	:= $(wildcard *.h)

all: osi6502  ositerm

osi6502: $(SRCS) $(OBJS) $(INCS) 
	$(CC) -o $(BIN) $(CFLAGS) $(OBJS) $(LIBS)

$(BUILD)/console.o: console/console.c console/console.h
	make -C console/

ositerm: console/ositerm.c 
	make -C console/

$(BUILD)/linenoise.o: ln/linenoise.c ln/linenoise.h
	make -C ln/

$(BUILD)/filecomp.o: ln/filecomp.c ln/filecomp.h
	$(CC) -c -o $(BUILD)/filecomp.o filecomp.c $(CFLAGS)

$(BUILD)/%.o: %.c $(INCS)
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	rm -rf $(BUILD)/*

install:
	cp $(BINS) $(INSTALL_DIR)
#	cp $(MANSRC) $(MANPATH)/$(MANFILE)

uninstall:
	rm $(INSTALL_DIR)/$(BINS)
#	rm $(MANPATH)/$(MANFILE)


# MAKEFILE NOTES
# $@  -- the file named on the left side of the :
# $<  -- the first item in the dependencies list

