//
// 2018/01/22 
//
//  this (client) will test the ositerm server to send/receive chars
//  and ping requests

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
// #include <sys/types.h>
#include <sys/socket.h>

// The <sys/time.h> header shall define the timeval structure
#include <sys/time.h>

// sys/un.h - definitions for UNIX domain sockets
#include <sys/un.h>

// for statusline ioctl
#include <sys/ioctl.h> 
// for tty and signal control
#include <termios.h>
#include <signal.h>

#define FALSE 0
#define TRUE -1

#define SOCKET_PATH "/tmp/osi_serial_server"
#define CLIENT_PATH "/tmp/osi_serial_client"
#define PING "!HELLO!"
#define PING_RESP "!HELLOBACK!"
#define HALT "!QUIT!"
#define BSIZE 16

static struct termios save_termios;
// static int ttysavefd = -1;
static enum { RESET, RAW, CBREAK } ttystate = RESET;
static int ctrl_c_pressed = FALSE;

// tty control
int tty_raw(int fd);
int tty_reset(int fd);
// signal control
void sigint_grab(void);
void sigint_restore(void);
// top status line
// void statusline(char line[]);
// tx & rx buffer management
// void clearbuf(char buf[]);
// void addbuf(char buf[], char c);


int main(void)
{
 int  rc;
//  int  n;
 int  done = FALSE;
 int  ping_outstanding = FALSE;

 socklen_t len;
 struct sockaddr_un server_addr;       // this server address struct
 struct sockaddr_un this_client_addr;  // client address struct
 int  sockfd;                          // this server socket file desc
 int  nbytes = 0;
 char buf[256];
 FILE *shcmd=NULL;

 int max_sd;
 fd_set         readfds;
 struct timeval timeout;
    printf("type q to quit\n");

    memset(&server_addr, 0, sizeof(struct sockaddr_un));
    memset(&this_client_addr, 0, sizeof(struct sockaddr_un));
    memset(buf, 0, 256);

    sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sockfd == -1){
        perror("Server SOCKET ERROR ");
        exit(1);
    }

// #include <unistd.h>
// #include <sys/types.h>
// uid_t getuid(void);
// sprintf({buf, "system pgrep -u %i ositerm", getuid());
// result = system (buf);
   
    shcmd=popen("pgrep ositerm", "r");
    fgets(buf, 255, shcmd);
    if(strcmp(buf, "") ==0) {
        printf("starting ositerm\n");
        system("mate-terminal -e /home/john/src/6502_emulator/build/ositerm &");
    }

    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, SOCKET_PATH); 

    this_client_addr.sun_family = AF_UNIX;
    strcpy(this_client_addr.sun_path, CLIENT_PATH); 
    unlink(CLIENT_PATH);

 // here is the magic sauce. This means the server can send responses

    len = sizeof(this_client_addr);
    rc = bind(sockfd, (struct sockaddr *) &this_client_addr, len);
    if (rc == -1){
        perror("BIND ERROR " );
        close(sockfd);
        exit(1);
    }

// If  no  messages are available at the socket, the receive calls wait for a
// message to arrive, unless the socket is nonblocking (see fcntl(2)), in which
// case the value -1 is returned and the external variable errno is set to EAGAIN
// or EWOULDBLOCK.  The receive calls normally return any data available, up to
// the requested amount, rather than waiting for receipt of the full amount
// requested.

    tty_raw(STDIN_FILENO);

    while(!done) {
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds); 
        FD_SET(sockfd, &readfds);
        max_sd=sockfd;
        // on linux the timeout is modified by select, need to reset it
        timeout.tv_sec  = 8;
        timeout.tv_usec = 0;

//         printf("Ready...\n");
        rc = select(max_sd+1, &readfds, NULL, NULL, &timeout);
        if (rc < 0) {                    // select failed
            perror("select failed");
            done=FALSE;
            continue;
        } 
        if (rc == 0) {            // timeout
           if(ping_outstanding) {
            printf("i\r\nmissed ping,...\r\n"); fflush(STDIN_FILENO);
           }
               strcpy(buf, PING);
               rc = sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr *) &server_addr, len);
               if (rc == -1) {
                   perror("\r\nClient SENDTO ERROR");
//                    done=TRUE;
               }
               ping_outstanding = TRUE;
           continue;
        }

       // STDIN -- Standard In --
        if (FD_ISSET(STDIN_FILENO, &readfds)){
           rc = read(STDIN_FILENO, &buf, 256);
           buf[rc]='\0';
           rc = sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr *) &server_addr, len);
           if (rc == -1) {
               perror("Client SENDTO ERROR ");
               done=TRUE;
               continue;
           }
           printf("<%s>", buf); fflush(STDIN_FILENO);
           if(buf[0] == 'q'){
               done = TRUE;
               strcpy(buf, HALT);
               rc = sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr *) &server_addr, len);
           }
        } // STDIN

        if (FD_ISSET(sockfd, &readfds)){    // socket input ready
            nbytes = recvfrom(sockfd, buf, 256, 0, NULL, NULL);
            if (nbytes == -1){
                perror("RECVFROM ERROR " );
                done=TRUE;
                continue;
            }
            else {
               buf[nbytes]='\0';
               if(strcmp(buf, PING_RESP)==0) {
                  ping_outstanding = FALSE;
               }
               printf("%s", buf); fflush(STDIN_FILENO);
               rc = sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr *) &server_addr, len);
               if (rc == -1) {
                   perror("Client SENDTO ERROR ");
                   done=TRUE;
                   continue;
               }
            }

        }
    } // while

  // restore control-C and turn off raw tty
//     sigint_restore();
    tty_reset(STDIN_FILENO);

  // remove and close client socket 
    unlink(CLIENT_PATH);
    close(sockfd);
    return 0;
}


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
    ctrl_c_pressed = TRUE;
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


