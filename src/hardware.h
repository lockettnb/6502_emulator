//
// 2018/01/20  created, moving hardware circuits into separate include
//
#ifndef HARDWARE_BITS_H
#define HARDWARE_BITS_H


// 6820 Peripheral Interface Adapter (PAI)
//  address 0 = r/w output register A (cra2=1) or DDRA (cra2=0)
//  address 1 = r/w control register A
//  address 2 = r/w output register B (crb2=1) or DDRB (crb2=0)
//  address 3 = r/w control register B
typedef struct {
    uint8_t cra;        // Control Register A
    uint8_t ddra;       // Data Direction Register A
    uint8_t ora;        // Output Register A
    uint8_t crb;
    uint8_t ddrb;
    uint8_t orb;
} Pia; 

// PIA Control Register:
//  ┌─────────────────────────────────────────────┐ 
//  │  7  │ 6    │ 5    4   3 │  2     │ 1   0    │ 
//  │     │      │            │        │          │ 
//  │ IRQ1│ IRQ2 │    CA1     │  DDR   │ CA1      │ 
//  │     │      │    Control │  Access│ Control  │ 
//  └─────────────────────────────────────────────┘ 




// 6850 Asynchronous Communication Interface Adapter (ACIA)
//  address 0 = control(write) or status (read)
//  address 1 = transmit(write) or receive(read)
typedef struct {
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
} Acia ;

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

#endif // HARDWARE_BITS_H 
