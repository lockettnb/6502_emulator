//
// 2018/01/23  stormy winter day, snow then ice pellets then rain
//

#include <stdio.h>
// #include <termios.h>
#include <sys/ioctl.h>      // for ioctl
#include <unistd.h>         // for STDIN_FILENO 
#include <string.h>         // for strlen
#include <stdlib.h>         // for malloc


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

    // build the padding string  to fill the size of screen
    npad= win.ws_col - strlen(line);
    for(i=0; i<npad-1; i++) pad[i]='_';
    pad[npad-1]='|';
    pad[npad]='\0';

    sprintf(buffer, "%s%s", line, pad);

    printf("\e7\e[0;0H\e[7m");  // save cursor, move 0:0, rev video on
    printf("%s", buffer);
    printf("\e[0m\e8");         // rev video off, restore cursor
    fflush(stdout);

 // Esc[Line;ColumnH 	Move cursor to screen location v,h 
 // Esc[7m 	Turn reverse video on 
 // Esc[0m 	Turn off character attributes 
 // Esc7 	Save cursor position and attributes 
 // Esc8 	Restore cursor position and attributes 
}


int main(int argc, char *argv[])
{
 struct winsize win;
 char cmd[32];

    ioctl(STDOUT_FILENO, TIOCGWINSZ, &win);
//     ioctl(0, TIOCGWINSZ, &win);
//     printf("Screen width: %i  Screen height: %i\n", win.ws_col, win.ws_row);
//     printf("\e[2J");
//     printf("\e[0;0HA");
//     sprintf(cmd, "\e[%i;%iHB", win.ws_row, win.ws_col);
//     printf("%s", cmd);
//     sprintf(cmd, "\e[%i;%iHC", 0, win.ws_col);
//     printf("%s", cmd);
//     sprintf(cmd, "\e[%i;%iHD", win.ws_row, 0);
//     printf("%s", cmd);
// fflush(stdout);
    statusline("Hello");
    sleep(2);
    statusline("World.....");

    return 0;
}

// NOTES
// 
// Esc[2J 	Clear entire screen
// Esc[2K 	Clear entire line  
// Esc[Line;ColumnH 	Move cursor to screen location v,h 
// Esc[7m 	Turn reverse video on 
// Esc[0m 	Turn off character attributes 
// Esc7 	Save cursor position and attributes 
// Esc8 	Restore cursor position and attributes 



//  int ioctl(int fd, int cmd, ...);
// 
// TIOCGWINSZ
//     Fill in the winsize structure pointed to by the third argument with the screen width and height.
// 
//     The winsize structure is defined in `sys/ioctl.h' as follows:
// 
//      	
// 
//     struct winsize
//     {
//       unsigned short ws_row;	/* rows, in characters */
//       unsigned short ws_col;	/* columns, in characters */
//       unsigned short ws_xpixel;	/* horizontal size, pixels */
//       unsigned short ws_ypixel;	/* vertical size, pixels */
//     };
// 
// Return Value
// Zero for TIOCGWINSZ. Otherwise, -1 is returned and errno is set to ENOSYS for all others.
// 
// #include <sys/ioctl.h>
// #include <stdio.h>
// int main(int argc, char **argv)
// {
//   struct winsize sz;
// 
//   ioctl(0, TIOCGWINSZ, &screen_size);
//   printf("Screen width: %i  Screen height: %i\n", sz.ws_col, sz.ws_row);
//   return 0;
// }
