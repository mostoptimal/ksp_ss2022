#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "stack.h"

#define HALT 0
#define PUSHC 1
#define ADD 2
#define SUB 3
#define MUL 4
#define DIV 5
#define MOD 6
#define RDINT 7
#define WRINT 8
#define RDCHR 9
#define WRCHR 10
#define PUSHG 11
#define POPG 12
#define ASF 13
#define RSF 14
#define PUSHL 15
#define POPL 16
#define EQ 17
#define NE 18
#define LT 19
#define LE 20
#define GT 21
#define GE 22
#define JMP 23
#define BRF 24
#define BRT 25
#define CALL 26
#define RET 27
#define DROP 28
#define PUSHR 29
#define POPR 30
#define DUP 31
#define SIGN_EXTEND(i) ((i)&0x00800000 ? (i) | 0xFF000000 : (i))
#define IMMEDIATE(x) ((x)&0x00FFFFFF)

FILE *file;
char *fileName;
int version;
int *programMemory;
int *sda;
int pc = 0;
unsigned int counter = 0;
int fp = 0;
int numberOfInstructions;
int numberOfVariables;
int returnValue;

int a, b, c;

void halt() {
    exit(0);
}

void add() {
    a = pop();
    b = pop();
    c = a + b;
    push(c);
}

void sub() {
    a = pop();
    b = pop();
    c = b - a;
    push(c);
}

void mul() {
    a = pop();
    b = pop();
    c = a * b;
    push(c);
}

void divid() {
    a = pop();
    b = pop();
    c = b / a;
    push(c);
}

void mod() {
    a = pop();
    b = pop();
    c = b % a;
    push(c);
}

void readInt() {
    int eingabe;
    scanf("%d", &eingabe);
    push(eingabe);
}

void wrint() {
    a = pop();
    printf("%d", a);
}

void rdchr() {
    char eingabe;
    scanf("%c", &eingabe);
    push(eingabe);
}

void wrchr() {
    char output = pop();
    printf("%c", output);
}

void pushg(int element) {

    int val = sda[element];
    //printf("pushg %d\n", val);
    push(val);
}

void popg(int element) {

    int val = pop();
    //printf("popg %d\n", val);
    sda[element] = val;
}
//Allocate Release:
void asf(int n) {
    int old_fp = fp;
    push(old_fp);
    fp = sp;
    sp = sp + n;
}
void rsf() {
    sp = fp;
    fp = pop();
}
//PUSHL POPL
void pushl(int lokVar) {
    // push to stack local variable
    push(stack[fp + lokVar]);
}
void popl(int lokVar) {
    stack[fp + lokVar] = pop();
}
//Logic
void equal() {
    a = pop();
    b = pop();
    if (a == b) {
        push(1);
    } else {
        push(0);
    }
}
void nequal() {
    a = pop();
    b = pop();
    if (a != b) {
        push(1);
    } else {
        push(0);
    }
}
void lessThan() {
    a = pop();
    b = pop();
    if (b > a) {
        push(1);
    } else {
        push(0);
    }
}
void lessEqual() {
    a = pop();
    b = pop();
    if (b <= a) {
        push(1);
    } else {
        push(0);
    }
}
void greaterThan() {
    a = pop();
    b = pop();
    if (b > a) {
        push(1);
    } else {
        push(0);
    }
}
void greatEqual() {
    a = pop();
    b = pop();
    if (b >= a) {
        push(1);
    } else {
        push(0);
    }
}
void branchIfTrue(int a) {
    b = pop();
    if (b == 1) pc = a;
}
void branchIfFalse(int x) {
    b = pop();
    if (b == 0) pc = x;
}
//jump
void jmp(int x) {
    pc = x;
}
//call & return
void call(int x) {
    push(pc);
    pc = x;
}
void ret() {
    pc = pop();
}

void drop(int n) {
    int ct;
    for(ct=0;ct<n;ct++){
        pop();
    }
}

void pushr() {
    push(returnValue);
}

void popr() {
    returnValue = pop();
}

void dup() {
    int x;
    x = pop();
    push(x);
    push(x);
}

void executable(unsigned int IR) {
    unsigned int instruction = IR >> 24;
    int immediate = SIGN_EXTEND(IMMEDIATE(IR));
    switch (instruction) {
        case PUSHC:
            push(immediate);
            break;
        case ADD:
            add();
            break;
        case SUB:
            sub();
            break;
        case MUL:
            mul();
            break;
        case DIV:
            divid();
            break;
        case MOD:
            mod();
            break;
        case RDINT:
            readInt();
            break;
        case WRINT:
            wrint();
            break;
        case RDCHR:
            rdchr();
            break;
        case WRCHR:
            wrchr();
            break;
        case PUSHG:
            pushg(immediate);
            break;
        case POPG:
            popg(immediate);
            break;
        case ASF:
            asf(immediate);
            break;
        case RSF:
            rsf();
            break;
        case PUSHL:
            pushl(immediate);
            break;
        case POPL:
            popl(immediate);
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
            greatEqual();
            break;
        case BRF:
            branchIfFalse(immediate);
            break;
        case BRT:
            branchIfTrue(immediate);
            break;
        case JMP:
            jmp(immediate);
            break;
        case CALL:
            call(immediate);
            break;
        case RET:
            ret();
            break;
        case DROP:
            drop(immediate);
            break;
        case PUSHR:
            pushr();
            break;
        case POPR:
            popr(immediate);
            break;
        case DUP:
            break;
        case HALT:

            break;
    }
}

/**
void printInst(unsigned int IR) {
    unsigned int instruction = IR >> 24;
    switch (instruction) {

        case PUSHC:
            printf(" %03d:\t PUSHC\t%d\n", counter, SIGN_EXTEND(IMMEDIATE(IR)));
            break;

        case ADD:
            printf(" %03d:\t ADD\n", counter);

            break;

        case SUB:
            printf(" %03d:\t SUB\n", counter);
            break;

        case MUL:
            printf(" %03d:\t MUL\n", counter);
            break;

        case DIV:
            printf(" %03d:\t DIV\n", counter);
            break;

        case MOD:
            printf(" %03d:\t MOD\n", counter);
            break;

        case RDINT:
            printf(" %03d:\t RDINT\n", counter);
            break;

        case WRINT:
            printf(" %03d:\t WRINT\n", counter);
            break;

        case RDCHR:
            printf(" %03d:\t RDCHR\n", counter);
            break;

        case WRCHR:
            printf(" %03d:\t WRCHR\n", counter);
            break;

        case HALT:
            printf(" %03d:\t halt\n", counter);
            break;

        case PUSHG:
            printf(" %03d:\t PUSHG\t%d\n", counter, SIGN_EXTEND(IMMEDIATE(IR)));
            break;

        case POPG:
            printf(" %03d:\t POPG\t%d\n", counter, SIGN_EXTEND(IMMEDIATE(IR)));
            break;

        case ASF:
            printf(" %03d:\t ASF\t%d\n", counter, SIGN_EXTEND(IMMEDIATE(IR)));
            break;

        case RSF:
            printf(" %03d:\t RSF\t%d\n", counter, SIGN_EXTEND(IMMEDIATE(IR)));
            break;

        case PUSHL:
            printf(" %03d:\t PUSHL\t%d\n", counter, SIGN_EXTEND(IMMEDIATE(IR)));
            break;

        case POPL:
            printf(" %03d:\t POPL\t%d\n", counter, SIGN_EXTEND(IMMEDIATE(IR)));
            break;

        case EQ:
            printf(" %03d:\t EQ\t%d\n", counter, SIGN_EXTEND(IMMEDIATE(IR)));
            break;
        case NE:
            printf(" %03d:\t NE\t%d\n", counter, SIGN_EXTEND(IMMEDIATE(IR)));
            break;

        case LT:
            printf(" %03d:\t LT\t%d\n", counter, SIGN_EXTEND(IMMEDIATE(IR)));
            break;

        case LE:
            printf(" %03d:\t LE\t%d\n", counter, SIGN_EXTEND(IMMEDIATE(IR)));
            break;

        case GT:
            printf(" %03d:\t GT\t%d\n", counter, SIGN_EXTEND(IMMEDIATE(IR)));
            break;
        case CALL:
            printf(" %03d:\t call\t%d\n", counter, SIGN_EXTEND(IMMEDIATE(IR)));
            break;

        case RET:
            printf(" %03d:\t GE\t%d\n", counter, SIGN_EXTEND(IMMEDIATE(IR)));
            break;

        case DROP:
            printf(" %03d:\t drop\t%d\n", counter, SIGN_EXTEND(IMMEDIATE(IR)));
            break;
        case PUSHR:
            printf(" %03d:\t pushr greatEqual\t%d\n", counter, SIGN_EXTEND(IMMEDIATE(IR)));
            break;

        case POPR:
            printf(" %03d:\t popr\t%d\n", counter, SIGN_EXTEND(IMMEDIATE(IR)));
            break;
        default:
            printf("not knowed instruction\n");
    }
}
*/

void execute() {
    int IR = 0;
    do {
        IR = programMemory[pc];
        pc = pc + 1;
        executable(IR);
    } while (IR >> 24 != HALT);

}
void workWithFile(int argc,char *argv[]){
    if (argv[1] != NULL) {
        fileName = argv[1];
        file = fopen(fileName, "r");
        //printf("after fopen() before if NULL \n");
        if (file == NULL) {
            printf("Error: cannot open code file '%s'\n", fileName);
            exit(1);
        }
        /** 1st Step: read the format of Binary File "NJBF" */
        //1) Read the first 4 bytes of the file.
        char *njbf = (char *) malloc(sizeof(char) * 4);
        fread(njbf, sizeof(char), 4, file);
        if (strcmp(njbf, "NJBF") != 0) {
            //printf("Format is NOT Correct :) \n");
            exit(1);
        }

        /** 2nd Step: Read the version number. **********/
        //2) Read the version number.
        fread(&version, sizeof(int), 1, file);
        //TODO: Version always under Observation
        if (version < 2) {
            printf("Error: file %s has wrong version number %d\n", fileName, version);
            exit(1);
        }

        /** 3rd Step: Read the Number of Instructions */
        // 3) Read the number of instructions.
        fread(&numberOfInstructions, sizeof(int), 1, file);
        programMemory = malloc(numberOfInstructions * sizeof(unsigned int));

        /** 4th Step: read the Number of Global-Variables in SDA */
        //4) Read the number of variables in the static data area.
        fread(&numberOfVariables, sizeof(int), 1, file);
        if (numberOfVariables > 0) {
            //SDA Allocate for Public Variables
            sda = malloc(numberOfVariables * sizeof(int));
        }

        /**5th Step: reading the rest of File */
        //5) Read the rest of the file into the memory allocated in step 3).
        fread(programMemory, sizeof(unsigned int), numberOfInstructions, file);
        execute();
        /**6th Step: Closing the File */
        if (fclose(file) != 0) {
            perror("ERROR (fclose)");
        }
    }
}

int main(int argc, char *argv[]) {


    printf("Ninja Virtual Machine started\n");

    workWithFile(argc,argv);


    printf("Ninja Virtual Machine stopped\n");
    return 0;
}
