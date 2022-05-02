#include <stdio.h>
#include <sys/ioctl.h>
#include <termios.h>

int main() {

    //get window size
    struct winsize ws;
    ioctl(0, TIOCGWINSZ, &ws);
    printf("%i\n",ws.ws_col);
    return 0;
}
