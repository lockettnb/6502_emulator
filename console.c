//
// 2018/01/01 created
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "console.h"

#define NOPARITY   0
#define EVENPARITY 1
#define ODDPARITY  2

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

#define LOG(fmt, ...)  printf(fmt, ##__VA_ARGS__ )

struct Acia {
    uint8_t cr;
    uint8_t sr;
    uint8_t rx;
    uint8_t tx;
    uint8_t rxshift;
    uint8_t txshift;

    uint8_t rx_irq;
    uint8_t tx_irq;
    uint8_t rts;
    uint8_t bits;
    uint8_t parity;
    uint8_t stop;
    uint8_t baud;

    uint8_t interrupt_request;
    uint8_t parity_error;
    uint8_t overrun_error;
    uint8_t framing_error;
    uint8_t cts;
    uint8_t dcd;
    uint8_t tx_data;
    uint8_t rx_data;
};

static struct Acia *console;

void console_init(void)
{
    console = malloc(sizeof(struct Acia));

// disable rx int, disable tx int, RTS hi, 8bit/2stop/noparity, clock normal
    console->cr=0x50;
// no IRQ, no parity err, no overrun, no frame err, CTS, DCD, tx ready, rx no data
    console->sr=0x0E;

    console->rx=0;
    console->tx=0;

    console->rx_irq = FALSE;
    console->tx_irq = FALSE;
    console->rts = 1;
    console->bits=8;
    console->parity=NOPARITY;
    console->stop=2;
    console->baud=0;

    console->interrupt_request=0;
    console->parity_error=0;
    console->overrun_error=0;
    console->framing_error=0;
    console->cts=1;
    console->dcd=1;
    console->tx_data=1;
    console->rx_data=0;

}

uint8_t console_read(uint16_t address) {
    LOG("console read\r\n");
    return (0);
}


void console_write(uint16_t address, uint8_t data) 
{
 int bits;

// address $00 is control register
// address $01 is transmit register
if((address & 0x01) == 0 ) {
    console->cr=data;
    console->rx_irq = (data & 0x80) ? ENABLED: DISABLED;
    console->tx_irq = (data & 0x60)== 0x20 ? ENABLED: DISABLED;
    console->rts = (data & 0x60) == 0x40 ? 1:0;

    bits=(data & 0x1c)>>2;
    console->bits= bits < 4 ? 7:8;

    bits=(data & 0x1c)>>2;
    console->parity = (bits==0||bits==2||bits==6) ? EVENPARITY: ODDPARITY;
    console->parity = (bits==4||bits==5) ? NOPARITY: console->parity;

    bits=(data & 0x1c)>>2;
    console->stop=(bits == 4 || bits<=1) ? 2:1;

    console->baud=0;
    if( (data &0x03) == 3 ) console_init();
} else {
    console->tx=data;
    putchar(data);
}

}

void console_halt(void) {
    LOG("console shutdown\n");
}


void console_status(void)
{
 char bits[32];

  printf("___________________________________________________________________\n");
    printf("CONSOLE STATUS\n");
        printf(" Receive Buffer  %i 0x%02x\n", console->rx, console->rx);
        printf(" Transmit Buffer %i 0x%02x\n", console->tx, console->tx);

    sprintf(bits,"%c %c %c %c %c %c %c %c", BITS(console->cr));
    printf("                              7 6 5 4 3 2 1 0\n");
    printf("CONTROL REGISTER 0x%02x         %s\n", console->cr, bits);

    if((console->cr & 0x80) == 0)
        printf("interrupts disabled-----------' | | | | | | |\n");
    else
        printf("rx interrupts enabled---------' | | | | | | |\n");

    switch ((console->cr & 0x60)>>5) {
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

    switch ((console->cr & 0x1C)>>2) {
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
    switch (console->cr & 0x03) {
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
    printf("|rx_irq:%i", console->rx_irq );
    printf("|tx_irq:%i", console->tx_irq );
    printf("|rts:%i", console->rts );
    printf("|bits:%i", console->bits);
    if(console->parity==EVENPARITY) printf("|parity:even");
    if(console->parity==ODDPARITY) printf("|parity:odd");
    if(console->parity==NOPARITY) printf("|parity:no");
    printf("|stop:%i", console->stop);
    printf("|baud_clock:%i|\n", console->baud);

  // status register
    sprintf(bits,"%c %c %c %c %c %c %c %c", BITS(console->sr));
    printf("\n                              7 6 5 4 3 2 1 0\n");
    printf("STATUS REGISTER 0x%02x          %s\n", console->sr, bits);
    printf("Interrupt request ------------' | | | | | | |\n");
    printf("Parity error -------------------' | | | | | |\n");
    printf("Rx overrun -----------------------' | | | | |\n");
    printf("Framing error ----------------------' | | | |\n");
    printf("CTS ----------------------------------' | | |\n");
    printf("DCD ------------------------------------' | |\n");
    printf("Tx data register empty -------------------' |\n");
    printf("Rx data register full ----------------------'\n");
    printf("|irq:%i", console->interrupt_request);
    printf("|parity:%i", console->parity_error);
    printf("|overrun:%i", console->overrun_error);
    printf("|framing:%i", console->framing_error);
    printf("|cts:%i", console->cts);
    printf("|dcd:%i", console->dcd);
    printf("|tx:%i", console->tx_data);
    printf("|rx:%i|\n", console->rx_data);
  printf("___________________________________________________________________\n\n");
}


int parity(int x, int size)
{
    int i;
    int p = 0;
    x = (x & ((1<<size)-1));
    for (i=0; i<size; i++)
    {
        if (x & 0x1) p++;
        x = x >> 1;
    }
    return (0 == (p & 0x1));
}


// MC6850 CONTROL REGISTER (cr)
//  7    Receive Interrupt (1=Enable on buffer full/buffer overrun)
//  5-6  Transmit Interrupt/RTS/Break control (0..3)
//        00 = Output /RTS=low,  and disable Tx Interrupt
//        01 = Output /RTS=low,  and enable Tx Interrupt
//        10 = Output /RTS=high, and disable Tx Interrupt
//        11 = Output /RTS=low,  and disable Tx Interrupt, and send a Break
//  2-4  Mode (0..7 = 7e2,7o2,7e1,7o1,8n2,8n1,8e1,8o1) (data/stop bits, parity)
//        000 - 7 bit, even, 2 stop bit
//        001 - 7 bit, odd, 2 stop bit
//        010 - 7 bit, even, 1 stop bit
//        011 - 7 bit, odd, 1 stop bit
//        100 - 8 bit, 2 stop bit
//        101 - 8 bit, 1 stop bit
//        110 - 8 bit, even, 1 stop bit
//        111 - 8 bit, odd, 1 stop bit
//  0-1  Baudrate (0=CLK/64, 1=CLK/16, 2=CLK/1, 3=RESET)
//        00 - Normal
//        01 - Div by 16
//        10 - Div by 64
//        11 - Master reset
// 
// MC6850 STATUS REGISTER (sr)
// 
//  7    Interrupt Request (console ACIA IRQ pin is not connected)
//  6    Receive Parity Error  (1=Error)
//  5    Receive Overrun Error (1=Error)
//  4    Receive Framing Error (1=Error)
//  3    CTS level
//  2    DCD level
//  1    Transmit Data (0=Busy, 1=Ready/Empty)
//  0    Receive Data  (0=No data, 1=Data can be read)
// 
// + data can be read when Status Bit0=1
// + data can be written when Status Bit1=1

