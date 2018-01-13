//
//
// 2014/10/30 created
// 2018/01/05 updated added tty and signals


#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/stat.h>
#include <string.h>
#include <ctype.h>
// for tty and signals
#include <termios.h>
#include <signal.h>
#include <unistd.h>
// regular expressions
#include <regex.h>

#include "e6502.h"

// Globals used for tty and signals
int sentinel = FALSE;
static struct termios save_termios;
// static int ttysavefd = -1;
static enum { RESET, RAW, CBREAK } ttystate = RESET;


// ******************************************************************
// trim white space oof front/back of a  command line string
char *trimwhite (char *string)
{
  register char *s, *t;

  for (s = string; isspace (*s); s++)
    ;

  if (*s == 0)
    return (s);

  t = s + strlen (s) - 1;
  while (t > s && isspace (*t))
    t--;
  *++t = '\0';

  return s;
}



// replace the pattern with replacement string in target
//    sort of like perl: target =~ s/pattern/replacement/g 
void str_replace(char *target, const char *pattern, const char *replacement)
{
 char   buffer[128] = { 0 };
 char   *ip = &buffer[0];
 char   *p;
 char   *tp = target;
 size_t pat_len = strlen(pattern);
 size_t repl_len = strlen(replacement);

 regex_t *rg;
 regmatch_t match_buf[1];
 int rc = 0;
 int so;
 int eo;

    // make room for the regular expression and zero it
    rg = (regex_t *) malloc(sizeof(regex_t));
    memset(rg, 0, sizeof(regex_t));

    rc=regcomp(rg, pattern, REG_EXTENDED); 
    if (rc !=0) {
        fprintf(stderr, "Error: regular expression failed to compile\n");
        exit(1);
    }

    while (rc == 0) {  // loop changing one match at a time
        rc = regexec(rg, target, 1, match_buf ,0);

        if(rc == 0) {
            so=match_buf[0].rm_so;
            eo=match_buf[0].rm_eo;

            // copy part before pattern
            memcpy(ip, tp, so);
            ip = ip + so;

            // insert replacement part 
            memcpy(ip, replacement, repl_len);
            ip = ip + repl_len;

            // copy rest of target string
            strcpy(ip, tp+eo);

            // copy temp buffer back into the target string
            strcpy(target, buffer);
            ip =&buffer[0];
        }
    }
}   // end of str_replace


// ******************************************************************
// figure out the size of the file we are loading into memory
int fsize(const char *filename) {
    struct stat st; 

    if (stat(filename, &st) == 0)
        return st.st_size;

    return -1; 
}

// load the binary file into the memory buffer
int load_file(FILE *fp, char *fname, int addr)
{
int filesize;                   // size of the file we are trying to load
int loadsize;                   // number of byte we will actually load into memory
uint8_t buffer[MEMORY_SIZE];    // temp buffer 
int i;

    filesize = fsize(fname);
    if (filesize == -1) {
        fputs("Error: reading file", stderr);
        exit(1);
    }

    if (filesize == 0) {
        fputs("Error: file is empty", stderr);
        exit(1);
    }
    
    if(addr+filesize <= MEMORY_SIZE) {
        loadsize=filesize;
    }
    else {
        loadsize = MEMORY_SIZE-addr;
    }

    filesize = fread(buffer, sizeof(char), loadsize, fp);
    if (filesize == 0) {
        fputs("Error> reading file", stderr);
        exit(1);
    }
//     printf(" load size %i\n", filesize);
    for(i=0; i<loadsize; i++){
        mwrite(addr+i, buffer[i]);
    }

    return(filesize);
}


// ******************************************************************
// Print Usage Instructions
void inst(char *iptr[], int status)
{
if(status != 0)
    fprintf(stderr,"Try, %s --help for more information.\n", program_name);

else {
    if (version) {
        printf("%s %s \n", program_name, VERSION);
        exit(SUCCESS);
        }

    printf("Usage: %s [OPTION]... [FILE]...\n", program_name);
    while (*iptr != NULL_CHAR)
      puts(*iptr++);
    }

exit(status == 0 ? SUCCESS : FAIL);
} /* inst */




// ******************************************************************
// TTY ROUTINES
// tty to raw input
int tty_raw(int fd)
{
 struct termios buf;

    if (tcgetattr(fd, &save_termios) < 0)
        return(-1);

    buf = save_termios; 
    buf.c_lflag &= ~ (ECHO | ICANON | IEXTEN );
    //  buf.c_lflag &= ~ (ECHO | ICANON | IEXTEN | ISIG);

    buf.c_iflag &= ~ (BRKINT | ICRNL | INPCK | ISTRIP | IXON);

    buf.c_cflag |= ~(CSIZE | PARENB);

    buf.c_oflag &= ~ (OPOST);

    buf.c_cc[VMIN] = 1;
    buf.c_cc[VTIME]= 0;

    if (tcsetattr(fd, TCSAFLUSH, &buf) < 0)
       return(-1);

    ttystate = RAW;
//     ttysavefd = fd;
    return(0);
}

// restore tty to cooked input
int tty_reset(int fd)
{
    if((ttystate != CBREAK && ttystate != RAW))
      return(0);

    if(tcsetattr(fd, TCSAFLUSH, &save_termios) < 0)
      return(-1);

    ttystate = RESET;
    return(0);

}



// ******************************************************************
// signal hander for SIGINT
void sig_handler(int signo)
{
  if (signo == SIGINT) {
    printf("\nreceived SIGINT\n");
    sentinel = TRUE;
  }
}


// grab a hold of the SIGINT signal
void sigint_grab(void)
{
    if (signal(SIGINT, sig_handler) == SIG_ERR)
        printf("\ncan't catch SIGINT\n");

}

// restore SIGINT to default
void sigint_restore(void)
{
    signal(SIGINT, SIG_DFL);
}


