//
// 2018/01/20 
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <glob.h>

char *myname;

/* globerr --- print error message for glob() */

int globerr(const char *path, int eerrno)
{
    fprintf(stderr, "%s: %s: %s\n", myname, path, strerror(eerrno));
    return 0;   /* let glob() keep going */
}

/* main() --- expand command-line wildcards and print results */

int main(int argc, char **argv)
{
    int i;
    int flags = 0;
    glob_t results;
    int ret;

    if (argc == 1) {
        fprintf(stderr, "usage: %s wildcard ...\n", argv[0]);
        exit(1);
    }

    myname = argv[0];   /* for globerr() */

    ret = glob(argv[1], flags, globerr, &results);

    if (ret != 0) {
        if(ret == GLOB_NOMATCH ) printf("no match \n");
        } else {
            if (ret != 0) {
                fprintf(stderr, "%s: problem with %s (%s), stopping early\n",
                myname, argv[i],
                (ret == GLOB_ABORTED ? "filesystem problem" :
                 ret == GLOB_NOSPACE ? "no dynamic memory" :
                 "unknown problem"));
            exit(1);
            }
        }

//     printf("count %i\n", results.gl_pathc);
// 
//     for (i = 0; i < results.gl_pathc; i++)
//         printf("%s\n", results.gl_pathv[i]);

    globfree(& results);
    return 0;
}
