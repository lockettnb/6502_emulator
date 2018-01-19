//  
// 2014/10/25 created
// 2018/01/04 extensive update to add OSI mode
// 

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <getopt.h>
#include "emulator.h"
#include "opcodes.h"
#include "console.h"

// message logging, change to suit, default is to output message to stdin
// the ## tells cpp to remove the previous comas if only one argument is passed
#define LOG(fmt, ...)  printf(fmt, ##__VA_ARGS__ )

// do-while groups statements and avoid trailing semicolon problems
// define dbprintf(fmt, ...) do { if (debug) fprintf(stderr, fmt, ## __VA_ARGS__); } while (0)


// global variables
static struct CPU_state cpu;        // the 6502 registers
uint8_t memory[MEMORY_SIZE];        // array of bytes for RAM memory
struct device dev[DEVMAX];          // array of devices.... memory, console, floppy etc
int    mode=GENERIC;                // system emulation mode, generic or OSI


// OSI synmon ROM three pages (0xffd0, 0xffe0, -xfff0)
char rom[] = "200cfd4c18e02016fd4c18e0a9008dffefa9008dfeefd8a207a9002c80c2\
9d00c2ca10f7adffef8d00c2adfeef8d01c2a9102c80c28d02c2a9008d02\
c22098fda203bdabfd2c80c29d03c2ca10f42c80c2a9808d07c2ad07c230\
fbad12e04dffef30b3ad13e04dfeefd0aba91885fca9e085fda90e85fea9\
00aaa81871fc9004e8f00118c8d0f5e6fdc6fed0efcd18eed084ec19eed0\
f960ad02c2c9d9f00b29c4c9c4d0f368684c16fd60130025072016fd6cfc\
fe2424242424242424242424242424242424242424242424242424242424\
242424242424242424242424242424242424242424242424242424242424\
24242424242424242424242424242424ad00fc4a90faad01fc297f48ad00\
fc4a4a90f9688d01fc602000fec952f016c93030f5c93a300bc94130edc9\
4710e918e906290f60a9038d00fca9b18d00fcd878a2269aa90d200bfea9\
0a200bfe2000fec94cf022c950f034c947d0d8ae2d019aae2a01ac2901ad\
2e0148ad2f0148ad2c0148ad2b014020c7fea203a00020b5fea5ff91fcc8\
d0f6e6fdb850f120c7fea000a209a90d200bfea90a200bfecaf00b20e0fe\
c8d0f7e6fd4c9efead00fc4ab08eea90dd2018fe0a0a0a0a95fc2018fe18\
75fc95fc60a20120b5feca20b5fe60186930c93ab004200bfe60690690f8\
b1fc29f04a4a4a4a20d1feb1fc290f20d1fea920200bfe60409d300135fe\
c001a0008c01c08c00c0a2048e01c08c03c0888c02c08e03c08c02c0a9fb\
d009a9022c00c0f01ca9ff8d02c02099ff29f78d02c02099ff09088d02c0\
a2182085fff0dda27f8e02c02085ffad00c030fbad00c010fba9038d10c0\
a9588d10c02090ff85feaa2090ff85fd2090ff85ffa0002090ff91fdc8d0\
f8e6fec6ffd0f286fea9ff8d02c060a0f888d0fd55ffcad0f660ad10c04a\
90faad11c060482f442f4d3fd8a2d8a9d085fea00084fda92091fdc8d0fb\
e6fee4fed0f5a9038d00fca9b18d00fcb99aff300e99c6d0ae01fed00320\
0bfec8d0edad01fed0052000feb00320edfec948f00ac944d00c2000ff4c\
00224c00fd2000ff6cfcfeea3001a0ffc001";


//******************************************************************
//
//  MEMORY 64k and 48k Routines
//
void mem64k_init(void)
{
 int i;
    for (i=0; i<MEMORY_SIZE; i++) memory[i]=0;
    memory[0xfffc]=0x00; //reset vector $1000
    memory[0xfffd]=0x10;
}


void mem48k_init(void)
{
 int i;
   for (i=0; i<OSI_MEMORY_SIZE; i++) memory[i]=0;
   for (i=OSI_MEMORY_SIZE; i<MEMORY_SIZE; i++) memory[i]=0xea;
   // 0xEA just happens to be a NOP
}

uint8_t memory_read(uint16_t address)
{
    return memory[address];
}

void memory_write(uint16_t address, uint8_t byte)
{
    memory[address] = byte;
}

void memory_halt(void)
{
    LOG("RAM memory shutting down....\n");
}


//******************************************************************
//
//  ROM (synmon) Routines
//
void rom_init(void)
{
 int i;
 char hex[4];
 int romlen;
 uint8_t byte;
 uint16_t addr = 0xfd00 ;

    romlen=strlen(rom);
    for (i=0; i<romlen; i=i+2) {
        hex[0]=rom[i];
        hex[1]=rom[i+1];
        hex[2]='\0';
        byte=strtol(hex, NULL, 16);
        memory[addr++] = byte;
    }
}

void rom_halt(void) {
    LOG("synmon ROM shutting down\n");
}


//******************************************************************
//
//  floppy device Routines
//
void floppy_init(void) {
    LOG("floppy init\n");
}
uint8_t floppy_read(uint16_t address) {
    LOG("floppy read\n");
    return (0);
}
void floppy_write(uint16_t address, uint8_t byte) {
    LOG("floppy write\n");
}
void floppy_halt(void) {
    LOG("floppy halt\n");
}



//******************************************************************
// initalize the memory map to match generic or osi system emulation
void init_memory_map(void)
{
 int i;
 int romlen;
 uint8_t byte;

  // create empty device/memory map
    for(i=0; i<DEVMAX; i++) {
        dev[i].start=DEVEND;
        dev[i].end=DEVEND;
        dev[i].type=NA;
        dev[i].init = NULL;
        dev[i].read = NULL;
        dev[i].write = NULL;
        dev[i].halt = NULL;
    }

  // generic system with 64k of memory
    if(mode == GENERIC) {
        dev[0].start=0;
        dev[0].end=0xffff;
        dev[0].type=RW;
        dev[0].init = mem64k_init;
        dev[0].read = memory_read;
        dev[0].write = memory_write;
        dev[0].halt = memory_halt;
        
    }

  // OSI Challenger3 8P -- 6502, 48k memory, serial console, 8" floppy controller
    if(mode==OSI) {
        dev[0].start=0x0000;       // 48k r/w memory
        dev[0].end = 0xbfff;
        dev[0].type=RW;
        dev[0].init = mem48k_init;
        dev[0].read = memory_read;
        dev[0].write = memory_write;
        dev[0].halt = memory_halt;

        dev[1].start=0xc000;       // floppy controller PIA & ACIA
        dev[1].end = 0xc011;
        dev[1].type=IO;
        dev[1].init = floppy_init;
        dev[1].read = floppy_read;
        dev[1].write = floppy_write;
        dev[1].halt = floppy_halt;
        
        dev[2].start=0xFC00;       // serial console ACIA
        dev[2].end = 0xFc01;
        dev[2].type=IO;
        dev[2].init = console_init;
        dev[2].read = console_read;
        dev[2].write = console_write;
        dev[2].halt = console_halt;
        
        dev[3].start=0xfd00;       // synmon monitor/boot ROM
        dev[3].end=0xffff;
        dev[3].type=RO;
        dev[3].init = rom_init;
        dev[3].read = memory_read;
        dev[3].write = memory_write;
        dev[3].halt = rom_halt;
    }
}



// ******************************************************************
// get copy of all cpu registers
void get_registers(struct CPU_state regs)
{
  regs.pc=cpu.pc;
  regs.sp=cpu.sp;
  regs.a=cpu.a;
  regs.x=cpu.x;
  regs.y=cpu.y;
  regs.p=cpu.p;
}

uint8_t get_reg(Register reg)
{


}

void    set_reg(Register reg, int value)
{

    switch (reg) {
     case PC:
        cpu.pc = value;
      break;
     case SP:
        cpu.sp = value;
      break;
    case A:
        cpu.a = value;    
      break;
    case X:
        cpu.x = value;
      break;
     case Y:
        cpu.y = value;
      break;
     case P:
        cpu.p = value;
      break;
    }
}


// ******************************************************************
// fix this to read console and floppy status not cpu
//
void get_device(void *p, device_t x)
{
  ((struct CPU_state*)p)->pc=cpu.pc;
  ((struct CPU_state*)p)->sp=cpu.sp;
  ((struct CPU_state*)p)->a=cpu.a;
  ((struct CPU_state*)p)->x=cpu.x;
  ((struct CPU_state*)p)->y=cpu.y;
  ((struct CPU_state*)p)->p=cpu.p;
}

// ******************************************************************
// get/set flags
//
int get_flag(int f )
{
    
    return GETFLAG(f);
}

void set_flag(int f, int value)
{

    SETFLAG(f, value);
}

// ******************************************************************
// emulation initialization 
//
//  +sets mode to generic of osi
//  +setup memory map to match mode
void em_init(int initmode)
{
 int i;
    mode=initmode;
    init_memory_map();

  // initalize the devices/memory 
    for(i=0; i<DEVMAX; i++) {
        if(dev[i].start == DEVEND) break;
        LOG("init device %i\n", i);
        dev[i].init();
    }

  // initalize cpu 
    cpu.pc=mread(0xfffc) + (mread(0xfffd)<<8);
    cpu.sp=0xff; cpu.a=0;
    cpu.x=0;  cpu.y=0; cpu.p=0;
    SETFLAG(X_FLAG, 1);
}

// ******************************************************************
// emulation RESET 
//  +set program counter PC to reset vector
//  +set cpu A,X, Y registers to zero 
//  +set stackh to $01FF
//  +set flags to zero except the X flag
//
//  +sets mode to generic of osi
void em_reset(void)
{
        cpu.pc=mread(0xfffc) + (mread(0xfffd)<<8);
        cpu.sp=0xff;
        cpu.a=0;
        cpu.x=0;
        cpu.y=0;
        cpu.p=0;
        SETFLAG(X_FLAG, 1);
}


// ******************************************************************
// memory read, lookup devie at address and call device read function
uint8_t mread(uint16_t address)
{
 int i;

  // look thru the device memory map to find IO device or memory
    for(i=0; i<DEVMAX; i++) {
        if(address >= dev[i].start && address <= dev[i].end) break;
    }

    if(i < DEVMAX)
        return dev[i].read(address);
    else{
        LOG("Read from invalid address %04x\n", address);
        return(0);
    }
}


// ******************************************************************
// memory write, lookup devie at address and call device write function
void mwrite(uint16_t address, uint8_t byte)
{
 int i;

  // look thru the device memory map to find IO device or memory
    for(i=0; i<DEVMAX; i++) {
        if(address >= dev[i].start && address <= dev[i].end) break;
    }

    if(i < DEVMAX)
        dev[i].write(address, byte);
    else
        LOG("Write to invalid address %04x\n", address);
}


// ******************************************************************
// invalid instructions, we print error and skip over it
void invalid_instruction(struct CPU_state *cpu)
{
    LOG("Error: Invalid Instruction at PC=$%04x\n", cpu->pc);
    cpu->pc++;
}



// ******************************************************************
// .... finally here is the 6502 magic
//  + execute one instruction
//  + return the size of the opcode/instruction
int em_exec(void)
{
 int rc=0;               // return codes 0=instruction ok, 1=break
 int cycles = 4;
 uint16_t addr, zpaddr; 
 int8_t   branch_offset;
 uint8_t  db;
 uint16_t dw;
 uint8_t  opcode;

    opcode = mread(cpu.pc);

    switch (opcode) {
        case 0x00:      // BRK not implemented, skip over this op
            cpu.pc+=1;
            rc=1;
            break;
        case 0xea:      // NOP 
            cpu.pc+=1;
            break;

        #include "bits.h"
        #include "branch.h"
        #include "compare.h"
        #include "logical.h"
        #include "math.h"
        #include "move.h"
        #include "shift.h"
        #include "stack.h"
        #include "invalid.h"
    }
    return rc;
}

