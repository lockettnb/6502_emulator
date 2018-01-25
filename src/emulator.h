//
//  2014/10/30 created
//  2018/01/03 update many times
//

#define VERSION "0.42"

// Emulator Memory Map definitions and structures
#define MEMORY_SIZE 65536
#define OSI_MEMORY_SIZE 49152

#define STACK 0x0100    // 6502 stack page
#define DEVMAX 32       // number of device/memory segments we will support 
#define DEVEND 99999    // end of device map sentinal value
#define RW 0            // read-write memory
#define RO 1            // read-only memory
#define WO 2            // mwrite-only memory
#define IO 3            // IO hardware address
#define NA 4            // nothing at this memory (not available)

#define GENERIC 1      // generic mode, 6502 with 64k r/w memory
#define OSI 0          // Ohio Scientific mode 48K memory, floppy, serial I/O, rom 

// 6502 status flags: 
#define C_FLAG    0x01         // 1: Carry occured
#define Z_FLAG    0x02         // 1: Result is zero
#define I_FLAG    0x04         // 1: Interrupts disabled
#define D_FLAG    0x08         // 1: Decimal mode 
#define B_FLAG    0x10         // Break [0 on stk after int]
#define X_FLAG    0x20         // Always 1 (undefined)
#define V_FLAG    0x40         // 1: Overflow occured
#define N_FLAG    0x80         // 1: Result is negative

// setflag -- if v is true set flag else reset flag
#define SETFLAG(f,v) if(v) {cpu.p |= f;} else {cpu.p &= ~f; }

//getflag -- return true or false
#define GETFLAG(f) ((cpu.p & f)>0) 

// address mode calculations
// each of these marcos return an address
#define IMM_DATA     cpu.pc+1
#define AB(offset)   offset + mread(cpu.pc+1) + (mread(cpu.pc+2)<<8)
#define IN           mread(cpu.pc+1) + (mread(cpu.pc+2)<<8)
#define ZP(offset)   (offset + mread(cpu.pc+1)) & 0x00ff
#define IDX          mread(ZP(x)) + mread(ZP(x)+1) <<8
#define IDY          cpu.y + mread(ZP(0)+(mread(ZP(0)+1)<<8)) 

typedef enum { 
    cpu_registers,
    console_device,
    floppy_device
} device_t;

//------------------------------------------------------------------
// EXTERNAL monitor VARIABLES USED BY THE EMULATOR
extern int help;
extern int version;
extern int verbose;
extern char *program_name;
// extern char *instructions[];

//------------------------------------------------------------------
// EMULATOR DATA ELEMENTS
extern int mode;            // generic 6502 or OSI emulation
typedef enum { A, X, Y, P, SP, PC } Register;

struct CPU_state {   // CPU state
    uint8_t     a;
    uint8_t     y;
    uint8_t     x;
    uint8_t     p;
    uint8_t     sp;
    uint16_t    pc;
};

extern struct device {    // IO device or memory 
    int     start;
    int     end;
    int     type;
    void    (*init)(void);
    uint8_t (*read)(uint16_t addr);
    void    (*write)(uint16_t addr, uint8_t db);
    void    (*halt)(void);
} dev[DEVMAX];


//------------------------------------------------------------------
// emulator public interface
void    em_init(int initmode);
int     em_exec(void);
void    em_reset(void);

void    mwrite(uint16_t address, uint8_t byte);
uint8_t mread(uint16_t address);

void    get_registers(struct CPU_state regs);
uint8_t get_reg(Register reg);
int     get_flag(int f); 
void    set_reg(Register reg, int value);
void    set_flag(int f, int value);

void    get_device(void *p, device_t x);
void    reset_cpu(void);


