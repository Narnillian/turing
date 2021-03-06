#include <stdio.h>
#include <stdlib.h>
//#include <sys/ioctl.h> /* IO control */
//#include <termios.h>   /* terminal interface */
typedef struct square square;

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
    -(Unlimited number of states, instructions per state -- as far as RAM allows)
    -Pipe symbol -- splits States from Tape
    -Tape
        -Aterisk indicates initial head position

*/

struct {
    char title[3]; /* direct title of state -- can be as long as you want, above length 0 */
    char *nickname; /*used for longer, more descriptive names for the state -- haven't done yet */
    
    struct { /* one instruction */
        char halt; /* positive value if instruction is HALT */
        char newval; /* write value of head */
        char movehead; /* based on moving right -- positive is right, negative is left, 0 is neither. will only move one position no matter the value (unless neither) */
        char newstate[3];
    } *instructions;

} *allstates = NULL; /* global list of all states */

struct square { /* one square of the tape */
    char value;
    square *prev;
    square *next;

};

void *n_realloc(void * ptr, size_t size);
int loadinstrs();
int freevars();

char verbose = 0;
char *preset1 = "(f;4Lf,4Lf,3Rf,2Lf,0Rf,0Rb)(*b;5LB,0Rb,5LB,4Rb,H,2Rb)(F;5Rf,1RF,4Rb,2RF,H,1Rf)(B;1LB,0RB,3LF,4RB,3LB,2RB)|00000005155*0000000";
char *preset2 = NULL; /*        these will be made eventually                   */
char *preset3 = NULL; /*        there will be preset machines                   */
char *fullspec = NULL; /*       machine we are using                            */
int actstate = 0; /*            active state                                    */
//int values; /*                  how many values a single tape square can have   */


int main(int argc, char **argv) {
    if (argv[1] && *argv[1] == 'v') verbose = 1; /* this is temp */
    fullspec = preset1; /* this is temp */
    printf("Instruction loading:\n\n");
    loadinstrs();
    freevars();
    return 0;
}

void *n_realloc(void *ptr, size_t size) {
    void *tmp = realloc(ptr,size);
    if (!tmp) {
        printf("YOU HAD A PROBLEM");
        exit(55);
    } else return tmp;
}


int loadinstrs() {
    char *split = NULL;
    int states = 0;
    int instrs = 0;
    char instrpart;
    char *readchar = fullspec;

    while (readchar) {
        printf("\033[46m\"%c\"\033[0m\n",*readchar);

        /* end of instructions */
        if (*readchar == '|') {
            printf("PIPE-CHAR!\n");
            split = readchar;
            printf("\033[100m%p\033[0m\n",allstates[states]);
            break;
        }


        /* new set */
        printf("OPEN-PAREN\n");
        if (verbose) {
            printf(">%d\n>%d\n", sizeof(*allstates), (states+1)*sizeof(*allstates));
            printf("There are \033[46m%d instruction sets\033[0m\n", states+1);
        }
        allstates = n_realloc(allstates, (states+1)*sizeof(*allstates));
        allstates[states].instructions = NULL;
        allstates[states].instructions = n_realloc(allstates[states].instructions, (instrs+1)*sizeof(*allstates[states].instructions));
        if (*(readchar+1)=='*') {
            actstate = states;
            readchar++;
        }
        {
            int iter = 0;
            while (*++readchar != ';') {
                printf("TITLE --- \033[102m\"%c\"\033[0m\n",*readchar);
                if (iter < 3) {
                    allstates[states].title[iter++] = *readchar;
                    printf("\tFULL TITLE of %d --- %s\n",states, allstates[states].title);
                }
            }
        }
        readchar++;
        instrs = 0;


        /* the actual reading of instructions */
        while (1) {
            /* get instruction */
            printf("DEFAULT\n");
            printf("%d || %d\n",states,instrs);
            if (*readchar=='H') {   
                allstates[states].instructions[instrs].halt = 1;
                continue;
            }
            allstates[states].instructions[instrs].newval = readchar[0];
            readchar++;
            //printf("\033[43mnv: \"%c\"\033[0m\n",allstates[states].instructions[instrs].newval);
            //printf("\033[43mrc: \"%c\"\033[0m\n",*readchar);
            switch(*readchar++) {
                case 'L':
                    printf("LEFT\n");
                    allstates[states].instructions[instrs].movehead = -1;
                    break;
                case 'R':
                    printf("RIHT\n");
                    allstates[states].instructions[instrs].movehead = 1;
                    break;
                default:
                    printf("NONE\n");
                    allstates[states].instructions[instrs].movehead = 0;
            }
            {
                int iter = 0;
                allstates[states].instructions[instrs].newstate[0] = 0;
                allstates[states].instructions[instrs].newstate[1] = 0;
                allstates[states].instructions[instrs].newstate[2] = 0;
                do {
                    printf("NSTATE --- \033[102m\"%c\"\033[0m\n",*readchar);
                    if (iter < 3) {
                        allstates[states].instructions[instrs].newstate[iter++] = *readchar;
                        printf("FULL NEWSTATE of %d,%d --- %s\n",states, instrs, allstates[states].instructions[instrs].newstate);
                    }
                    readchar++;
                } while (*(readchar) != ',' && *(readchar) != ')');
                //printf("\033[100m%s\033[0m",*readchar);
            }
            /* if there's another instruction, realloc and continue */
            if (*readchar == ',') {
                printf("COMMA-SEPER\n");
                /* end of instruction */
                if (verbose) {
                    printf("There are \033[42m%d instructions per state\033[0m\n", instrs+1);
                    printf("^%d\n^%d\n", sizeof(*allstates[states].instructions), (instrs+1)*sizeof(*allstates[states].instructions));
                    printf("\033[101mBEFOR)%p\033[0m\n",allstates[states].instructions);
                }
                //allstates[states].instructions = NULL;
                allstates[states].instructions = n_realloc(allstates[states].instructions, (instrs+1)*sizeof(*allstates[states].instructions));
                if (verbose) printf("\033[41m%p\033[0m\n",allstates[states]);
                instrs++;
                readchar++;
                continue;
            }
            readchar++;
            break;
        }
        printf("CLOSE-PAREN\n");
        states++;
        if (verbose) printf("There are \033[42m%d instructions per state\033[0m\n", ++instrs);
        readchar++;

    }
    if (verbose) printf("\n\n");
    printf("There are %d states\n",states);
    printf("There are %d insructions per state\n", instrs);
    printf("Name of Set [0]: '%s'\n", allstates[0].title); /* f -- THIS IS WORKING */
    printf("Name of Set [1]: '%s'\n", allstates[1].title); /* b -- THIS IS WORKING */
    printf("Name of Set [2]: '%s'\n", allstates[2].title); /* F -- THIS IS WORKING */
    printf("Name of Set [3]: '%s'\n", allstates[3].title); /* B -- THIS IS WORKING */
    printf("New value of instruction [0] in set [0]: %c\n", allstates[0].instructions[0].newval);
    printf("New state of instruction [5] in set [2]: %c\n", allstates[2].instructions[5].newstate);
    //printf("Instruction [2] at Set [1]: %c\n", allstates[1].instructions[2]); /* 5LB */
    return 0;
}

int freevars() {
    /* it might be good practice to free all the parts of allstates */
    free(allstates);
    return 0;
}