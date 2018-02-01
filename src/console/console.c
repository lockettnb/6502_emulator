//
// 2018/01/01 created
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>         // for uint8_t etc
#include <string.h>

#include <sys/socket.h>
#include <sys/time.h>       // timeval structure
#include <sys/un.h>         // UNIX domain sockets

#include "../common.h"
#include "console.h"


#define SERVER_PATH "/tmp/osi_serial_server"
#define CLIENT_PATH "/tmp/osi_serial_client"
#define PING "!HELLO!"
#define PING_RESP "!HELLOBACK!"
#define HALT "!QUIT!"


 static Acia console;
 static socklen_t len;
 static struct sockaddr_un server_addr;    // server (ositerm)  address
 static struct sockaddr_un console_addr;    // console client address
 static int  cfd;                       // server socket file desc

// Function Prototypes
void start_terminal(void);


// receive serial input from terminal (ositerm)
//   returns TRUE if received char and char in ch
//   returns FALSE if no data 
int serial_in(char *ch)
{
 int  rc;
 int  nbytes = 0;
 char buf[256];
 int  max_sd;
 fd_set         readfds;
 struct timeval timeout;


    FD_ZERO(&readfds);
    FD_SET(cfd, &readfds);
    max_sd=cfd;
    // on linux the timeout is modified by select, need to reset it
    timeout.tv_sec  = 1;
    timeout.tv_usec = 0;

    rc = select(max_sd+1, &readfds, NULL, NULL, &timeout);
    if (rc < 0) {                    // select failed
        perror("select failed"); fflush(STDIN_FILENO);
        return FALSE; 
    } else if (rc == 0) {            // timeout
        printf(">>Timeout \n");
        return FALSE;
    }

    if (FD_ISSET(cfd, &readfds)){    // socket input ready
        nbytes = recvfrom(cfd, buf, 256, 0, NULL, NULL);
        if (nbytes == -1){
            perror("Console Read: RECVFROM ERROR " ); fflush(STDIN_FILENO);
            close(cfd);
            exit(1);
        }
        else {
           buf[nbytes]='\0';
           if( strcmp(buf, PING_RESP)==0 ){
               return FALSE;
           }
        }
        *ch=buf[0];
printf("serial in nbytes %i %s <%c>\n", nbytes, buf, *ch);
        return TRUE;
    } // SOCKET cfd SET

  // should never really get to this point
    printf("\r\nConsole Read: Unknown error"); fflush(STDIN_FILENO);
    return FALSE;
}

// receive serial output 
//  +if 6850 tx data is ready to send, return it else send NOT READY
void serial_out(char c)
{
 int  rc;
//  int  done = FALSE;
//  int  nbytes = 0;
 char buf[256];
//     if(!console.tx_empty){         // if data ready to transmit
//         console.tx_empty=TRUE;     // set tx data register empty flag
//         return console.tx;         // send the data 
//     } else {
//         return NO_TX_DATA;
//     }

        buf[0]=c;
        buf[1]='\0';
       rc = sendto(cfd, buf, strlen(buf), 0, (struct sockaddr*) &server_addr, len);
       printf("DATA SENT = %s <%02x> \n", buf, buf[0]);

       if (rc == -1) {
           perror("Serial Out: Server SENDTO ERROR "); fflush(STDIN_FILENO);
           close(cfd);
           exit(1);
       }
}



void console_init(void)
{
 int  rc;
 char buf[256];

//     console = malloc(sizeof(struct Acia));
    LOG("console serial port init\n");
    console.rx=0; console.rxshift=0;
    console.tx=0; console.txshift=0;

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


    memset(&server_addr, 0, sizeof(struct sockaddr_un));
    memset(&console_addr, 0, sizeof(struct sockaddr_un));
    memset(buf, 0, 256);

    cfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (cfd == -1){
        perror("Server SOCKET ERROR ");
        exit(1);
    }

    // start up ositerm, if it is not already running
    start_terminal();

    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, SERVER_PATH); 

    console_addr.sun_family = AF_UNIX;
    strcpy(console_addr.sun_path, CLIENT_PATH); 
    unlink(CLIENT_PATH);

    len = sizeof(server_addr);
    rc = bind(cfd, (struct sockaddr *) &console_addr, len);
    if (rc == -1){
        perror("Console Init: BIND ERROR " );
        close(cfd);
        exit(1);
    }

   return;
}

uint8_t console_read(uint16_t address)
{
// for control address construct bits and return them
// for data address, if rx_full flag is 1, then copy rxshift
// to rxdata and return rxdata and reset rx_full flag
 char c;
    if((address & 0x01) == 0){
        if( serial_in(&c) ){        // check if char received
            console.rxshift=c;
            console.rx_full=TRUE;
        }
        console.sr = 0;
        if(console.int_request) console.sr |= 0x80;
        if(console.parity_error)      console.sr |= 0x40;
        if(console.overrun_error)     console.sr |= 0x20;
        if(console.framing_error)     console.sr |= 0x10;
        if(console.cts)               console.sr |= 0x08;
        if(console.dcd)               console.sr |= 0x04;
        if(console.tx_empty)          console.sr |= 0x02;
        if(console.rx_full)           console.sr |= 0x01;
printf("console read %04x sr %02x rx full=%i \n", address, console.sr, console.rx_full);
        return console.sr;
    } // addr 0

    if ((address & 0x01) == 1){
printf("console read %04x rx full=%i <%02x> \n", address, console.rx_full, console.rxshift);
        if( console.rx_full ){
            console.rx=console.rxshift;
            console.rxshift=0;
            console.rx_full=FALSE;
            return console.rx;
        }
    } // addr 1

  // we should never really get to this point but if we do just return zer0
    LOG("Warning: read from invalid ACIA address %04x\n", address);
    return 0;
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
        console.txshift=data;        // not really using this but 
        console.tx_empty=FALSE;      // tx data register is NOT empty
        serial_out(data);
        console.tx_empty=TRUE;       //so what was the value in this?? 
    }

}

void console_halt(void)
{
char buf[42];

    LOG("console shutdown\n");

    strcpy(buf, HALT);
    sendto(cfd, buf, strlen(buf), 0, (struct sockaddr*) &server_addr, len);
    close(cfd);

    unlink(CLIENT_PATH);
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


void start_terminal(void)
{
 FILE *shcmd=NULL;
 char buf[256];

// need to make this smarter on finding the ositerm program 

    shcmd=popen("pgrep ositerm", "r");
    fgets(buf, 255, shcmd);
    if(strcmp(buf, "") ==0) {
        printf("starting ositerm\n");
        system("mate-terminal --geometry 80x24+10+10 -e /home/john/src/6502_emulator/build/ositerm &");
    }
 }
