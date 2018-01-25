//
// 2018/01/19 
//
#include <stdio.h> 
#include <stdlib.h>
#include <sys/ioctl.h>      // for ioctl
#include <unistd.h>         // for STDIN_FILENO 
#include <string.h>         // for strlen
#include <stdlib.h>         // for malloc
#include <errno.h>
#include <glob.h>

#include "linenoise.h"
#include "../common.h"


// prototypes for functions at the bottom of the file
int globerr(const char *path, int eerrno);
void statusline(char line[]);



void filecompletion(const char *buf, linenoiseCompletions *lc)
{
 int  i,n ;
 int  sp;
 int  rc;
 int  flags = 0;
 char sbuf[255];
 char wildcard[255];
 char compbuf[255];
 char statbar[2048];
 char *space;
 glob_t results;

    statbar[0]='\0';

    space=NULL;          // default space to end of buffer
    space = rindex(buf, ' ');

    if(space==NULL){
        strcpy(sbuf, buf);
        strcpy(wildcard, buf);
    } else {
        sp=space-buf;
        strncpy(sbuf, buf,sp);
        sbuf[sp]='\0';
        strcpy(wildcard, space+1);
    }
    strcat(wildcard, "*");
    if(strcmp(wildcard, "*" ) == 0 ) return;
    sprintf(statbar, "<%s> ", wildcard);

// printf("\r\n b<%s> w<%s> \r\n", buf, wildcard);

    rc = glob(wildcard, flags, globerr, &results);

    if (rc != 0) {
        if(rc == GLOB_NOMATCH ){
            strcat(statbar, "no match");
            statusline(statbar);
            globfree(&results);
            return ;
        } else {
            perror("glob failed");
            exit(1);
        }
    }

     n = results.gl_pathc;

     for (i = 0; i < n; i++){
        strcat(statbar, results.gl_pathv[i]);
        strcat(statbar, " ");
         compbuf[0]='\0';
         if(space != NULL){
            strcpy(compbuf, sbuf);
            strcat(compbuf, " ");
         }
         strcat(compbuf, results.gl_pathv[i]);
         linenoiseAddCompletion(lc, compbuf);
     }

    statusline(statbar);
    globfree(& results);
}



char *filehints(const char *buf, int *color, int *bold)
{
UNUSED(buf);
UNUSED(color);
UNUSED(bold);

    return NULL;
}


int globerr(const char *path, int eerrno)
{
    fprintf(stderr, " %s: %s\n",  path, strerror(eerrno));
    return 0;   /* let glob() keep going */
}

// print statusline on top of screen
//  not using termcap/terminfo, doing it the lazy (hardcoded) way
void statusline(char line[])
{
 struct winsize win;
 char *buffer;
 char *pad;
 int  npad;
 int  i;

    ioctl(STDOUT_FILENO, TIOCGWINSZ, &win);

    buffer =  malloc(sizeof(char) * (win.ws_col+1));
    pad    =  malloc(sizeof(char) * (win.ws_col+1));

    if(strlen(line) >= win.ws_col) {            // line too long chop it off
        strncpy(buffer, line, win.ws_col-1);
        buffer[win.ws_col-1]='>';
        buffer[win.ws_col]='\0';
    } else {                    // build the padding string to fill the size of screen
        npad= win.ws_col - strlen(line);
        for(i=0; i<npad-1; i++) pad[i]='_';
        pad[npad-1]='|';
        pad[npad]='\0';
        sprintf(buffer, "%s%s", line, pad);
    }

    printf("\e7\e[0;0H\e[7m");  // save cursor, move 0:0, rev video on
    printf("%s", buffer);
    printf("\e[0m\e8");         // rev video off, restore cursor
    fflush(stdout);

 // Esc[Line;ColumnH 	Move cursor to screen location v,h 
 // Esc[7m 	Turn reverse video on 
 // Esc[0m 	Turn off character attributes 
 // Esc7 	Save cursor position and attributes 
 // Esc8 	Restore cursor position and attributes 
    free(buffer);
    free(pad);
}

