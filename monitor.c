/*
*   2014/11/21 created
*   2017/12/19 getting back to finishing this
*
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>

#include <readline/readline.h>
#include <readline/history.h>
#include "e6502.h"
#include "util.h"

#define FILEMAX  256
#define HISTMAX  24 
#define BREAKMAX 12
#define NOBREAK  99999
#define TRACEMAX 26
#define TRACELEN 80


// Global Variables
char *program_name;
int  running=FALSE;                  // no program running or stepping 
int  breakpoints[BREAKMAX];
char trace_buffer[TRACEMAX][TRACELEN];
int  trace_head;
int  trace_tail;


/* Option Flags set by `--options' */
int verbose = FALSE;
int help    = FALSE;
int version = FALSE;
// int debug   = FALSE;

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int xdump (char *arg[])
{
int i,j;
int start_addr, end_addr;
char line[80];
char byte[8];
long nn;

    if (arg[0]) {
        start_addr = strtol(arg[0], NULL, 16) & 0x0fff0;
    } else {
        start_addr=cpu.pc & 0x0fff0;
    }

    if(arg[1]) {
        end_addr= strtol(arg[1], NULL, 16)  & 0x0fff0;
    } else {
        end_addr=start_addr+256;
    }
    if(end_addr <= start_addr) 
        end_addr=start_addr+256;

    if(end_addr > 0xffff) 
        end_addr = 0xffff;

    for(i=start_addr; i<=end_addr; i=i+16) {
        sprintf(line, " %04x ", i);
        for(j=0; j<16; j++) { 
            sprintf(byte, " %02x",mread(i+j));
            strcat(line, byte);
            if(j == 7)  strcat(line, " ");
        }
        strcat(line, " ");
        for(j=0; j<16; j++) { 
            if(mread(i+j) <= 32 || mread(i+j)>=127) {
                sprintf(byte, ".");
            } else { 
                sprintf(byte, "%c",mread(i+j));
            }
            if(j == 7)  strcat(line, " ");
            strcat(line, byte);
        }
    printf("%s\n", line);
    }
return(0);
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int xload(char *arg[])
{
FILE *fp;
int bytes;
int addr;

    // new program is considered "not" running
    running=FALSE;

    if((fp=fopen(arg[0], "r")) == NULL) {
        fprintf(stderr, "%s: error opening file <%s>\n", program_name, arg[0]);
        exit(FAIL);
    }

    if (arg[1] == NULL)
    {
        addr = 0x1000;
        cpu.pc= 0x1000;
    } else {
        addr = strtol(arg[1], NULL, 16);
    }
    bytes = load_file(fp, arg[0], addr);     //load file(0) at address

    fclose(fp);
    printf("%i bytes loaded at $%04x\n", bytes, addr);
return(0);
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int xregisters(char *arg[])
{
    printf(" PC   A  X  Y  SP  NVXB DIZC\n");
    printf(" %04x %02x %02x %02x %02x", cpu.pc, cpu.a, cpu.x, cpu.y,cpu.sp);
    printf("  %01x", GETFLAG(N_FLAG) );
    printf("%01x", GETFLAG( V_FLAG) );
    printf("%01x", GETFLAG( X_FLAG) );
    printf("%01x", GETFLAG( B_FLAG) );
    printf(" %01x", GETFLAG( D_FLAG) );
    printf("%01x", GETFLAG( I_FLAG) );
    printf("%01x", GETFLAG(Z_FLAG) );
    printf("%01x\n", GETFLAG( C_FLAG) );

return(0);
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int xhelp(char *arg[])
{
    printf(">>help\n");

int i = 0;
char *helptext[] = {
    " ",
    "dump    +display memory            -- dump <start addr> [end addr]",
    "mem     +display memory            -- mem  <start addr> [end addr]",
    "load    +load file into memory     -- load <file name> <address>",
    "clear   +clear cpu registers",
    "run     +run 6502 emulator         -- run <address>",
    "step    +execute next instruction",
    "reg     +display cpu registers",
    "set     +set cpu register          -- set pc <addr>",
    "set     +set memory location       -- set <addr> <data> [data]...[data]",
    "dis     +disassemble               -- dis <start addr> [end addr]",
    "break   +set breakpoint            -- break <addr>",
    "info    +display breakpoint info   -- info",
    "history +display command history",
    "quit    +exit program",
    "                                                     jal 2018/01",
    NULL_CHAR
    };

    while (helptext[i] != NULL_CHAR){
        printf("%s\n", helptext[i++]);
    }
return(0);
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int xdis(char *arg[])
{
 int i =0;
 int opsize = 0;
 int start_addr, num_bytes;
 char disline[255];


    // if no address give use the program counter at the start
    if (arg[0]==NULL || strcmp(arg[0],"pc")==0 ) {
        start_addr=cpu.pc & 0x0ffff;
    } else {
        start_addr = strtol(arg[0], NULL, 16) & 0x0fff0;
    } 

    // is no end given use one 
    if(arg[1]) {
        num_bytes = strtol(arg[1], NULL, 16);
    } else {
        num_bytes=40;
    }

    for(i=0; i<num_bytes; i=i+opsize) {
        opsize = disassemble(start_addr+i, disline);
        printf("%s\n", disline);
    }



return(0);
}

// **********************************************************************
void trace_clear(void)
{
 int i;

    for (i=0; i<TRACEMAX; i++) trace_buffer[i][0]='\0';
    trace_head=trace_tail=0;
}



void trace_append(char *newline)
{
// printf("append <%s>\n" , newline);

    strcpy(trace_buffer[trace_head++], newline);
    if(trace_head == TRACEMAX) trace_head = 0;
    if(trace_head == trace_tail) trace_tail++;
    if(trace_tail == TRACEMAX) trace_tail = 0;
}


void trace_print(int count)
{
 int i;

    for(i=trace_tail; i!=trace_head; ) {
//      printf("head=%i tail=%i %i \n", trace_head, trace_tail, i);
        if(trace_head == trace_tail) break;
        if(trace_buffer[i][0]!='\0') printf("%s\n", trace_buffer[i]);
        if(i++ == TRACEMAX-1) i=0;
    }
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int xtrace(char *arg[])
{
 int i =0;
 int count = 20;
 int tail, head;

    // if no address give use the program counter at the start
    if (arg[0]) {
        count = strtol(arg[0], NULL, 16);
    } 

    trace_print(count);

return(0);
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int xmode(char *arg[])
{
    printf(">>Current mode %s\n", mode?"GENERIC":"OSI");

    if(strcmp(arg[0], "osi") ==0) {
        emulation_init(OSI);
    } else {
        emulation_init(GENERIC);
    }
    printf(">>New mode %s\n", mode?"GENERIC":"OSI");

    arg[0]="cpu";
    xclear(arg);


return(0);
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int xquit(char *arg[])
{
    printf(">>quit\n");

return(-1);
}

void get_cpu_status(char result[])
{
 char flags[32];
 flags[0]='\0';

    sprintf(result, " A%02x X%02x Y%02x S%02x   %02x", cpu.a,cpu.x, cpu.y,cpu.sp,cpu.p);

// C_FLAG    0x01         // 1: Carry occured
// Z_FLAG    0x02         // 1: Result is zero
// I_FLAG    0x04         // 1: Interrupts disabled
// D_FLAG    0x08         // 1: Decimal mode 
// B_FLAG    0x10         // Break [0 on stk after int]
// X_FLAG    0x20         // Always 1 (undefined)
// V_FLAG    0x40         // 1: Overflow occured
// N_FLAG    0x80         // 1: Result is negative

    GETFLAG(N_FLAG)? strcpy(flags, "N") : strcpy(flags, "n");
    GETFLAG(V_FLAG)? strcat(flags, "V") : strcat(flags, "v");
    GETFLAG(X_FLAG)? strcat(flags, "X") : strcat(flags, "x");
    GETFLAG(B_FLAG)? strcat(flags, "B") : strcat(flags, "b");

    GETFLAG(D_FLAG)? strcat(flags, " D") : strcat(flags, " d");
    GETFLAG(I_FLAG)? strcat(flags, "I") : strcat(flags, "i");
    GETFLAG(Z_FLAG)? strcat(flags, "Z") : strcat(flags, "z");
    GETFLAG(C_FLAG)? strcat(flags, "C") : strcat(flags, "c");

    sprintf(result,"%s %s", result, flags);
}


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int xrun(char *arg[])
{
 int  i;
 int  tc=0;
 int  count=-1;           // number of instruction to execute (-1 forever)
 int  inst=0;             // number of instructions executed
 char regs[TRACELEN];
 int  breakpoint=FALSE;
 char traceline[TRACELEN];
 int  done=FALSE;
 int  rc;

    if(!running) {          
        trace_clear();
        running=TRUE;
    }

    if(arg[0] != NULL) {
        count = strtol(arg[0], NULL, 16);
    } 
    
    printf("run starting at 0x%04x\n", cpu.pc);
    sigint_grab();
    tty_raw(STDIN_FILENO);
    
    while(!done && !sentinel){
        disassemble(cpu.pc, traceline);
        rc=execute();
        get_cpu_status(regs);
        sprintf(traceline, "%-30s %30s", traceline, regs);
        trace_append(traceline);
        for(i=0; i<BREAKMAX; i++) {
            if(cpu.pc == breakpoints[i]) {
                printf(">> breakpoint at 0x%04x\n", breakpoints[i]);
                breakpoint = TRUE;
            }
        }
        if(inst++ == count-1) done = TRUE;
        if(rc != 0) done = TRUE;
        if(breakpoint) done = TRUE;
    }
    sigint_restore();
    sentinel = FALSE;
    tty_reset(STDIN_FILENO);
    printf("run complete at 0x%04x\n", cpu.pc);
//     xregisters(arg);

return(0);
}




// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int xstep(char *arg[])
{
char *addr="pc";
char *num="1";
int tc=0;
char traceline[TRACELEN];
char regs[TRACELEN];

    if(!running) {          
        trace_clear();
        running=TRUE;
    }

    arg[0]=addr;
    arg[1]=num;
//     printf(">>>");
//     xdis(arg);

    disassemble(cpu.pc, traceline);
    execute();
    get_cpu_status(regs);
    sprintf(traceline, "%-30s %30s", traceline, regs);
    trace_append(traceline);
//     xregisters(arg);
//     printf("<<");
//     xdis(arg);
    printf("%s\n", traceline);
return(0);
}



// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int xset(char *arg[])
{
 int i, addr;
    printf(">>set\n");

    // program counter
    if(strcmp(arg[0], "pc") == 0) {
        cpu.pc= strtol(arg[1], NULL, 16);
        return(0);
    }

    // set memory location
    addr = strtol(arg[0], NULL, 16);
    for(i=1; arg[i]; i++) {
        mwrite(addr-1+i,  strtol(arg[i], NULL, 16));
    }

return(0);
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int xbreak(char *arg[])
{
 int i, addr;

    // set memory location
    addr = strtol(arg[0], NULL, 16);
    for(i=0; i<=BREAKMAX; i++) {
        if(breakpoints[i] == NOBREAK) {
            breakpoints[i]=addr;
            break;
        }
    }
    printf(">>break %i $%04x\n", i, breakpoints[i]);

return(0);
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int xinfo(char *arg[])
{
 int i;
    printf(">>break information\n");

    // set memory location
    for(i=0; i<BREAKMAX; i++) {
       if(breakpoints[i] == NOBREAK) {
            printf("%i : no breakpoint\n", i );
       } else {
            printf("%i : 0x%04x\n", i, breakpoints[i]); 
       }
    }

return(0);
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int xclear(char *arg[])
{
int i;

    if(strcmp(arg[0],"cpu") == 0) {     // clear CPU state
        printf(">>clear\n");
        cpu.pc=mread(0xfffc) + (mread(0xfffd)<<8);
        cpu.sp=0xff;
        cpu.a=0;
        cpu.x=0;
        cpu.y=0;
        cpu.p=0;
        SETFLAG(X_FLAG, 1);
        xregisters(arg);
        return(0);
    } 

    if(strcmp(arg[0],"trace") == 0) {
        trace_clear();
        return(0);
    } 

    if(strcmp(arg[0], "breakpoints") ==0 ) {
        for (i=0; i<BREAKMAX; i++) breakpoints[i]=NOBREAK;
        return(0);
    } 

    if(arg[0] != NULL) {
        i = strtol(arg[0], NULL, 16);
        if((i>=0) && (i<BREAKMAX) && (breakpoints[i] != NOBREAK) ){
            printf("%i: 0x%04x ---> no breakpoint\n", i, breakpoints[i]);
            breakpoints[i] = NOBREAK;
        }
    }
return(0);

}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int xhistory(char *arg[])
{
 HIST_ENTRY **hlist;
 int i;

    printf(">>history\n");

    hlist = history_list ();
//    printf("length %i, offset xx, base %i\n", history_length, history_base); 
    if (hlist)
        for (i = 0; hlist[i]; i++)
             printf ("%2i: %s\n", i , hlist[i]->line);

return(0);
}



// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// monitor command list
struct cmd {
    char *name;             // name of command
    int (*f)(char **);     // command function
};

struct cmd cmdlist[] = {
    {"quit",    xquit  },
    {"q",       xquit  },
    {"dump",    xdump  },
    {"mem",     xdump  },
    {"load",    xload  },
    {"lo",      xload  },
    {"clear",   xclear },
    {"run",     xrun   },
    {"dis",     xdis   },
    {"step",    xstep  },
    {"reg",     xregisters },
    {"help",    xhelp},
    {"set",     xset   },
    {"history", xhistory  },
    {"his",     xhistory  },
    {"break",   xbreak},
    {"info",    xinfo  },
    {"trace",   xtrace},
    {"mode",   xmode},
    {'\0',      NULL  }
};

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
main (int argc, char *argv[])
{
 char *line, *trimline, *command;
 char *arg_list[64];
 int i,j; 
 int valid_cmd = FALSE;
 const char *home;
 char historyfile[FILEMAX];
 int done = FALSE;
 HIST_ENTRY *hlast;

  // initalize 6502 emulation
    emulation_init(GENERIC);

  // initalize breakpoint list and trace buffer to empty
    for (i=0; i<BREAKMAX; i++) breakpoints[i]=NOBREAK;
    trace_clear();
  // initalize command line histroy 
    using_history();
    stifle_history (HISTMAX);
    home = getenv("HOME");
    if(home != NULL) {
        sprintf(historyfile, "%s/.m6502history", home);
        printf("reading History File: %s\n", historyfile);
        if(read_history(historyfile)!=0) 
            add_history("hello");
    } else {
        printf("Environment HOME not set, no history file\n");
    }

  // loop reading and executing commands
    while (!done) {
        line = readline ("M65> ");
        if (line == NULL) { 
            fprintf(stderr, "Error: failed to read command line\n");
            exit(1);
        }

        trimline = trimwhite (line);

        if (*trimline) {    // add to history if not blank or duplicate
            // I still do not understand the history_base value
            // history_get code subtracts it from the offset
            hlast= history_get(history_length-1+history_base);
            if(hlast != NULL) {
                if(strcmp(hlast->line, trimline) !=0) {
                  add_history (trimline);
                }
            }
        }

        command = strtok(trimline," ");     // command should be the first thing
        if(command != NULL) {
         for(i=0; cmdlist[i].name; i++) {   // loop thru list look for command
            if(strcmp(command, cmdlist[i].name) == 0) {
                j=0;
                do {    // split line into an array of tokens
                    arg_list[j] = strtok(NULL," ");
                } while(arg_list[j++] != NULL);
                arg_list[j]=NULL;

                // dispatch command 
                done = cmdlist[i].f(arg_list);     // exec the command with args
                valid_cmd=TRUE;
                break;
            }
          }
        }
        if(! valid_cmd) printf(">> Error: unknown command \n");
        valid_cmd=FALSE;
    }

   // we are done clean up and save command line history
    if(home != NULL) {
        printf("writing history....%s\n", historyfile);
        write_history(historyfile);
    } else {
        printf("Environment HOME not set, no history file saved\n");
    }
    free (line);
} //main


