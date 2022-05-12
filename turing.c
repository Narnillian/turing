#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h> //IO control
#include <termios.h>   //terminal interface
typedef struct square square;
typedef struct instrset instrset;


struct instrset {
    char title;
    char **instructions;
    char *nickname; //used for longer, more descriptive names for the state -- haven't done yet

};
struct square {
    square *prev;
    square *next;
    char value;

};


int loadinstrs();

char *preset1 = "(f04Lf,14Lf,23Rf,32Lf,40Rf,50Rb)(*b05LB,10Rb,25LB,34,Rb,4H,52Rb)(F05Rf,11RF,24Rb,32RF,4H,51Rf)(B01LB,10RB,23RB,43LB,52RB)|00000005155*0000000";
char *preset2; //               these will be made eventually
char *preset3; //               there will be preset machines
char *fullspec; //              holds the machine we are using
instrset *instrsets; //         holds all instruction sets


int main(int argc, char **argv) {
    fullspec = preset1; //this is temp
    loadinstrs();
    return 0;
}


int loadinstrs() {
    int set,instrs = 0;
    for (char *i = fullspec; *i; i++) {
        if (*i=='|') break;
        if (*i==')') {
            set++;
            instrs++;
            printf("There are %d \033[47minstruction sets\033[49m\n", set);
            continue;
        }
        if (*i==',') {
            instrs++;
            printf("There are %d \033[42minstructions per set\033[49m\n", instrs);
            continue;
        }
    }
    //we end up with total number of instructions, not number per set
    printf("\n\nThere are %d sets\n",set);
    printf("There are %d insructions per set\n", instrs);
    return 0;

}
