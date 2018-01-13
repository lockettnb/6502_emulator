//
//  2014/10/30 created
//  2018/01/03 update many times
// 

#define VERSION "0.42"
#define NULL_CHAR '\0'
#define TRUE -1
#define FALSE 0
#define SUCCESS 0
#define FAIL 1


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


//------------------------------------------------------------------
// EXTERNAL monitor VARIABLES USED BY THE EMULATOR
extern int help;
extern int version;
extern int verbose;

extern char *program_name;
extern char *instructions[];

//------------------------------------------------------------------
// EMULATOR DATA ELEMENTS
extern int mode;            // generic 6502 or OSI emulation

extern struct CPU_state {   // CPU state
	uint8_t		a;
	uint8_t		y;
	uint8_t		x;
	uint8_t     p;
	uint8_t     sp;
	uint16_t	pc;
} cpu;

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
void    emulation_init(int initmode);
void    mwrite(uint16_t address, uint8_t byte);
uint8_t mread(uint16_t address);
int     execute();

//------------------------------------------------------------------
// Op Codes Values
#define BRK     0x00
#define ORA_INX 0x01
#define ORA_ZP  0x05
#define ASL_ZP  0x06
#define PHP     0x08
#define ORA_IMM 0x09
#define ASL_ACC 0x0A
#define ORA_AB  0x0D
#define ASL_AB  0x0E

#define BPL_REL 0x10
#define ORA_INY 0x11
#define ORA_ZPX 0x15
#define ASL_ZPX 0x16
#define CLC     0x18
#define ORA_ABY 0x19
#define ORA_ABX 0x1D
#define ASL_ABX 0x1E

#define JSR_AB  0x20
#define AND_INX 0x21
#define BIT_ZP  0x24
#define AND_ZP  0x25
#define ROL_ZP  0x26
#define PLP     0x28
#define AND_IMM 0x29
#define ROL_ACC 0x2A
#define BIT_AB  0x2C
#define AND_AB  0x2D
#define ROL_AB  0x2E

#define BMI_REL 0x30
#define AND_INY 0x31
#define AND_ZPX 0x35
#define ROL_ZPX 0x36
#define SEC     0x38
#define AND_ABY 0x39
#define AND_ABX 0x3D
#define ROL_ABX 0x3E

#define RTI     0x40
#define EOR_INX 0x41
#define EOR_ZP  0x45
#define LSR_ZP  0x46
#define PHA     0x48
#define EOR_IMM 0x49
#define LSR_ACC 0x4A
#define JMP_AB  0x4C
#define EOR_AB  0x4D
#define LSR_AB  0x4E

#define BVC_REL 0x50
#define EOR_INY 0x51
#define EOR_ZPX 0x55
#define LSR_ZPX 0x56
#define CLI     0x58
#define EOR_ABY 0x59
#define EOR_ABX 0x5D
#define LSR_ABX 0x5E

#define RTS     0x60
#define ADC_INX 0x61
#define ADC_ZP  0x65
#define ROR_ZP  0x66
#define PLA     0x68
#define ADC_IMM 0x69
#define ROR_ACC 0x6A
#define JMP_IN  0x6C
#define ADC_AB  0x6D
#define ROR_AB  0x6E

#define BVS_REL 0x70
#define ADC_INY 0x71
#define ADC_ZPX 0x75
#define ROR_ZPX 0x76
#define SEI     0x78
#define ADC_ABY 0x79
#define ADC_ABX 0x7D
#define ROR_ABX 0x7E

#define STA_INX 0x81
#define STY_ZP  0x84
#define STA_ZP  0x85
#define STX_ZP  0x86
#define DEY     0x88
#define TXA     0x8A
#define STY_AB  0x8C
#define STA_AB  0x8D
#define STX_AB  0x8E

#define BCC_REL 0x90
#define STA_INY 0x91
#define STY_ZPX 0x94
#define STA_ZPX 0x95
#define STX_ZPY 0x96
#define TYA     0x98
#define STA_ABY 0x99
#define TXS     0x9A
#define STA_ABX 0x9D

#define LDY_IMM 0xA0
#define LDA_INX 0xA1
#define LDX_IMM 0xA2
#define LDY_ZP  0xA4
#define LDA_ZP  0xA5
#define LDX_ZP  0xA6
#define TAY     0xA8
#define LDA_IMM 0xA9
#define TAX     0xAA
#define LDY_AB  0xAC
#define LDA_AB  0xAD
#define LDX_AB  0xAE

#define BCS_REL 0xB0
#define LDA_INY 0xB1
#define LDY_ZPX 0xB4
#define LDA_ZPX 0xB5
#define LDX_ZPY 0xB6
#define CLV     0xB8
#define LDA_ABY 0xB9
#define TSX     0xBA
#define LDY_ABX 0xBC
#define LDA_ABX 0xBD
#define LDX_ABY 0xBE

#define CPY_IMM 0xC0
#define CMP_INX 0xC1
#define CPY_ZP  0xC4
#define CMP_ZP  0xC5
#define DEC_ZP  0xC6
#define INY     0xC8
#define CMP_IMM 0xC9
#define DEX     0xCA
#define CPY_AB  0xCC
#define CMP_AB  0xCD
#define DEC_AB  0xCE

#define BNE_REL 0xD0
#define CMP_INY 0xD1
#define CMP_ZPX 0xD5
#define DEC_ZPX 0xD6
#define CLD     0xD8
#define CMP_ABY 0xD9
#define CMP_ABX 0xDD
#define DEC_ABX 0xDE

#define CPX_IMM 0xE0
#define SBC_INX 0xE1
#define CPX_ZP  0xE4
#define SBC_ZP  0xE5
#define INC_ZP  0xE6
#define INX     0xE8
#define SBC_IMM 0xE9
#define NOP     0xEA
#define CPX_AB  0xEC
#define SBC_AB  0xED
#define INC_AB  0xEE

#define BEQ_REL 0xF0
#define SBC_INY 0xF1
#define SBC_ZPX 0xF5
#define INC_ZPX 0xF6
#define SED     0xF8
#define SBC_ABY 0xF9
#define SBC_ABX 0xFD
#define INC_ABX 0xFE

