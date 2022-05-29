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
int *sda; //static Data Area
int version;
unsigned int IR = 0;
int numberOfInstructions;
int numberOfVariables;
int sdaIndex = 0;
int pc = 0;
unsigned int *allInstruct;
unsigned int *allVariable;
int rv = 0;
int immediate = 0;
int *progMemory;
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

void execute(unsigned int instruct) {
    int a, b;
    immediate = SIGN_EXTEND(IMMEDIATE(instruct));
    IR = (instruct >> 24);
    switch (IR) {
        case HALT://HALT
            printf("halt\n");
            break;
        case PUSHC://PUSHC
            push(immediate);
            printf("pushc %d\n", immediate);
            break;
        case ADD://ADD
            add();
            printf("add\n");
            break;
        case SUB://SUB
            sub();
            printf("sub\n");
            break;
        case MUL://MUL
            mul();
            printf("mul\n");
            break;
        case DIV://DIV
            divid();
            printf("div\n");
            break;
        case MOD://MOD
            mod();
            printf("mod\n");
            break;
        case PUSHG: //PUSHG
            // push n element from static data area to Stack
            push(sda[immediate]);
            printf("pushg %d\n", immediate);
            break;
        case POPG:
            //popg <n> → ... value -> ... - Der Wert value wird in der SDA als n-tes Element
            //gespeichert
            // pop element from stack
            // push a element in position n in SDA
            a = pop();
            sda[immediate] = a;
            printf("popg %d\n", immediate);
            break;
        case ASF://ASF 13
            //asf <n> Allocate Stack Frame — n gibt die Anzahl der zu reservierenden lokalen Variablen an
            asf(immediate);
            printf("asf %d\n", immediate);
            break;
        case RSF:
            rsf();
            printf("rsf\n");
            break;
        case PUSHL:
            pushl(immediate);
            printf("puschl %d\n", immediate);
            break;
        case POPL:
            popl(immediate);
            printf("puschl %d\n", immediate);
            break;
        case EQ://eq
            equal();
            printf("eq\n");
            break;
        case NE://ne
            nequal();
            printf("neq\n");
            break;
        case LT://lt
            lessThan();
            printf("lt\n");
            break;
        case LE://le
            lessEqual();
            printf("le\n");
            break;
        case GT://gt
            greaterThan();
            printf("qt\n");
            break;
        case GE://ge
            greaterEqual();
            printf("qe\n");
            break;
        case JMP://jmp <target>
            pc = jump(immediate);
            printf("jmp %d\n", immediate);
            break;
        case BRF://brf <target>
            b = pop();
            if (b == 0) pc = immediate;
            printf("brf %d\n", immediate);
            break;
        case BRT://brt <target>
            b = pop();
            if (b == 1) pc = immediate;
            printf("brt %d\n", immediate);
            break;
        case RDINT:
            readInt();
            printf("rdint\n");
            break;
        case WRINT:
            writeInt();
            printf("wrint\n");
            break;
        case RDCHR:
            readChar();
            printf("rdint\n");
            break;
        case WRCHR:
            writeChar();
            printf("wrchr\n");
            break;
        case CALL:
            call(immediate, pc);
            printf("call %d\n", immediate);
            break;
        case RET:
            pc = pop();
            printf("ret %d\n", pc);
            break;
        case DROP: {
            int k = 0;
            while (k < immediate) {
                pop();
                ++k;
            }
        }
            printf("drop %d\n", immediate);
            break;
        case PUSHR:
            push(rv);
            printf("pushr %d\n", rv);
            break;
        case POPR:
            rv = pop();
            printf("popr %d\n", rv);
            break;
        case DUP:
            // sp +1
            // lege ich da die gleiche variable
            a = pop();
            push(a);
            push(a);
            printf("dup\n");
            break;
        case NEW:
            break;
        case GETF:
            break;
        case PUTF:
            break;
        case NEWA:
            break;
        case GETFA:
            break;
        case PUTFA:
            break;
        case GETSZ:
            break;
        case PUSHN:
            break;
        case REFEQ:
            break;
        case REFNE:
            break;
        default:
            printf("Error: Unknown opcode %d\n", IR);
            break;
    }
}


void RunInstructionToAssemble(unsigned int programMemory[]) {

    while (programMemory[pc] != 0) {// HALT
        printf("%03d\t: ", pc);
        execute(programMemory[pc]);
        pc++;
    }
    printf("halt\n");
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
    char *fileName;
    fileName = argv[1];
    filePointer = fopen(fileName, "r");

    //printf("after fopen() before if NULL \n");
    if (filePointer == NULL) {
        printf("Error: cannot open code file '%s'\n", fileName);
        exit(1);
    }
    printf("Virtual Machine started\n");
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