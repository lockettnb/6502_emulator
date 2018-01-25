//
// 2018/01/22 
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>

// The <sys/time.h> header shall define the timeval structure
#include <sys/time.h>

// sys/un.h - definitions for UNIX domain sockets
#include <sys/un.h>

#define SOCKET_PATH "/tmp/osi_serial_server"
#define FALSE 0
#define TRUE 1

#define PING "!HELLO!"
#define PING_RESP "!HELLOBACK!"
#define HALT "!QUIT!"


int main(void)
{
 int  rc;
//  int  n;
 int  done = FALSE;

 socklen_t len;
 struct sockaddr_un server_sock;    // this server address struct
 struct sockaddr_un peer_sock;      // remote client address struct
 int  sockfd;                       // this server socket file desc
 int  nbytes = 0;
 char buf[256];

 int max_sd;
 fd_set         readfds;
 struct timeval timeout;

    memset(&server_sock, 0, sizeof(struct sockaddr_un));
    memset(buf, 0, 256);

    sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sockfd == -1){
        perror("Server SOCKET ERROR ");
        exit(1);
    }

    server_sock.sun_family = AF_UNIX;
    strcpy(server_sock.sun_path, SOCKET_PATH); 
    unlink(SOCKET_PATH);

    len = sizeof(server_sock);
    rc = bind(sockfd, (struct sockaddr *) &server_sock, len);
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
   
    while(!done) {
        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);
        max_sd=sockfd;
        // on linux the timeout is modified by select, need to reset it
        timeout.tv_sec  = 3;
        timeout.tv_usec = 0;

        printf("waiting for...\n");
        rc = select(max_sd+1, &readfds, NULL, NULL, &timeout);
        if (rc < 0) {                    // select failed
            perror("select failed");
            done=TRUE; 
        } else if (rc == 0) {            // timeout
            printf(">>Timeout \n");
            continue;
        }
    
        if (FD_ISSET(sockfd, &readfds)){    // socket input ready
            nbytes = recvfrom(sockfd, buf, 256, 0, (struct sockaddr *) &peer_sock, &len);
            if (nbytes == -1){
                perror("RECVFROM ERROR " );
                close(sockfd);
                exit(1);
            }
            else {
               buf[nbytes]='\0';
               printf("DATA RECEIVED = %s\n", buf);
               if( strcmp(buf, PING)==0 ) strcpy(buf, PING_RESP);
               if( strcmp(buf, HALT)==0 ) done=TRUE;

               rc = sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr*) &peer_sock, len);
               printf("DATA SENT = %s\n", buf);
               if (rc == -1) {
                   perror("Server SENDTO ERROR ");
                   close(sockfd);
                   exit(1);
               }   
            }
        }
    } // while

    unlink(SOCKET_PATH);
    close(sockfd);
    return 0;
}

// #include <unistd.h>
// #include <fcntl.h>
// 
// int fcntl(int fd, int cmd, ... /* arg */ );

