//
// 2018/01/01 
//

#define NOPARITY   0
#define EVENPARITY 1
#define ODDPARITY  2

#define  NO_TX_DATA 1024        // transmit data ready to send
                                // return large value as we only send bytes 0..255 

#define DISABLED    0
#define ENABLED    1
#define FALSE 0
#define TRUE -1

#define BITS(byt)  \
  (byt & 0x80 ? '1' : '0'), \
  (byt & 0x40 ? '1' : '0'), \
  (byt & 0x20 ? '1' : '0'), \
  (byt & 0x10 ? '1' : '0'), \
  (byt & 0x08 ? '1' : '0'), \
  (byt & 0x04 ? '1' : '0'), \
  (byt & 0x02 ? '1' : '0'), \
  (byt & 0x01 ? '1' : '0') 

// 6850 ACIA 
struct Acia {
    uint8_t cr;             // control register
    uint8_t sr;             // status register
    uint8_t rx;             // receive data register
    uint8_t tx;             // transmit data register
    uint8_t rxshift;        // receive data shift register
    uint8_t txshift;        // transmitt data shift register

    uint8_t rx_irq;         // control register bits
    uint8_t tx_irq;
    uint8_t rts;
    uint8_t bits;
    uint8_t parity;
    uint8_t stop;
    uint8_t baud;

    uint8_t int_request;      // status register bits
    uint8_t parity_error;
    uint8_t overrun_error;
    uint8_t framing_error;
    uint8_t cts;
    uint8_t dcd;
    uint8_t tx_empty;
    uint8_t rx_full;
};

void    console_init(void);
uint8_t console_read(uint16_t address);
void    console_write(uint16_t address, uint8_t data);
void    console_halt(void);

// void    console_status(void);
void    console_status(struct Acia *a6850);

// these routine provide the virtual RS232 serial interface for the console
//  + the external progarm (monitor.c in this case) is responsible for
//  + actually sending/recieiving of the characters
void    serial_in(char c);
int     serial_out(void);



