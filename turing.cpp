#include <iostream>
#include <sys/ioctl.h>
#include <termios.h>
using namespace std;

int main() {

    //get window length
    struct winsize ws;
    ioctl(0, TIOCGWINSZ, &ws);
    cout << ws.ws_col << "\n";
}