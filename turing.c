#include <stdio.h>
#include <stdlib.h>
//#include <sys/ioctl.h> /* IO control */
//#include <termios.h>   /* terminal interface */
typedef struct square square;
size_t strlen(const char *s);
int strcmp(const char *s1, const char *s2);


/*
Machine organization:
(f;4Lf,4Lf,3Rf,2Lf,0Rf,0Rb)(*b;5LB,0Rb,5LB,4Rb,H,2Rb)(F;5Rf,1RF,4Rb,2RF,H,1Rf)(B;1LB,0RB,3LF,4RB,3LB,2RB)|00000005155*0000000
    -One State is encapsulated by parenthesis
    -If there's an asterisk, that's the initial State
    -The next characters until the semicolon is the State Shorthand
    -Instructions, seperated by commas:
        -Character to change tape[head] to
        -Direction to move -- exactly one square (R)ight, (L)eft, or (N)either
        -New active state -- indicated by State Shorthand
    Or:
        -Halt
    -(Unlimited number of states, instructions per state -- as far as RAM allows)
    -Pipe symbol -- splits States from Tape
    -Tape
        -Aterisk indicates initial head position

*/

struct {
    char title[3]; /* direct title of state -- up to 3 chars */
    char *nickname; /*used for longer, more descriptive names for the state -- haven't done yet */
    
    struct { /* one instruction */
        unsigned int halt:1; /* HALT = 1, !HALT = 0 */
        unsigned int movehead:2; /* 01 means RIGHT, 11 means LEFT, 00 means NONE */
        char newval; /* value to write at HEAD */
        char newstate[3]; /* title of new interperative state to change to */
    } *instructions;

} *allstates = NULL; /* global list of all states */

struct square {
    char value;
    square *left;
    square *right;

} *tape = NULL;

void *n_realloc(void * ptr, size_t size);
int loadinstrs();
int loadtape();
char *tapetostring();
int runmachine();
int freevars();

u_int8_t verbose = 0;
char *preset1 = "(f;4Lf,4Lf,3Rf,2Lf,0Rf,0Rb)(*b;5LB,0Rb,5LB,4Rb,H,2Rb)(F;5Rf,1RF,4Rb,2RF,H,1Rf)(B;1LB,0RB,3LF,4RB,3LB,2RB)|00000005155*0000000";
char *preset2 = NULL; /*          these will be made eventually (base10 adder?) */  /* i may want to make them external files, so it doesn't take up too much */
char *preset3 = NULL; /*          there will be preset machines (base2  adder?) */  /* memory, and other people could use the special features i have planned */
char *fullspec = NULL; /*       machine we are using                            */
int actstate = 0; /*            active state                                    */
square *head = NULL; /*         which square the head is pointing to on the tape*/
char **statenames = NULL;
int    *stateindex = NULL;


int main(int argc, char **argv) {
    if (argv[1] && *argv[1] == 'v') verbose = 1; /* this is temp */
    //if (argv[1] && (strlen(argv[1]) > 1) && argv[1] == "vv") xverbose = (verbose = 1); /* this is wrong */ //screw this
    fullspec = preset1; /* this is temp */
    //printf("Instruction loading:\n\n");
    loadinstrs();
    loadtape();
    runmachine();
    freevars();
    return 0;
}


void *n_realloc(void *ptr, size_t size) {
    void *tmp = realloc(ptr,size);
    if (!tmp) {
        printf("YOU HAD A PROBLEM REALLOCING");
        exit(55);
    } else return tmp;
}


int loadinstrs() {
    int states = 0;
    int instrs = 0;
    char *readchar = fullspec;

    while (*readchar != '|') {
        /* new state */
        if (*readchar == '(') {
            allstates = n_realloc(allstates, (states+1)*sizeof(*allstates));
            allstates[states].instructions = NULL;
            allstates[states].instructions = n_realloc(allstates[states].instructions, (instrs+1)*sizeof(*allstates[states].instructions));
            
            statenames = n_realloc(statenames, states+1);                /* */ 
                statenames[states] = NULL;                              /* */
                statenames[states] = n_realloc(statenames[states], 3); /* */
                statenames[states] = "\0\0\0";                        /* */
            stateindex = n_realloc(stateindex, states+1);
                stateindex[states] = 0;
        } else {
            printf("MACHINE NOT CONFIGURED PROPERLY\n");
            exit(99);
        }
        /* active state */
        if (*(readchar+1)=='*') {
            actstate = states;
            readchar++;
        }
        { /* state title */
            int iter = 0;
            while (*++readchar != ';') {
                if (iter < 3) {
                    allstates[states].title[iter++] = *readchar;
                }
            }


            /* find place to insert new state */
            int insert = 0;
            int midpoint = 0; /* as is: runs through the loop once to get to the real midpoint. change it to `int midpoint = states/2` to reduce that one loop */
            int prevmid = states;
            if (states != 0) while (1) {
                if (strcmp(statenames[midpoint],allstates[states].title) > 0 && strcmp(statenames[midpoint-1],allstates[states].title) < 0) {
                    printf("found\n");
                    insert = midpoint;
                    break;
                }
                if (strcmp(statenames[midpoint],allstates[states].title) < 0) /* statenames[midpoint] is alphabetically before allstates[states].title */ {
                    printf("before\n");
                    midpoint/=2;
                    continue;
                }
                if (strcmp(statenames[midpoint],allstates[states].title) > 0) /* statenames[midpoint] is alphabetically after  allstates[states].title */ {
                    printf("after\n");
                    int tmp = midpoint;
                    midpoint = prevmid+((states-prevmid)/2);
                    prevmid = tmp;
                    continue;
                }
            }
            /* insert new state */
            

            //char hold[3];
            //int i = 0;
            //do {
            //    char tmp[3];
            //    for (int j=0;j<3;j++) { /* tmp = *statenames[midpoint+i]; */
            //        tmp[j]=statenames[midpoint+i+j];
            //    }
            //    statenames[midpoint+i] = hold;
            //    *hold = *tmp;
            //    i++;
            //} while (statenames[i]);

            //printf("%d ~ %d\n",states,sizeof(statenames));
            //char tmp[3];
            ///* i have a basic idea. flesh it out on paper */
            //for (int i = 0; *statenames[i]; i++) {
            //    if (strcmp(statenames[i],allstates[states].title) < 0) { /* statenames[i] is alphabetically before allstates[states].title */
            //        *tmp = *statenames[i];
            //        //statenames[i]

            //    }
            //}

        }
        readchar++;
        instrs = 0;


        /* read instructions */
        while (1) {
            /* HALT */
            if (*readchar=='H') {   
                allstates[states].instructions[instrs].halt=1;
                instrs++;
                /* end of instruction */
                allstates[states].instructions = n_realloc(allstates[states].instructions, (instrs+1)*sizeof(*allstates[states].instructions));
                readchar+=2;
                continue;
            }
            allstates[states].instructions[instrs].newval = *readchar++;

            /* head move direction */
            switch(*readchar++) {
                case 'L':
                    allstates[states].instructions[instrs].movehead = 3;
                    break;
                case 'R':
                    allstates[states].instructions[instrs].movehead = 1;
                    break;
                default:
                    allstates[states].instructions[instrs].movehead = 0;
            }
            /* set new head state */
            {
                int iter = 0;
                allstates[states].instructions[instrs].newstate[0] = 0;
                allstates[states].instructions[instrs].newstate[1] = 0;
                allstates[states].instructions[instrs].newstate[2] = 0;
                do {
                    if (iter < 3) {
                        allstates[states].instructions[instrs].newstate[iter++] = *readchar;
                    }
                    readchar++;
                } while ((*(readchar) != ',') && (*(readchar) != ')'));
            }
            /* if there's another instruction, realloc and continue */
            if (*readchar == ',') {
                instrs++;
                /* end of instruction */
                allstates[states].instructions = n_realloc(allstates[states].instructions, (instrs+1)*sizeof(*allstates[states].instructions));
                readchar++;
                continue;
            }
            break;
        }
        states++;
        instrs++;
        readchar++;

    }
    fullspec = readchar;

    if (verbose) {
        printf("There are %d states\n",states);
        printf("There are %d insructions per state\n", instrs);
        printf("SIZE: %d\n",sizeof(allstates[0].title));
        printf("ACTIVE: %d\n",states);
        printf("Name of Set [0]: '%s'\n", allstates[0].title); /* f */
        printf("Name of Set [1]: '%s'\n", allstates[1].title); /* b */
        printf("Name of Set [2]: '%s'\n", allstates[2].title); /* F */
        printf("Name of Set [3]: '%s'\n", allstates[3].title); /* B */
        printf("New value of instruction [0] in set [0]: %c\n", allstates[0].instructions[0].newval);
        printf("New state of instruction [5] in set [2]: %c\n", allstates[2].instructions[5].newstate);
    }
    return 0;
}


int loadtape() {
    char *readchar = fullspec;
    int squares = 0;
    readchar++;

    while (*readchar) {
        //printf("%c=>%d\n",*readchar,squares);
        tape = n_realloc(tape,(squares+1)*sizeof(tape[0]));
        if (*readchar == '*') {
            readchar++;
            head = &tape[squares];
            continue; /* this is just to make the printf debug make more sense. not necessary */
        }
        tape[squares].value = *readchar;
        tape[squares].right = NULL;
        if (squares == 0) {
            tape[squares].left = NULL;
        } else {
            tape[squares]  .left  = &tape[squares-1];  /* only works for setting up the machine because we are loading all the squares */
            tape[squares-1].right = &tape[squares];    /* in order. later we will have to find a different way to set .left and .right */
        }
        readchar++;
        squares++;
        
    }
           char *tapestring;
    printf("%s\n", tapestring = tapetostring()); free(tapestring);/* not permanent */

    return 0;

}

char *tapetostring() {
    square *square = &tape[0];
    char *string = malloc((size_t)1);
    int length = 1;
    //printf("\n\n~~\n1)%p\n2)%d\n",tape,sizeof(tape));
    while (square->left != NULL) {
        square = square->left;
        //printf("3)%p\n",square);
    }
    //printf("3)%p\n",square);
    while (1) {
        string = n_realloc(string, length+1);
        //printf("<%d||%c\n",length,square->value);
        string[length-1] = square->value;
        //if (square->right != NULL) {square = square->right;} else break;
        square = square->right ? square->right : NULL; if (!square) break;
        length++;
    }

    return string;
}


int runmachine() {
    //while (allstates[actstate].instructions->halt != 1) {
    //    head->value = allstates[actstate].instructions[atoi(&head->value)].newval;
    //    /* fix actstate's design, then put other stuff here */

    //}
    return 0;
}

int freevars() {
    /* it might be good practice to free all the parts of allstates */
    free(allstates);
    free(tape);
    free(statenames);
    free(stateindex);
    return 0;
}