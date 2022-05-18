#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h> //IO control
#include <termios.h>   //terminal interface
typedef struct square square;
typedef struct state state;

/*
Machine organization:
(f04Lf,14Lf,23Rf,32Lf,40Rf,50Rb)(*b05LB,10Rb,25LB,34Rb,4H,52Rb)(F05Rf,11RF,24Rb,32RF,4H,51Rf)(B01LB,10RB,23LF,34RB,43LB,52RB)|00000005155*0000000
    -One state is encapsulated by parenthesis
    -If there's an asterisk, that's the initial state
    -The next character is the State Shorthand
    -Instructions, seperated by commas:
        -Character to check against tape[head]
        Then:
            -Character to change tape[head] to
            -Direction to move -- exactly one square Right, Left, or Neither
            -New active state -- indicated by State Shorthand
        Or:
            -Halt
    -(Unlimited number of states, instructions per state)
    -Pipe symbol -- splits States from Tape
    -Tape
        -Aterisk indicates initial head position

*/


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

char *preset1 = "(f04Lf,14Lf,23Rf,32Lf,40Rf,50Rb)(*b05LB,10Rb,25LB,34Rb,4H,52Rb)(F05Rf,11RF,24Rb,32RF,4H,51Rf)(B01LB,10RB,23LF,34RB,43LB,52RB)|00000005155*0000000";
char *preset2; //               these will be made eventually
char *preset3; //               there will be preset machines
char *fullspec; //              machine we are using
state *statelist = 0; //        all instruction sets
int actstate; //                active state
char ***allinstrs; //           list of instructions, referenced by each state


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
    int instr = 0;
    char *split;
    for (char *readchar = fullspec; *readchar; readchar++) {
        if (*readchar=='|') {
            split = readchar;
            break;
        }; //end of instructions
        if (*readchar=='(') { //new set
            printf(">%d\n>%d\n", sizeof(*statelist), (set+1)*sizeof(*statelist));
            printf("{%d\n{%d\n", sizeof(*allinstrs), (set+1)*sizeof(*allinstrs));
            printf("There are \033[46m%d instruction sets\033[49m\n", set+1);
            statelist = realloc(statelist, (set+1)*sizeof(*statelist));
            allinstrs = realloc(allinstrs, (set+1)*sizeof(*allinstrs));
            readchar++;
            if (*readchar=='*') {
                actstate = set;
                readchar++;
            }
            statelist[set].title = *readchar;
            instr=1;
            readchar++;
            set++;
            continue;
        }
        if (*readchar==')') continue;
        if (*readchar==',') { //end of instruction
            instr++;
            printf("There are \033[42m%d instructions per state\033[49m\n", instr);
            printf("^%d\n^%d\n%d\n", sizeof(*allinstrs[set]), instr*sizeof(*allinstrs[set]));
            allinstrs[set] = realloc(allinstrs[set], instr*sizeof(*allinstrs[set]));
            continue;
        }
    }
    printf("\n\nThere are %d sets\n",set);
    printf("There are %d insructions per set\n", instr);
    return 0;

}

/*
(as,df,gh,jk,l;)(qw,er,ty,ui,op)(zx,cv,bn,m',:\)

char* -- list of chars -- string
char** -- list of list of chars -- list of strings
char*** -- list of list of list of chars -- list of list of strings

globally:
list: (i.e. all instructions for all states) [realloc when new state]
    list: (i.e. instructions for state) [realloc when new instruction]
        strings: (i.e. each instruction) []
            as
            df
            gh
        -indicated by instr
    -indicated by set



inside struct:
list: (i.e. instructions for state)
    strings: (i.e. each instruction)
        as
        df
        gh
*/