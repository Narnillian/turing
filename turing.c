#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h> //IO control
#include <termios.h>   //terminal interface
typedef struct square square;
typedef struct state state;
typedef struct instr instr;

/*
Machine organization:
(f4Lf,4Lf,3Rf,2Lf,0Rf,0Rb)(*b5LB,0Rb,5LB,4Rb,H,2Rb)(F5Rf,1RF,4Rb,2RF,H,1Rf)(B1LB,0RB,3LF,4RB,3LB,2RB)|00000005155*0000000
    -One state is encapsulated by parenthesis
    -If there's an asterisk, that's the initial state
    -The next character is the State Shorthand
    -Instructions, seperated by commas:
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
struct instr { //one instruction
    char newval; //write value of head
    char movehead; //based on moving right -- positive is right, negative is left, 0 is neither. will only move one position no matter the value (unless neither)
    char newstate;
};
struct state { //one state of the machine
    char title;
    instr *instructions;
    char *nickname; //used for longer, more descriptive names for the state -- haven't done yet

};
struct square { //one square of the tape
    square *prev;
    square *next;
    char value;

};


int loadinstrs();
int freevars();

char verbose = 0;
char *preset1 = "(f4Lf,4Lf,3Rf,2Lf,0Rf,0Rb)(*b5LB,0Rb,5LB,4Rb,H,2Rb)(F5Rf,1RF,4Rb,2RF,H,1Rf)(B1LB,0RB,3LF,4RB,3LB,2RB)|00000005155*0000000";
char *preset2; //               these will be made eventually
char *preset3; //               there will be preset machines
char *fullspec = NULL; //       machine we are using
state *allstates = NULL; //     all instruction sets
int actstate; //                active state
int values; //                  how many values a single tape square can have


int main(int argc, char **argv) {
    if (argv[1] && *argv[1] == 'v') verbose = 1; //this is temp
    fullspec = preset1; //this is temp
    printf("sizeof state: %d\n",sizeof(state));
    loadinstrs();
    printf("1-char name of Set [1]: '%c'\n", allstates[1].title); //b -- THIS IS WORKING
    //printf("Instruction [2] at Set [1]: %c\n", statelist[1].instructions[2]); //25LB
    freevars();
    return 0;
}

int loadinstrs() {
    char *split;
    int states = 0;
    int instr = 0;
    char instrpart;
    for (char *readchar = fullspec; *readchar; readchar++) {
        if (*readchar=='|') {
            split = readchar;
            break;
        }; //end of instructions
        if (*readchar=='(') {
            /* new set */
            if (verbose) {
                printf(">%d\n>%d\n", sizeof(*allstates), (states+1)*sizeof(*allstates));
                printf("There are \033[46m%d instruction sets\033[49m\n", states+1);
            }
            allstates = realloc(allstates, (states+1)*sizeof(*allstates));
            allstates[states].instructions = NULL;
            if (*(readchar+1)=='*') {
                actstate = states;
                readchar++;
            }
            readchar++;
            allstates[states].title = *readchar;
            readchar++; //because the first character of the instruction is the indicator -- i.e. when it's called
            instr=0;
            //states++;
            values++;
            continue;
        }
        if (*readchar==')') {
            states++;
            continue;
        }
        if (*readchar==',') { //end of instruction
            if (verbose) {
                printf("^%d\n^%d\n", sizeof(*allstates[states].instructions), (instr+1)*sizeof(*allstates[states].instructions));
                printf("There are \033[42m%d instructions per state\033[49m\n", instr+1);
            }
            allstates[states].instructions = realloc(allstates[states].instructions, (instr+1)*sizeof(*allstates[states].instructions));
            printf("\033[41m%p\033[49m\n",allstates[states]);
            instr++;
            readchar++; //because the first character of the instruction is the indicator -- i.e. when it's called
            continue;
        }

    }
    printf("\n\nThere are %d states\n",states);
    printf("There are %d insructions per state\n", instr);
    return 0;
}

int freevars() {
    /* it might be good practice to free all the parts of allstates */
    free(allstates);
    return 0;
}