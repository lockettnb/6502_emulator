//
// 2018/01/01 created
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "../common.h"
#include "console.h"




static Acia console;


// receive serial input from external source 
void serial_in(char c)
{
    console.rxshift=0xff & c;      // serial data comes into shift register
    console.rx = console.rxshift; // then shift register moves to rx data register
    console.rx_full=TRUE;             // and turn on rx data ready bit

}

// receive serial output 
//  +if 6850 tx data is ready to send, return it else send NOT READY
int serial_out(void)
{
    if(!console.tx_empty){         // if data ready to transmit
        console.tx_empty=TRUE;     // set tx data register empty flag
        return console.tx;         // send the data 
    } else {
        return NO_TX_DATA;
    }

}



void console_init(void)
{
//     console = malloc(sizeof(struct Acia));
    LOG("console serial port init\n");
    console.rx=0;
    console.tx=0;

// disable rx int, disable tx int, RTS hi, 8bit/2stop/noparity, clock normal
    console.cr=0x50;

    console.rx_irq = FALSE;
    console.tx_irq = FALSE;
    console.rts = 1;
    console.bits=8;
    console.parity=NOPARITY;
    console.stop=2;
    console.baud=0;

// no IRQ, no parity err, no overrun, no frame err, CTS, DCD, tx ready, rx no data
    console.sr=0x0E;

    console.int_request=0;
    console.parity_error=0;
    console.overrun_error=0;
    console.framing_error=0;
    console.cts=1;
    console.dcd=1;
    console.tx_empty=TRUE;
    console.rx_full=FALSE;

}

uint8_t console_read(uint16_t address) {
// for control address construct bits and return them
// for data address, if rx_full flag is 1, then copy rxshift
// to rxdata and return rxdata and reset rx_full flag
    if((address & 0x01) == 0){
        // MC6850 STATUS REGISTER (sr)
        //  7    Interrupt Request (console ACIA IRQ pin is not connected)
        //  6    Receive Parity Error  (1=Error)
        //  5    Receive Overrun Error (1=Error)
        //  4    Receive Framing Error (1=Error)
        //  3    CTS level
        //  2    DCD level
        //  1    Transmit Data (0=Busy, 1=Ready/Empty)
        //  0    Receive Data  (0=No data, 1=Data can be read)
        console.sr = 0;
        if(console.int_request) console.sr |= 0x80;
        if(console.parity_error)      console.sr |= 0x40;
        if(console.overrun_error)     console.sr |= 0x20;
        if(console.framing_error)     console.sr |= 0x10;
        if(console.cts)               console.sr |= 0x08;
        if(console.dcd)               console.sr |= 0x04;
        if(console.tx_empty)          console.sr |= 0x02;
        if(console.rx_full)           console.sr |= 0x01;
        return console.sr;
    }

    if ((address & 0x01) == 1){
        console.rx=console.rxshift;
        console.rx_full=FALSE;
        return console.tx;
    }
       
    LOG("Warning: read from invalid ACIA address\n");
  // we should never really get to this point but if we do just return zer0
    return (0);
}


void console_write(uint16_t address, uint8_t data)
{
 int bits;     // temp used to help figure out register bits

  // address $00 is control register
  // address $01 is transmit register
    if((address & 0x01) == 0){
        console.cr=data;
        console.rx_irq = (data & 0x80) ? ENABLED: DISABLED;
        console.tx_irq = (data & 0x60)== 0x20 ? ENABLED: DISABLED;
        console.rts = (data & 0x60) == 0x40 ? 1:0;

        bits=(data & 0x1c)>>2;
        console.bits= bits < 4 ? 7:8;

        bits=(data & 0x1c)>>2;
        console.parity = (bits==0||bits==2||bits==6) ? EVENPARITY: ODDPARITY;
        console.parity = (bits==4||bits==5) ? NOPARITY: console.parity;

        bits=(data & 0x1c)>>2;
        console.stop=(bits == 4 || bits<=1) ? 2:1;

        console.baud=0;
        if( (data &0x03) == 3 ) console_init();
    }

    if((address & 0x01) == 1){
        console.tx=data;
        console.txshift=data;      // not really using this but 
        console.tx_empty=FALSE;        // tx data register is NOT empty
    }

}

void console_halt(void)
{
    LOG("console shutdown\n");
}


void console_status(Acia *a6850)
{

     a6850->cr=console.cr;             // control register
     a6850->sr=console.sr;             // status register
     a6850->rx=console.rx;             // receive data register
     a6850->tx=console.tx;             // transmit data register
     a6850->rxshift=console.rxshift;        // receive data shift register
     a6850->txshift=console.txshift;        // transmitt data shift register
     a6850->rx_irq=console.rx_irq;         // control register bits
     a6850->tx_irq=console.tx_irq;
     a6850->rts=console.rts;
     a6850->bits=console.bits;
     a6850->parity=console.parity;
     a6850->stop=console.stop;
     a6850->baud=console.baud;
     a6850->int_request=console.int_request;      // status register bits
     a6850->parity_error=console.parity_error;
     a6850->overrun_error=console.overrun_error;
     a6850->framing_error=console.framing_error;
     a6850->cts=console.cts;
     a6850->dcd=console.dcd;
     a6850->tx_empty=console.tx_empty;
     a6850->rx_full=console.rx_full;
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

