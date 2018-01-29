//
// 2018/01/01 
//

#include "../hardware.h"

#define NOPARITY   0
#define EVENPARITY 1
#define ODDPARITY  2

#define  NO_TX_DATA 1024        // transmit data ready to send
                                // return large value as we only send bytes 0..255 

#define DISABLED  0
#define ENABLED   1

#define BITS(byt)  \
  (byt & 0x80 ? '1' : '0'), \
  (byt & 0x40 ? '1' : '0'), \
  (byt & 0x20 ? '1' : '0'), \
  (byt & 0x10 ? '1' : '0'), \
  (byt & 0x08 ? '1' : '0'), \
  (byt & 0x04 ? '1' : '0'), \
  (byt & 0x02 ? '1' : '0'), \
  (byt & 0x01 ? '1' : '0') 


void    console_init(void);
uint8_t console_read(uint16_t address);
void    console_write(uint16_t address, uint8_t data);
void    console_halt(void);

// void    console_status(void);
void  console_status(Acia *a6850);

// these routine provide the virtual RS232 serial interface for the console
//  + the external progarm (monitor.c in this case) is responsible for
//  + actually sending/recieiving of the characters
int    serial_in(char *c);
void   serial_out(char c);



