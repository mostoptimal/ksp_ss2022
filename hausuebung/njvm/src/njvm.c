#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "stack.h"
#include "execute.h"
#include "programs.h"
#include "opCodes.h"
#include "instructions.h" //Contains VM Instructions

#define DEBUG

char *runOption;
FILE *filePointer;
unsigned int instruction;
int *sda; //static Data Area
int version;
unsigned int IR = 0;
int numberOfInstructions;
int numberOfVariables;
int sdaIndex = 0;
int pc = 0;
unsigned int *allInstruct;
unsigned int *allVariable;
int rv=0;
int immediate=0;
/** todo list
 * 1)  run instruction testen
 * 2) execute testen
 *  3) pushl und pushg popg popl testen
 *  4) implementieren call , andere methode drop,
 *  5) test die ausgabe von aufgabe 2 ,3 ,4
 *  6) debbug implementieren als option ,
   */

/*
 * */
void pushg(int index) {
    int value = sda[index];
    push(value);
}

void popg(int index) {
    int value = pop();
    sda[index] = value;
}

/**
 * */
void execute(int instruct) {
    int a, b;
    immediate= SIGN_EXTEND(instruct);
    IR = (instruct >> 24); /* 1. musn't shift*/

    switch (IR) {
        case HALT://HALT
            break;
        case PUSHC://PUSHC
            push(immediate);
        case ADD://ADD
            add();
        case SUB://SUB
            sub();
        case MUL://MUL
            mul();
        case DIV://DIV
            divid();
        case MOD://MOD
            mod();
        case PUSHG: //PUSHG
            // push n element from static data area to Stack
            push(sda[immediate]);

        case POPG:
            //popg <n> → ... value -> ... - Der Wert value wird in der SDA als n-tes Element
            //gespeichert
            // pop element from stack
            // push a element in position n in SDA
            a = pop();
            sda[immediate] = a;

        case ASF://ASF 13
            //asf <n> Allocate Stack Frame — n gibt die Anzahl der zu reservierenden lokalen Variablen an
            asf(immediate);
        case RSF:
            rsf();
        case PUSHL:
            pushl(immediate);
        case POPL:
            popl(immediate);
        case EQ://eq
            equal();
        case NE://ne
            nequal();
        case LT://lt
            lessThan();
        case LE://le
            lessEqual();
        case GT://gt
            greaterThan();
        case GE://ge
            greaterEqual();
        case JMP://jmp <target>
            pc = jump(immediate);
        case BRF://brf <target>
            b = pop();
            if (b == 0) pc = immediate;
        case BRT://brt <target>
            b = pop();
            if (b == 1) pc = immediate;
        case RDINT:
            readInt();
        case WRINT:
            writeInt();
        case RDCHR:
            readChar();
        case WRCHR:
            writeChar();
        case CALL:
            call(immediate, pc);
        case RET:
            pc = pop();
        case DROP: {
            int k = 0;
            while (k < immediate) {
                pop();
                ++k;
            }
        }
        case PUSHR:
            push(rv);
        case POPR:
            rv=pop();
        case DUP:
            // sp +1
            // lege ich da die gleiche variable
            a = pop();
            push(a);
            push(a);
        case NEW:
        case GETF:
        case PUTF:
        case NEWA:
        case GETFA:
        case PUTFA:
        case GETSZ:
        case PUSHN:
        case REFEQ:
        case REFNE:
        default:
            printf("Error: Unknown opcode %d\n", IR);
    }
}


void RunInstructionToAssemble(unsigned int programMemory[]) {
    int i = 0;

    while (programMemory[i] != 0) {// HALT

        execute(programMemory[i]); //TODO 2nd Parameter
        i++;
        pc++;
    }
    print_stack();
}

void readInputInTerminal(int argc, char *argv[]) {
    //read info for data
    //argumente
    //  char allarguments [argc];
    if (argc == 0) {
        //printf("u dont have any parameter");
    }
    for (int k = 1; k < argc; k++) {
        //
        if (strcmp(argv[k], "--debug") == 0) {
            //call program , option
            runOption = "debug";
            char debugOption = 0;

            scanf("%c", &debugOption);

            switch (debugOption) {

                case 'i':
                    break;

                case 'l':
                    break;

                case 'b':
                    break;

                case 's':
                    break;

                case 'r':
                    break;

                case 'q':
                    break;

                default:
                    break;

            }
        }
        //  allarguments[i] = argumente[i];
    }
    //option  filename
    // OPTION  -- debug , --run

}

//-------------------------------------------------------------
int main(int argc, char *argv[]) {
    printf("Virtual Machine started\n");

    char *fileName;
    fileName = argv[1];

    filePointer = fopen(fileName, "r");

    //printf("after fopen() before if NULL \n");
    if (filePointer == NULL) {
        printf("Error: cannot open code file '%s'\n", fileName);
        exit(1);
    }

    /** 1st Step: read the format of Binary File "NJBF" */
    //1) Read the first 4 bytes of the file.
    char *njbf = (char *) malloc(sizeof(char) * 4);
    fread(njbf, sizeof(char), 4, filePointer);
    if (strcmp(njbf, "NJBF") != 0) {
        //printf("Format is NOT Correct :) \n");
        exit(1);
    }

    /** 2nd Step: Read the version number. **********/
    //2) Read the version number.
    fread(&version, sizeof(int), 1, filePointer);
    if (version != 2) {
        printf("Error: file %s has wrong version number %d\n", fileName, version);
        exit(1);
    }

    /** 3rd Step: Read the Number of Instructions */
    // 3) Read the number of instructions.
    fread(&numberOfInstructions, sizeof(int), 1, filePointer);
    allInstruct = malloc(numberOfInstructions * sizeof(unsigned int));

    /** 4th Step: read the Number of Global-Variables in SDA */
    //4) Read the number of variables in the static data area.
    fread(&numberOfVariables, sizeof(int), 1, filePointer);
    if (numberOfVariables > 0) {
        //SDA Allocate for Public Variables
        sda = malloc(numberOfVariables * sizeof(int));
    }

    /**5th Step: reading the rest of File */
    //5) Read the rest of the file into the memory allocated in step 3).
    fread(allInstruct, sizeof(unsigned int), numberOfInstructions, filePointer);
    RunInstructionToAssemble(allInstruct);
    /**6th Step: Closing the File */
    if (fclose(filePointer) != 0) {
        perror("ERROR (fclose)");
    }
    printf("Virtual Machine stopped\n");
    return 0;
}