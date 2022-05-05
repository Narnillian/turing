#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h> //IO control
#include <termios.h>   //terminal interface
char *machine;

struct instr {
    //char *rule = malloc(sizeof(char)*4);
};

struct instrset {

};

//"struct instr name" --> "instr name"
typedef struct instr instr;



int main(int argc, char **argv) {
    //(f04Lf,14Lf,23Rf,32Lf,40Rf,50Rb)(*b05LB,10Rb,25LB,34,Rb,4H,52Rb)(F05Rf,11RF,24Rb,32RF,4H,51Rf)(B01LB,10RB,23RB,43LB,52RB)|00000005155*0000000
    instr first;
    machine = malloc(sizeof(char)*100);
    return 0;
    
}
