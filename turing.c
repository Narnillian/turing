#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h> //IO control
#include <termios.h>   //terminal interface
typedef struct square square;
typedef struct state state;


struct state { //one state of the machine
    char title;
    char **instructions;
    char *nickname; //used for longer, more descriptive names for the state -- haven't done yet

};
struct square { //one square of the tape
    square *prev;
    square *next;
    char value;

};


int loadinstrs();

char *preset1 = "(f04Lf,14Lf,23Rf,32Lf,40Rf,50Rb)(*b05LB,10Rb,25LB,34,Rb,4H,52Rb)(F05Rf,11RF,24Rb,32RF,4H,51Rf)(B01LB,10RB,23LF,34RB,43LB,52RB)|00000005155*0000000";
char *preset2; //               these will be made eventually
char *preset3; //               there will be preset machines
char *fullspec; //              machine we are using
state *statelist = 0; //        all instruction sets
int actstate; //                active state


int main(int argc, char **argv) {
    fullspec = preset1; //this is temp
    printf("sizeof state: %d\n",sizeof(state));
    loadinstrs();
    printf("1-char name of Set [1]: '%c'\n", statelist[1].title); //b -- THIS IS WORKING
    //printf("Instruction [2] at Set [1]: %c\n", statelist[1].instructions[2]); //25LB
    free(statelist);
    return 0;
}


int loadinstrs() {
    int set = 0;
    int instrs = 0;
    char *split;
    for (char *i = fullspec; *i; i++) {
        if (*i=='|') {
            split = i;
            break;
        }; //end of instructions
        if (*i=='(') { //new set
            instrs=1;
            printf("%d\n%d\n", sizeof(*statelist), set*sizeof(*statelist));
            printf("There are \033[47m%d instruction sets\033[49m\n", set);
            printf("There are \033[42m%d instructions per state\033[49m\n", instrs);
            statelist = realloc(statelist, set*sizeof(*statelist));
            set++;
            i++;
            if (*i=='*') {
                actstate = set;
                i++;
            }
            statelist[set-1].title = *i;
            i++;
            continue;
        }
        if (*i==',') { //end of instruction
            instrs++;
            printf("There are \033[42m%d instructions per state\033[49m\n", instrs);
            continue;
        }
    }
    //we end up with total number of instructions, not number per set
    printf("\n\nThere are %d sets\n",set);
    printf("There are %d insructions per set\n", instrs);
    return 0;

}
