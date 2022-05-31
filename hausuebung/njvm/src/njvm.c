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
unsigned int *allInstruct;
unsigned int *allVariable;
unsigned int pc = 0;
int rv = 0;
int immediate = 0;
int res;
/** todo list
 * 1)  run instruction testen
 * 2) execute testen
 *  3) pushl und pushg popg popl testen
 *  4) implementieren callInstruction , andere methode drop,
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

void execute(unsigned int IR, int imm) {
    int a, b;
    int old_pc;

    switch (IR) {
        case HALT://HALT
            break;
        case PUSHC://PUSHC
            push(imm);
            break;
        case ADD://ADD
            add();
            break;
        case SUB://SUB
            sub();
            break;
        case MUL://MUL
            mul();
            break;
        case DIV://DIV
            divid();
            break;
        case MOD://MOD
            mod();
            break;
        case PUSHG: //PUSHG
            // push n element from static data area to Stack
            push(sda[imm]);
            break;
        case POPG:
            //popg <n> → ... value -> ... - Der Wert value wird in der SDA als n-tes Element
            //gespeichert
            // pop element from stack
            // push a element in position n in SDA
            a = pop();
            sda[imm] = a;
            break;
        case ASF://ASF 13
            //asf <n> Allocate Stack Frame — n gibt die Anzahl der zu reservierenden lokalen Variablen an
            asf(imm);
            break;
        case RSF:
            rsf();
            break;
        case PUSHL:
            pushl(imm);
            break;
        case POPL:
            popl(imm);
            break;
        case EQ:
            equal();
            break;
        case NE:
            nequal();
            break;
        case LT:
            lessThan();
            break;
        case LE:
            lessEqual();
            break;
        case GT:
            greaterThan();
            break;
        case GE:
            greaterEqual();
            break;
        case JMP:
            pc = jump(imm);
            break;
        case BRF:
            b = pop();
            if (b == 0)
                pc = imm;
            break;
        case BRT:
            b = pop();
            if (b == 1)
                pc = imm;
            break;
        case RDINT:
            readInt();
            break;
        case WRINT:
            res = writeInt();
            break;
        case RDCHR:
            readChar();
            break;
        case WRCHR:
            res = writeChar();
            break;
        case CALL:
            //callInstruction(immediate, pc);
            old_pc = pc; //frptr
            push(old_pc);
            pc = imm;
            break;
        case RET:
            pc = pop();
            break;
        case DROP: {
            int k = 0;
            while (k < imm) {
                pop();
                ++k;
            }
        }
            break;
        case PUSHR:
            push(rv);
            break;
        case POPR:
            rv = pop();
            break;
        case DUP:
            // sp +1
            // lege ich da die gleiche variable
            a = pop();
            push(a);
            push(a);
            break;
/*        case NEW:
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
            break;*/
        default:
            break;
    }
}

void printExecute(unsigned int instruct) {
    immediate = SIGN_EXTEND(IMMEDIATE(instruct));
    IR = (instruct >> 24);
    switch (IR) {
        case HALT:
            printf("halt\n");
            break;
        case PUSHC:
            printf("pushc\t%d\n", immediate);
            break;
        case ADD:
            printf("add\n");
            break;
        case SUB:
            printf("sub\n");
            break;
        case MUL:
            printf("mul\n");
            break;
        case DIV:
            printf("div\n");
            break;
        case MOD:
            printf("mod\n");
            break;
        case PUSHG:
            printf("pushg\t%d\n", immediate);
            break;
        case POPG:
            printf("popg\t%d\n", immediate);
            break;
        case ASF:
            printf("asf\t%d\n", immediate);
            break;
        case RSF:
            printf("rsf\n");
            break;
        case PUSHL:
            printf("pushl\t%d\n", immediate);
            break;
        case POPL:
            printf("popl\t%d\n", immediate);
            break;
        case EQ:
            printf("eq\n");
            break;
        case NE:
            printf("neq\n");
            break;
        case LT:
            printf("lt\n");
            break;
        case LE:
            printf("le\n");
            break;
        case GT:
            printf("qt\n");
            break;
        case GE:
            printf("qe\n");
            break;
        case JMP:
            printf("jmp\t%d\n", immediate);
            break;
        case BRF:
            printf("brf\t%d\n", immediate);
            break;
        case BRT:
            printf("brt\t%d\n", immediate);
            break;
        case RDINT:
            printf("rdint\n");
            break;
        case WRINT:
            printf("wrint\n");
            break;
        case RDCHR:
            printf("rdint\n");
            break;
        case WRCHR:
            printf("wrchr\t%c\n", writeChar());
            break;
        case CALL:
            printf("call\t%d\n", immediate);
            break;
        case RET:
            printf("ret\t%d\n", pc);
            break;
        case DROP:
            printf("drop\t%d\n", immediate);
            break;
        case PUSHR:
            printf("pushr\t%d\n", rv);
            break;
        case POPR:
            printf("popr\t%d\n", rv);
            break;
        case DUP:
            printf("dup\n");
            break;
        case NEW:
            printf("new\n");
            break;
        case GETF:
            printf("getf\n");
            break;
        case PUTF:
            printf("putf\n");
            break;
        case NEWA:
            printf("newa\n");
            break;
        case GETFA:
            printf("getfa\n");
            break;
        case PUTFA:
            printf("putfa\n");
            break;
        case GETSZ:
            printf("getsz\n");
            break;
        case PUSHN:
            printf("pushn\n");
            break;
        case REFEQ:
            printf("refeq\n");
            break;
        case REFNE:
            printf("refne\n");
            break;
        default:
            printf("Error: Unknown opcode %d\n", IR);
            break;
    }
}

void RunInstructionToAssemble(unsigned int programMemory[]) {
    pc = frptr = sp = 0;
    IR = 200;
    while (IR != HALT && pc < numberOfInstructions) {// HALT
        printf("%03d:\t", pc);
        unsigned int value = programMemory[pc];
        immediate = SIGN_EXTEND(IMMEDIATE(value));
        IR = (value >> 24);

        printExecute(programMemory[pc]);
        execute(IR, immediate);
        pc++;
    }
    if (programMemory[pc == 0]) {
        execute(programMemory[pc], immediate);
        printf("%03d:\thalt\t\n", pc);
    }
    printf("%d\n", res);
    //print_stack();
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
            //callInstruction program , option
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
    printf("Ninja Virtual Machine started\n");
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
    //TODO: Version always under Observation
    if (version < 2) {
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
    printf("Ninja Virtual Machine stopped\n");
    return 0;
}