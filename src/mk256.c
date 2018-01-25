/*
*   6502 dissassembler -- make a file with all 256 byte values
*
*
*  2014/110/28 created
*
*
*/
#define VERSION "1.00"

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define NULL_CHAR '\0'
#define TRUE -1
#define FALSE 0
#define SUCCESS 0
#define FAIL 1
#define MAXMATCH 3

int main (int argc, char **argv)
{
int i = 0;                  // misc index


for(i=0; i<=255; i++) {
    putchar(i);
}
exit(SUCCESS);
}   /* main */







