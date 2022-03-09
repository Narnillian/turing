#include <iostream>
#include <sys/ioctl.h>
#include <termios.h>
using namespace std;

int main() {

    //get window size
    struct winsize ws;
    ioctl(0, TIOCGWINSZ, &ws);
    cout << ws.ws_col << "\n";
}
