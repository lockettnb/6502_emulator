//
// 2018/01/23 stormy winter day
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

// for statusline ioctl
#include <sys/ioctl.h> 
// for tty and signal control
#include <termios.h>
#include <signal.h>

#define SERVER_PATH "/tmp/osi_serial_server"
#define CLIENT_PATH "/tmp/osi_serial_client"
#define PING "!HELLO!"
#define PING_RESP "!HELLOBACK!"
#define HALT "!QUIT!"
#define BSIZE 16

#define FALSE 0
#define TRUE 1

static struct termios save_termios;
// static int ttysavefd = -1;
static enum { RESET, RAW, CBREAK } ttystate = RESET;
int sentinel;        // sigint sentinal MOVE TO util.h

// tty control
    int tty_raw(int fd);
    int tty_reset(int fd);
// signal control
    void sigint_grab(void);
    void sigint_restore(void);
// top status line
void statusline(char line[]);
// tx & rx buffer management
void clearbuf(char buf[]);
void addbuf(char buf[], char c);



int main(int argc, char *argv[])
{
 struct sockaddr_un my_addr;        // client (my) address
 struct sockaddr_un server_addr;    // server address 
//  socklen_t len;
 int sfd;                           // socket file desc

 int  rc;
 char buf[256];
 int  done = FALSE;
 int  waiting_ping = FALSE;
 int  conn_down = FALSE;
 int  nbytes;
 char ch;
 char statbar[255];
 char txbuf[BSIZE+1];
 char rxbuf[BSIZE+1];
 char toggch = '|';

 int max_sd;
 fd_set         readfds;
 struct timeval timeout;

  // Esc[2J 	Clear entire screen
 // Esc[Line;ColumnH 	Move cursor to screen location v,h 
    printf("\e[2J\e[3;0H -- OSI Challenger 3 --\r\n Terminal Ready\r\n"); fflush(STDIN_FILENO);

    sentinel=FALSE;                 // control-C detection
    statbar[0]='\0';                // status line buffer
    clearbuf(txbuf);
    clearbuf(rxbuf);
    memset(&server_addr, 0, sizeof(struct sockaddr_un));
    memset(&my_addr, 0, sizeof(struct sockaddr_un));
//     len = sizeof(server_addr);

    if(argc == 2) 
        strcpy(buf, argv[1]);
    else
        strcpy(buf, PING);

  // AF_UNIX      Local communication  
  // SOCK_DGRAM   Supports datagrams
  // Protocol     default
    sfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sfd == -1) {
        perror("SOCKET ERROR ");
        exit(1);
    }

  // address of the server
    server_addr.sun_family = AF_UNIX;        
    strcpy(server_addr.sun_path, SERVER_PATH); 
  // my (client) address
    my_addr.sun_family = AF_UNIX;        
    strcpy(my_addr.sun_path, CLIENT_PATH); 
    unlink(my_addr.sun_path);

  // here is the magic sauce. This means the server can send responses
    rc=bind(sfd, (struct sockaddr *) &my_addr, sizeof(struct sockaddr_un));
    if(rc == -1){
        perror("Client SENDTO ERROR ");
        done=TRUE;
    }

    sigint_grab();
    tty_raw(STDIN_FILENO);

    while(!done) {
        sprintf(statbar, "%c Tx<%s| Rx<%s|", toggch, txbuf, rxbuf);
        statusline(statbar);
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds); 
        FD_SET(sfd, &readfds);
        max_sd=sfd;
        // on linux the timeout is modified by select, need to reset it
        timeout.tv_sec  = 3;
        timeout.tv_usec = 0;

//         printf("waiting for...\n");

        rc = select(max_sd+1, &readfds, NULL, NULL, &timeout);
        if (rc < 0) {                    // select failed
            if(!sentinel) perror("select failed");
            done=TRUE; 
        } else if (rc == 0) {            // timeout
               waiting_ping=TRUE; 
               strcpy(buf, PING);
               rc=sendto(sfd, buf, 7, 0, (struct sockaddr *) &server_addr, sizeof(struct sockaddr_un));
            if (rc == -1) {
                if(errno == ECONNREFUSED) {
                    toggch='!';
                    conn_down=TRUE;
                } else {
                    perror("Client SENDTO ERROR ");
                }
            }
            continue;
        }

        // STDIN -- Standard In --
        if (FD_ISSET(STDIN_FILENO, &readfds)){
           rc = read(STDIN_FILENO, &ch, 1);
           if(conn_down) {
               printf("!"); fflush(STDIN_FILENO);
               continue;
           }
           rc = sendto(sfd, &ch, 1, 0, (struct sockaddr *) &server_addr, sizeof(struct sockaddr_un));
           if (rc == -1) {
               perror("Client SENDTO ERROR ");
//             done=TRUE;
           }
           addbuf(txbuf, ch);
        } // STDIN

        //  == SOCKET ==
        if (FD_ISSET(sfd, &readfds)){
            nbytes = recvfrom(sfd, buf, 256, 0, NULL, NULL);
            if (nbytes == -1){
                perror("RECVFROM ERROR " );
                done=TRUE; 
            }
            else {
               buf[nbytes]='\0';
               if(nbytes==1) addbuf(rxbuf, buf[0]);

               if(strcmp(buf, HALT)==0) done=TRUE;

               if(waiting_ping && strcmp(buf, PING_RESP)==0){
                    conn_down=FALSE;
                   if(toggch == '|') toggch='X';
                    else toggch='|';
                   continue;
               }
               printf("%s", buf); fflush(STDIN_FILENO);
            }
        } // SOCKET 

        if(sentinel) {
            done = TRUE;
        }

   } // while


  // restore control-C and turn off raw tty
    sigint_restore();
    sentinel = FALSE;
    tty_reset(STDIN_FILENO);

  // remove and close client socket 
    unlink(my_addr.sun_path);
    rc = close(sfd);
    return 0;
} //main


// ******************************************************************
// TTY ROUTINES
// tty to raw input
int tty_raw(int fd)
{
 struct termios buf;

    if (tcgetattr(fd, &save_termios) < 0)
        return(-1);

    buf = save_termios; 
    buf.c_lflag &= ~ (ECHO | ICANON | IEXTEN );
    //  buf.c_lflag &= ~ (ECHO | ICANON | IEXTEN | ISIG);

    buf.c_iflag &= ~ (BRKINT | ICRNL | INPCK | ISTRIP | IXON);

    buf.c_cflag |= ~(CSIZE | PARENB);

    buf.c_oflag &= ~ (OPOST);

//     buf.c_cc[VMIN] = 1;
// set VMIN and VTIME to zero for non-blocking reads
    buf.c_cc[VMIN] = 0;
    buf.c_cc[VTIME]= 0;

    if (tcsetattr(fd, TCSAFLUSH, &buf) < 0)
       return(-1);

    ttystate = RAW;
//     ttysavefd = fd;
    return(0);
}

// restore tty to cooked input
int tty_reset(int fd)
{
    if((ttystate != CBREAK && ttystate != RAW))
      return(0);

    if(tcsetattr(fd, TCSAFLUSH, &save_termios) < 0)
      return(-1);

    ttystate = RESET;
    return(0);

}



// ******************************************************************
// signal hander for SIGINT
void sig_handler(int signo)
{
  if (signo == SIGINT) {
    printf("\r\nreceived SIGINT\r\n");
    sentinel = TRUE;
  }
}


// grab a hold of the SIGINT signal
void sigint_grab(void)
{
    if (signal(SIGINT, sig_handler) == SIG_ERR)
        printf("\ncan't catch SIGINT\n");

}

// restore SIGINT to default
void sigint_restore(void)
{
    signal(SIGINT, SIG_DFL);
}



// print statusline on top of screen
//  not using termcap/terminfo, doing it the lazy (hardcoded) way
void statusline(char line[])
{
 struct winsize win;
 char *buffer;
 char *pad;
 int  npad;
 int  i;

    ioctl(STDOUT_FILENO, TIOCGWINSZ, &win);

    buffer =  malloc(sizeof(char) * (win.ws_col+1));
    pad    =  malloc(sizeof(char) * (win.ws_col+1));

    if(strlen(line) >= win.ws_col) {            // line too long chop it off
        strncpy(buffer, line, win.ws_col-1);
        buffer[win.ws_col-1]='>';
        buffer[win.ws_col]='\0';
    } else {                    // build the padding string to fill the size of screen
        npad= win.ws_col - strlen(line);
        for(i=0; i<npad-1; i++) pad[i]='_';
        pad[npad-1]='|';
        pad[npad]='\0';
        sprintf(buffer, "%s%s", line, pad);
    }

    printf("\e7\e[0;0H\e[7m");  // save cursor, move 0:0, rev video on
    printf("%s", buffer);
    printf("\e[0m\e8");         // rev video off, restore cursor
    fflush(stdout);

 // Esc[Line;ColumnH 	Move cursor to screen location v,h 
 // Esc[7m 	Turn reverse video on 
 // Esc[0m 	Turn off character attributes 
 // Esc7 	Save cursor position and attributes 
 // Esc8 	Restore cursor position and attributes 
    free(buffer);
    free(pad);
}



void clearbuf(char buf[])
{
 int i;
   for (i=0; i<=BSIZE ; i++) buf[i]=' '; 

   buf[BSIZE+1]='\0';

}

void addbuf(char buf[], char c)
{
 int i;

    for (i=0; i<=BSIZE; i++){
        buf[i] = buf[i+1];
    }
    if(c<32 || c> 126)
         buf[BSIZE]='.';
    else
        buf[BSIZE]=c;

    buf[BSIZE+1]='\0';
}


