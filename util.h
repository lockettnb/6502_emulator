//
// 2018/01/12 
//


// UTILITY ROUTINES
// 
extern int sentinel;        // sigint sentinal MOVE TO util.h

// tty control
    int tty_raw(int fd);
    int tty_reset(int fd);

// signal control
    void sigint_grab(void);
    void sigint_restore(void);

// print help and instructions
    void inst(char *iptr[], int status);

// load the binary file into the memory buffer
    int load_file(FILE *fp, char *fname, int addr);

// trimwhite space from front/back of a command line
    char *trimwhite (char *string);
    void str_replace(char *target, const char *pattern, const char *replacement);

