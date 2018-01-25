CFLAGS =  -g -Wall
CC = gcc

BUILD	= ../../build

# INSTALL_DIR = /home/john/bin
# MANPATH=/usr/local/man/man1

SRCS	= ositerm.c console.c
BINS	= ositerm serial_test 
OBJS	= console.o  

MANSRC  = 
MANFILE	= 
HEAD	= 
LIBS	= 
INCS 	:= $(wildcard *.h)

all: ositerm console.o 

console.o: console.c $(INCS)
	$(CC) -c -o $(BUILD)/console.o $(CFLAGS) console.c

ositerm: $(SRCS) $(INCS)
	$(CC) -o $(BUILD)/ositerm $(CFLAGS) ositerm.c 

serial_test: $(SRCS)
	$(CC) -o serial_test $(CFLAGS) serial_test.c 



# %.o: %.c $(INCS)
# 	$(CC) -c -o $@ $< $(CFLAGS)

# clean:
# 	rm -rf $(BINS) $(OBJS)	

# install:
# 	cp $(BINS) $(INSTALL_DIR)
#	cp $(MANSRC) $(MANPATH)/$(MANFILE)

# uninstall:
# 	rm $(INSTALL_DIR)/$(BINS)
#	rm $(MANPATH)/$(MANFILE)


# MAKEFILE NOTES
# $@  -- the file named on the left side of the :
# $<  -- the first item in the dependencies list


