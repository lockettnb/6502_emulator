//
// 2018/01/19 
//

#include <stdio.h> 
#include <stdlib.h>
#include <dirent.h> 
#include <string.h> 
#include <errno.h>
#include <glob.h>
#include "linenoise.h"

int globerr(const char *path, int eerrno)
{
    fprintf(stderr, " %s: %s\n",  path, strerror(eerrno));
    return 0;   /* let glob() keep going */
}



void completion(const char *buf, linenoiseCompletions *lc)
{
 char wildcard[255];
 int i;  
int flags = 0;
glob_t results;
int ret;
fprintf(stderr, "<++>");

strcpy(wildcard, buf);
strcat(wildcard, "*");
printf("<%s> ", wildcard);

    ret = glob(wildcard, flags, globerr, &results);

    if (ret != 0) {
        if(ret == GLOB_NOMATCH ) printf("no match \n");
        } else {
            if (ret != 0) {
                fprintf(stderr, ">> problem with %s (%s), stopping early\n", buf,
                (ret == GLOB_ABORTED ? "filesystem problem" :
                 ret == GLOB_NOSPACE ? "no dynamic memory" :
                 "unknown problem"));
            exit(1);
            }
        }

//     printf("count %i\n", results.gl_pathc);
// 
     for (i = 0; i < results.gl_pathc; i++){
         linenoiseAddCompletion(lc, results.gl_pathv[i]);
     }

    globfree(& results);
}

char *hints(const char *buf, int *color, int *bold) {
    if (!strcasecmp(buf,"hello")) {
        *color = 35;
        *bold = 0;
        return " World";
    }
    return NULL;
}

int main(int argc, char **argv) 
{
 char *line;
 char *prgname = argv[0];


    linenoiseSetCompletionCallback(completion);
    linenoiseSetHintsCallback(hints);

    while((line = linenoise("hello> ")) != NULL) {
        /* Do something with the string. */
        if (line[0] != '\0' && line[0] != '/') {
            printf("echo: '%s'\n", line);
            linenoiseHistoryAdd(line); /* Add to the history. */
            linenoiseHistorySave("history.txt"); /* Save the history on disk. */
        } else if (!strncmp(line,"/historylen",11)) {
            /* The "/historylen" command will change the history len. */
            int len = atoi(line+11);
            linenoiseHistorySetMaxLen(len);
        } else if (line[0] == '/') {
            printf("Unreconized command: %s\n", line);
            exit(0);
        }
        free(line);
    }

  return(0);
}
