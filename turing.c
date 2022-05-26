#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h> /* IO control */
#include <termios.h>   /* terminal interface */
typedef struct square square;
typedef struct instr instr;

/*
Machine organization:
(f;4Lf,4Lf,3Rf,2Lf,0Rf,0Rb)(*b;5LB,0Rb,5LB,4Rb,H,2Rb)(F;5Rf,1RF,4Rb,2RF,H,1Rf)(B;1LB,0RB,3LF,4RB,3LB,2RB)|00000005155*0000000
    -One state is encapsulated by parenthesis
    -If there's an asterisk, that's the initial state
    -The next characters until the semicolon is the State Shorthand
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

struct {
    char *title; /* direct title of state -- can be as long as you want, above length 0 */
    char *nickname; /*used for longer, more descriptive names for the state -- haven't done yet */
    
    struct { /* one instruction */
        char halt; /* positive value if instruction is HALT */
        char newval; /* write value of head */
        char movehead; /* based on moving right -- positive is right, negative is left, 0 is neither. will only move one position no matter the value (unless neither) */
        char *newstate;
    } *instructions;

} *allstates = NULL; /* global list of all states */

struct square { /* one square of the tape */
    char value;
    square *prev;
    square *next;

};


int loadinstrs();
int freevars();

char verbose = 0;
char *preset1 = "(f;4Lf,4Lf,3Rf,2Lf,0Rf,0Rb)(*b;5LB,0Rb,5LB,4Rb,H,2Rb)(F;5Rf,1RF,4Rb,2RF,H,1Rf)(B;1LB,0RB,3LF,4RB,3LB,2RB)|00000005155*0000000";
char *preset2 = NULL; /*        these will be made eventually                   */
char *preset3 = NULL; /*        there will be preset machines                   */
char *fullspec = NULL; /*       machine we are using                            */
int actstate; /*                active state                                    */
//int values; /*                  how many values a single tape square can have   */


int main(int argc, char **argv) {
    if (argv[1] && *argv[1] == 'v') verbose = 1; /* this is temp */
    fullspec = preset1; /* this is temp */
    printf("Instruction loading:\n\n");
    loadinstrs();
    freevars();
    return 0;
}

int loadinstrs() {
    char *split = 0;
    int states = 0;
    int instrs = 0;
    char instrpart;
    for (char *readchar = fullspec; *readchar; readchar++) {
        printf("\033[46m\"%c\"\033[49m\n",*readchar);
        if (*readchar == '|') {
            printf("PIPE-CHAR!\n");
            /* end of instructions */
            split = readchar;
            break;
        }
        if (*readchar == '(') {
            printf("OPEN-PAREN\n");
            /* new set */
            if (verbose) {
                printf(">%d\n>%d\n", sizeof(*allstates), (states+1)*sizeof(*allstates));
                printf("There are \033[46m%d instruction sets\033[49m\n", states+1);
            }
            allstates = realloc(allstates, (states+1)*sizeof(*allstates));
            allstates[states].instructions = NULL;
            allstates[states].instructions = realloc(allstates[states].instructions, (instrs+1)*sizeof(*allstates[states].instructions));
            if (*(readchar+1)=='*') {
                actstate = states;
                readchar++;
            }
            { /* iter only exists inside these braces */
                allstates[states].title = NULL;
                int iter = 0;
                while(++iter) {
                    readchar++;
                    if (*readchar == ';') {
                        //readchar++;
                        break;
                    }
                    if (verbose) printf("(%d\n(%d\n(%d\n(%d\n",states,iter,sizeof(*allstates[states].title),iter*sizeof(*allstates[states].title));
                    allstates[states].title = realloc(allstates[states].title, iter*sizeof(*allstates[states].title));
                    allstates[states].title[iter-1] = *readchar;
                }
                allstates[states].title[iter-1] = 0;
            }
            //values++;
            instrs = 0;
            continue;
        }
        if (*readchar == ')') {
            printf("CLOSE-PAREN\n");
            states++;
            if (verbose) printf("There are \033[42m%d instructions per state\033[49m\n", ++instrs);
            continue;
        }
        if (*readchar == ',') {
            printf("COMMA-SEPER\n");
            /* end of instruction */
            if (verbose) {
                printf("^%d\n^%d\n", sizeof(*allstates[states].instructions), (instrs+1)*sizeof(*allstates[states].instructions));
                printf("There are \033[42m%d instructions per state\033[49m\n", instrs+1);
            }
            allstates[states].instructions = realloc(allstates[states].instructions, (instrs+1)*sizeof(*allstates[states].instructions));
            if (verbose) printf("\033[41m%p\033[49m\n",allstates[states]);
            instrs++;
            continue;
        }
        printf("DEFAULT\n");
        printf("%d || %d\n",states,instrs);
        //printf("%p\n",allstates[states].instructions);
        //printf("%p\n",readchar);
        //printf("%p\n",++readchar);
        //readchar--;
        //printf("%p\n\n",readchar++);
        //readchar--;
        
        readchar+=2;

        //allstates[states].instructions[instrs].newval = *readchar;
        //readchar++;
        //switch(*readchar) {
        //    case 'L':
        //        printf("LEFT\n");
        //        allstates[states].instructions[instrs].movehead = -1;
        //        break;
        //    case 'R':
        //        printf("RITE\n");
        //        allstates[states].instructions[instrs].movehead = 1;
        //        break;
        //    default:
        //        printf("NONE\n");
        //        allstates[states].instructions[instrs].movehead = 0;
        //}
        //readchar++;
        //readchar+=1;

        //{
        //    int iter = 0;
        //    while(++iter) {
        //        if (*readchar == ';') {
        //            readchar--;
        //            break;
        //        };
        //        //if (verbose) printf(")%d\n)%d\n)%d\n)%d\n",states,iter,sizeof(*allstates[states].instructions[instrs].newstate),iter*sizeof(*allstates[states].instructions[instrs].newstate));
        //        allstates[states].instructions[instrs].newstate = realloc(allstates[states].instructions[instrs].newstate, iter*sizeof(*allstates[states].instructions[instrs].newstate));
        //        allstates[states].instructions[instrs].newstate[iter-1] = *readchar;
        //        readchar++;
        //    }
        //}
    }
    if (verbose) printf("\n\n");
    printf("There are %d states\n",states);
    printf("There are %d insructions per state\n", instrs);
    printf("Name of Set [0]: '%s'\n", allstates[0].title); /* f -- THIS IS WORKING */
    printf("Name of Set [1]: '%s'\n", allstates[1].title); /* b -- THIS IS WORKING */
    printf("Name of Set [2]: '%s'\n", allstates[2].title); /* F -- THIS IS WORKING */
    printf("Name of Set [3]: '%s'\n", allstates[3].title); /* B -- THIS IS WORKING */
    //printf("New value of instruction [0] in set [0]: %c\n", allstates[0].instructions[0].newval);
    //printf("Instruction [2] at Set [1]: %c\n", allstates[1].instructions[2]); /* 25LB */
    return 0;
}

int freevars() {
    /* it might be good practice to free all the parts of allstates */
    free(allstates);
    return 0;
}