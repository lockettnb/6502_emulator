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
#include <getopt.h>

#include <readline/readline.h>
#include <readline/history.h>
#include "emulator.h"
#include "utils.h"
#include "console.h"


#define FILEMAX  256
#define HISTMAX  24 
#define BREAKMAX 12
#define NOBREAK  99999
#define TRACEMAX 26
#define TRACELEN 80
#define  MAXLINE    256


// Global Variables
char *program_name;
int  running=FALSE;                  // no program running or stepping 
int  breakpoints[BREAKMAX];
char trace_buffer[TRACEMAX][TRACELEN];
int  trace_head;
int  trace_tail;

struct    CPU_state em_cpu;         // 6502 CPU registers
Acia      em_console;     // console 6850 ACIA device
device_t  em_device;      // emunumeration of devices


/* Option Flags set by `--options' */
int verbose = FALSE;
int help    = FALSE;
int version = FALSE;
int debug   = FALSE;

struct option long_options[] =
     {
       {"help",      no_argument, &help,    TRUE},
       {"version",   no_argument, &version, TRUE},
       {"verbose",   no_argument, &verbose, TRUE},
       {"debug",     no_argument, &debug,  TRUE},
       {"config",    required_argument, 0,  'f'},
       {NULL, 0, NULL, 0}
     };

char *instructions[] = {
    " fix file names: remove spaces and non-alphanumber characters",
    " ",
    " ",
    "                                                     john 2017/12",
    NULL_CHAR
    };

// Function Prototypes for stuff at the file of the file
int dispatch(char *command, int *done_flag);
void trace_clear(void);
void trace_append(char *newline);
void trace_print(int count);
void register_status(char result[]);
void display_console_status(Acia console);


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int xdump (char *arg[])
{
int i,j;
int start_addr, end_addr;
char line[80];
char byte[8];
long nn;

    em_device=cpu_registers;
    get_device((void *)&em_cpu, em_device); 

    if (arg[0]) {
        start_addr = strtol(arg[0], NULL, 16) & 0x0fff0;
    } else {
        start_addr=em_cpu.pc & 0x0fff0;
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
//         em_cpu.pc= 0x1000;
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

    em_device=cpu_registers;
    get_device((void *)&em_cpu, em_device); 

    printf(" PC   A  X  Y  SP  NVXB DIZC\n");
    printf(" %04x %02x %02x %02x %02x", em_cpu.pc, em_cpu.a, em_cpu.x, em_cpu.y,em_cpu.sp);
    printf("  %01x", get_flag(N_FLAG) );
    printf("%01x", get_flag( V_FLAG) );
    printf("%01x", get_flag( X_FLAG) );
    printf("%01x", get_flag( B_FLAG) );
    printf(" %01x", get_flag( D_FLAG) );
    printf("%01x", get_flag( I_FLAG) );
    printf("%01x", get_flag(Z_FLAG) );
    printf("%01x\n", get_flag( C_FLAG) );

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

    em_device=cpu_registers;
    get_device((void *)&em_cpu, em_device); 

    // if no address give use the program counter at the start
    if (arg[0]==NULL || strcmp(arg[0],"pc")==0 ) {
        start_addr=em_cpu.pc & 0x0ffff;
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
        em_init(OSI);
    } else {
        em_init(GENERIC);
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
char ch = 0;

    if(!running) {          
        trace_clear();
        running=TRUE;
    }

    if(arg[0] != NULL) {
        count = strtol(arg[0], NULL, 16);
    } 
    
    em_device=cpu_registers;
    get_device((void *)&em_cpu, em_device); 

    printf("run starting at 0x%04x\n", em_cpu.pc);
    sigint_grab();
    tty_raw(STDIN_FILENO);
    
    while(!done && !sentinel){
        disassemble(em_cpu.pc, traceline);
        rc=em_exec();
        register_status(regs);
        sprintf(traceline, "%-30s %30s", traceline, regs);
        trace_append(traceline);
        for(i=0; i<BREAKMAX; i++) {
            if(em_cpu.pc == breakpoints[i]) {
                printf(">> breakpoint at 0x%04x\n", breakpoints[i]);
                breakpoint = TRUE;
            }
        }
        if(inst++ == count-1) done = TRUE;
        if(rc != 0) done = TRUE;
        if(breakpoint) done = TRUE;

    rc = read(STDIN_FILENO, &ch, 1);
    if (rc == 1) {
      printf("char 0x%02x \n", ch);
      serial_in(ch);
    }

    rc = serial_out();
    if(rc != NO_TX_DATA)
        printf("%c", rc);
  }
    sigint_restore();
    sentinel = FALSE;
    tty_reset(STDIN_FILENO);
    printf("run complete at 0x%04x\n", em_cpu.pc);
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
int rc;

    if(!running) {          
        trace_clear();
        running=TRUE;
    }

    arg[0]=addr;
    arg[1]=num;

    em_device=cpu_registers;
    get_device((void *)&em_cpu, em_device); 

    disassemble(em_cpu.pc, traceline);
    em_exec();
    register_status(regs);
    sprintf(traceline, "%-30s %30s", traceline, regs);
    trace_append(traceline);
//     xregisters(arg);
//     printf("<<");
//     xdis(arg);
    printf("%s\n", traceline);
    rc = serial_out();
    if(rc != NO_TX_DATA){
        printf("%c", rc);
  }
return(0);
}



// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int xset(char *arg[])
{
 int i, addr;
    printf(">>set\n");

    // program counter
    if(strcmp(arg[0], "pc") == 0) {
// shit
        em_cpu.pc= strtol(arg[1], NULL, 16);
        set_reg(PC, em_cpu.pc);
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
 Acia console;

    if(strcmp(arg[0],"break") == 0) {
        printf(">>break information\n");
        // set memory location
        for(i=0; i<BREAKMAX; i++) {
           if(breakpoints[i] == NOBREAK) {
                printf("%i : no breakpoint\n", i );
           } else {
                printf("%i : 0x%04x\n", i, breakpoints[i]); 
           }
        }
    }

    if(strcmp(arg[0],"console") == 0) {
        console_status(&console);
        display_console_status(console);
    }

    if(strcmp(arg[0],"floppy") == 0) {

    }
    return(0);
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int xclear(char *arg[])
{
int i;

    if(strcmp(arg[0],"cpu") == 0) {     // clear CPU state
        printf(">>clear\n");
//shit
//         em_cpu.pc=mread(0xfffc) + (mread(0xfffd)<<8);
//         em_cpu.sp=0xff;
//         em_cpu.a=0;
//         em_cpu.x=0;
//         em_cpu.y=0;
//         em_cpu.p=0;
//         SETFLAG(X_FLAG, 1);
        em_device=cpu_registers;
        get_device((void *)&em_cpu, em_device); 
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
    {"breakpoints",    xinfo  },
    {"bp",      xinfo  },
    {"info",      xinfo  },
    {"trace",   xtrace},
    {"mode",   xmode},
    {'\0',      NULL  }
};

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
main (int argc, char *argv[])
{
 int optc, option_index;    // used to process options
 char *line, *trimline, *command;
 int i,j;
 char rcfile[32];
 int valid_cmd = FALSE;
 const char *home;
 char historyfile[FILEMAX];
 int done = FALSE;
 HIST_ENTRY *hlast;
 FILE *fp;
 char linebuf[MAXLINE];

    program_name=argv[0];
    rcfile[0]='\0';

    while ((optc=getopt_long(argc, argv, "f:", long_options, &option_index)) != -1) {
    switch (optc) {
        case 0:
            // getopt set one of the boolean options, just keep going
            break;
        case 'f': 
            printf ("option -f with value `%s'\n", optarg);
            strcpy(rcfile, optarg);
            break;
        case '?':
            // getopt_long already printed an error message
            display_help(instructions, 1);
            break;

        default:
            fprintf(stderr,"Invalid option, probably a filename starting with dash.\n");
            fprintf(stderr,"       Try, m6502 [options] -- {file list} \n");
            abort ();
        }
    } // while options

    if(help){
        display_help(instructions,0);
        exit(0);
    }

  // initalize 6502 emulation
    em_init(OSI);

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

 // if rc/config file process commands
    if(rcfile[0] != '\0') {
        printf(">>Config/Run Command File %s\n", rcfile);
        if((fp=fopen(rcfile, "r")) == NULL) {
            fprintf(stderr, "%s: error opening file <%s>\n", \
                    program_name, rcfile);
            exit(FAIL);
        }
        while (fgets(linebuf, MAXLINE, fp) != NULL) {
            printf("%s", linebuf);
            trimline = trimwhite (linebuf);
            command = strtok(trimline," ");
            if(command != NULL) 
                valid_cmd = dispatch(command, &done);
            if(!valid_cmd)
                printf("invalid command <%s>\n", command);
        }

        fclose(fp);

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
            valid_cmd = dispatch(command, &done);
        } else {
            // if the line is blank, repeat last command
            hlast = history_get(history_length-1+history_base);
            if(hlast != NULL)
               valid_cmd=dispatch(hlast->line, &done);
        }

        if(! valid_cmd) printf(">> Error: unknown command \n");
        valid_cmd=FALSE;

    } //while not done

   // we are done clean up and save command line history
    if(home != NULL) {
        printf("writing history....%s\n", historyfile);
        write_history(historyfile);
    } else {
        printf("Environment HOME not set, no history file saved\n");
    }
    free (line);
} //main



// **********************************************************************
int dispatch(char *command, int *done_flag)
{
 char *arg_list[32];
 int valid=FALSE;
 int i,j;

// function is a weird mix of variables passed in and some passed via 
// an implied strtok, you should really fix this mess

     for(i=0; cmdlist[i].name; i++) {   // loop thru list look for command
        if(strcmp(command, cmdlist[i].name) == 0) {
            j=0;
            do {    // split line into an array of argment tokens
                arg_list[j] = strtok(NULL," ");
            } while(arg_list[j++] != NULL);
            arg_list[j]=NULL;

            // dispatch command 
            *done_flag = cmdlist[i].f(arg_list);     // exec the command with args
            valid=TRUE;
            break;
        }
    }

    return valid;
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

    printf("%32s PC  A  X  Y  SP   Flags\n", " ");

    for(i=trace_tail; i!=trace_head; ) {
//      printf("head=%i tail=%i %i \n", trace_head, trace_tail, i);
        if(trace_head == trace_tail) break;
        if(trace_buffer[i][0]!='\0') printf("%s\n", trace_buffer[i]);
        if(i++ == TRACEMAX-1) i=0;
    }
}

void register_status(char result[])
{
 char flags[32];
 flags[0]='\0';

    em_device=cpu_registers;
    get_device((void *)&em_cpu, em_device); 

    sprintf(result, " %04x %02x %02x %02x %02x   %02x", em_cpu.pc, em_cpu.a,em_cpu.x, em_cpu.y,em_cpu.sp,em_cpu.p);
    get_flag(N_FLAG)? strcpy(flags, "N") : strcpy(flags, "n");
    get_flag(V_FLAG)? strcat(flags, "V") : strcat(flags, "v");
    get_flag(X_FLAG)? strcat(flags, "X") : strcat(flags, "x");
    get_flag(B_FLAG)? strcat(flags, "B") : strcat(flags, "b");

    get_flag(D_FLAG)? strcat(flags, " D") : strcat(flags, " d");
    get_flag(I_FLAG)? strcat(flags, "I") : strcat(flags, "i");
    get_flag(Z_FLAG)? strcat(flags, "Z") : strcat(flags, "z");
    get_flag(C_FLAG)? strcat(flags, "C") : strcat(flags, "c");

    sprintf(result,"%s %s", result, flags);
}


void display_console_status(Acia console)
{
 char bits[32];

      printf("___________________________________________________________________\n");
        printf("CONSOLE STATUS\n");
            printf(" Receive Buffer  %i 0x%02x\n", console.rx, console.rx);
            printf(" Transmit Buffer %i 0x%02x\n", console.tx, console.tx);

        sprintf(bits,"%c %c %c %c %c %c %c %c", BITS(console.cr));
        printf("                              7 6 5 4 3 2 1 0\n");
        printf("CONTROL REGISTER 0x%02x         %s\n", console.cr, bits);

        if((console.cr & 0x80) == 0)
            printf("interrupts disabled-----------' | | | | | | |\n");
        else
            printf("rx interrupts enabled---------' | | | | | | |\n");

        switch ((console.cr & 0x60)>>5) {
            case 0:
                printf("rts low, tx int disable --------+-+ | | | | |\n");
                break;
            case 1:
                printf("rts low, tx int enable ---------+-+ | | | | |\n");
                break;
            case 2:
                printf("rts high, tx int disable -------+-+ | | | | |\n");
                break;
            case 3:
                printf("rts low, tx int disable, break--+-+ | | | | |\n");
                break;
        }

        switch ((console.cr & 0x1C)>>2) {
            case 0:
                printf("7 bit, even, 2 stop bit ------------+-+-+ | |\n");
                break;
            case 1:
                printf("7 bit, odd, 2 stop bit -------------+-+-+ | |\n");
                break;
            case 2:
                printf("7 bit, even, 1 stop bit ------------+-+-+ | |\n");
                break;
            case 3:
                printf("7 bit, odd, 1 stop bit -------------+-+-+ | |\n");
                break;
            case 4:
                printf("8 bit, 2 stop bit ------------------+-+-+ | |\n");
                break;
            case 5:
                printf("8 bit, 1 stop bit ------------------+-+-+ | |\n");
                break;
            case 6:
                printf("8 bit, even, 1 stop bit ------------+-+-+ | |\n");
                break;
            case 7:
                printf("8 bit, odd, 1 stop bit -------------+-+-+ | |\n");
                break;
        }
        switch (console.cr & 0x03) {
            case 0:
                printf("normal -----------------------------------+-+\n");
                break;
            case 1:
                printf("div by 16 --------------------------------+-+\n");
                break;
            case 3:
                printf("div by 64 --------------------------------+-+\n");
                break;
            case 4:
                printf("master reset -----------------------------+-+\n");
                break;
        }
        printf("|rx_irq:%i", console.rx_irq );
        printf("|tx_irq:%i", console.tx_irq );
        printf("|rts:%i", console.rts );
        printf("|bits:%i", console.bits);
        if(console.parity==EVENPARITY) printf("|parity:even");
        if(console.parity==ODDPARITY) printf("|parity:odd");
        if(console.parity==NOPARITY) printf("|parity:no");
        printf("|stop:%i", console.stop);
        printf("|baud_clock:%i|\n", console.baud);

      // status register
        sprintf(bits,"%c %c %c %c %c %c %c %c", BITS(console.sr));
        printf("\n                              7 6 5 4 3 2 1 0\n");
        printf("STATUS REGISTER 0x%02x          %s\n", console.sr, bits);
        printf("Interrupt request ------------' | | | | | | |\n");
        printf("Parity error -------------------' | | | | | |\n");
        printf("Rx overrun -----------------------' | | | | |\n");
        printf("Framing error ----------------------' | | | |\n");
        printf("CTS ----------------------------------' | | |\n");
        printf("DCD ------------------------------------' | |\n");
        printf("Tx data register empty -------------------' |\n");
        printf("Rx data register full ----------------------'\n");
        printf("|irq:%i", console.int_request);
        printf("|parity:%i", console.parity_error);
        printf("|overrun:%i", console.overrun_error);
        printf("|framing:%i", console.framing_error);
        printf("|cts:%i", console.cts);
        printf("|dcd:%i", console.dcd);
        printf("|tx:%i", console.tx_empty);
        printf("|rx:%i|\n", console.rx_full);
      printf("___________________________________________________________________\n\n");
}

