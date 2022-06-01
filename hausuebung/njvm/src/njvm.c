#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "stack.h"

#define FORMAT "NJBF"
#define VERSION 4


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
char *filename;
char c;
int format[4];
char streamsFile[4];   //4 bytes -->NJBF
unsigned int version[3]; //version,instruction et var statik

int *progSpeicher;   // je speichern les instruction que jaurais lu
int *sda; // ici j alloquieren la place pr le anzahl d instruction qui yaura zb 13 dans le prog01.bin
int pc = 0;
unsigned int nummer = 0;
int fp = 0;
char *start = "Ninja Virtual Machine started";
char *end = "Ninja Virtual Machine stopped";
int returnValue;

int a ,b;

void halt() {
    exit(0);
}

void add() {
    int a = pop();
    int b = pop();
    int c = a + b;

    push(c);
}

void sub() {
    int a = pop();
    int b = pop();
    int c = b - a;

    push(c);
}

void mul() {
    int a = pop();
    int b = pop();
    int c = a * b;

    push(c);
}

void division() {
    int a = pop();
    int b = pop();
    int c = b / a;

    push(c);
}

void mod() {

    int a = pop();
    int b = pop();
    int c = b % a;

    push(c);
}

void rdint() {

    int eingabe;
    scanf("%d", &eingabe);
    push(eingabe);
}

void wrint() {
    int a = pop();
    printf("%d", a);
}

void rdchr() {
    char eingabe;
    scanf("%c", &eingabe);
    push(eingabe);
}

void wrchr() {
    char a = pop();
    printf("%c", a);
}

void pushg(int element) {

    int y = sda[element];
    printf("pushg %d\n", y);
    push(y);
}

void popg(int element) {

    int x = pop();
    printf("popg %d\n", x);
    sda[element] = x;
}

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

void pushl(int lokVar) {
    // push to stack local variable
    push(stack[fp + lokVar]);
}

void popl(int lokVar) {
    stack[fp + lokVar] = pop();
}

void eq() {
    a = pop();
     b = pop();
    if (a == b) {
        push(1);
    } else {
        push(0);
    }
}

void ne() {

    int a = pop();
    int b = pop();
    if (a != b) {
        push(1);
    } else {
        push(0);
    }
}

void lt() {
    int a = pop();
    int b = pop();
    if (b > a) {
        push(1);
    } else {
        push(0);
    }
}

void le() {
    int a = pop();
    int b = pop();
    if (b <= a) {
        push(1);
    } else {
        push(0);
    }
}

void gt() {
    int a = pop();
    int b = pop();
    if (b > a) {
        push(1);
    } else {
        push(0);
    }
}

void ge() {
    int a = pop();
    int b = pop();
    if (b >= a) {
        push(1);
    } else {
        push(0);
    }
}

void brt(int a) {
    int b = pop();

    if (b == 1) pc = a;

}

void brf(int a) {
    int b = pop();

    if (b == 0) pc = a;

}

void jmp(int a) {
    pc = a;
}

void call(int x) {
//ZB
    push(pc);
    pc = x;
}

void ret() {
    pc = pop();
}

void drop(int n) {
//loescht alle lokale var die in der methode waren
    while (n != 0) {
        pop();
        n--;
    }

}

void pushr() {
    //cette valeur qui etait le resultat d une methode kon avait appeler
    push(returnValue);

}

void popr() {

//pop die oberste value und speichert in returnvalue

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

    switch (instruction) {
        case PUSHC:
            push(SIGN_EXTEND(IMMEDIATE(IR)));
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
            division();
            break;
        case MOD:
            mod();
            break;
        case RDINT:
            rdint();
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
            pushg(SIGN_EXTEND(IMMEDIATE(IR)));
            break;
        case POPG:
            popg(SIGN_EXTEND(IMMEDIATE(IR)));
            break;

        case ASF:
            asf(SIGN_EXTEND(IMMEDIATE(IR)));
            break;
        case RSF:
            rsf();
            break;

        case PUSHL:
            pushl(SIGN_EXTEND(IMMEDIATE(IR)));
            break;
        case POPL:
            popl(SIGN_EXTEND(IMMEDIATE(IR)));
            break;

        case EQ:
            eq();
            break;
        case NE:
            ne();
            break;
        case LT:
            lt();
            break;
        case LE:
            le();
            break;
        case GT:
            gt();
            break;
        case GE:
            ge();
            break;
        case BRF:
            brf(SIGN_EXTEND(IMMEDIATE(IR)));
            break;

        case BRT:
            brt(SIGN_EXTEND(IMMEDIATE(IR)));
            break;

        case JMP:
            jmp(SIGN_EXTEND(IMMEDIATE(IR)));
            break;
        case CALL:
            call(SIGN_EXTEND(IMMEDIATE(IR)));
            break;

        case RET:
            ret();
            break;

        case DROP:
            drop(SIGN_EXTEND(IMMEDIATE(IR)));
            break;

        case PUSHR:
            pushr();
            break;
        case POPR:
            popr(SIGN_EXTEND(IMMEDIATE(IR)));
            break;
        case DUP:
            break;
        case HALT:
            halt();
            break;
    }
}

void listener(unsigned int IR) {

    if (IR >> 24 == PUSHC) {
        printf(" %03d:\t PUSHC\t%d\n", nummer, SIGN_EXTEND(IMMEDIATE(IR)));
    } else if (IR >> 24 == ADD) {
        printf(" %03d:\t ADD\n", nummer);
    } else if (IR >> 24 == SUB) {
        printf(" %03d:\t SUB\n", nummer);
    } else if (IR >> 24 == MUL) {
        printf(" %03d:\t MUL\n", nummer);
    } else if (IR >> 24 == DIV) {
        printf(" %03d:\t DIV\n", nummer);
    } else if (IR >> 24 == MOD) {
        printf(" %03d:\t MOD\n", nummer);
    } else if (IR >> 24 == RDINT) {
        printf(" %03d:\t RDINT\n", nummer);
    } else if (IR >> 24 == WRINT) {
        printf(" %03d:\t WRINT\n", nummer);
    } else if (IR >> 24 == RDCHR) {
        printf(" %03d:\t RDCHR\n", nummer);
    } else if (IR >> 24 == WRCHR) {
        printf(" %03d:\t WRCHR\n", nummer);
    } else if (IR >> 24 == HALT) {
        printf(" %03d:\t halt\n", nummer);
    } else if (IR >> 24 == PUSHG) {
        printf(" %03d:\t PUSHG\t%d\n", nummer, SIGN_EXTEND(IMMEDIATE(IR)));
    } else if (IR >> 24 == POPG) {
        printf(" %03d:\t POPG\t%d\n", nummer, SIGN_EXTEND(IMMEDIATE(IR)));
    } else if (IR >> 24 == ASF) {
        printf(" %03d:\t ASF\t%d\n", nummer, SIGN_EXTEND(IMMEDIATE(IR)));
    } else if (IR >> 24 == RSF) {
        printf(" %03d:\t RSF\t%d\n", nummer, SIGN_EXTEND(IMMEDIATE(IR)));
    } else if (IR >> 24 == PUSHL) {
        printf(" %03d:\t PUSHL\t%d\n", nummer, SIGN_EXTEND(IMMEDIATE(IR)));
    } else if (IR >> 24 == POPL) {
        printf(" %03d:\t POPL\t%d\n", nummer, SIGN_EXTEND(IMMEDIATE(IR)));
    } else if (IR >> 24 == EQ) {
        printf(" %03d:\t EQ\t%d\n", nummer, SIGN_EXTEND(IMMEDIATE(IR)));
    } else if (IR >> 24 == NE) {
        printf(" %03d:\t NE\t%d\n", nummer, SIGN_EXTEND(IMMEDIATE(IR)));
    } else if (IR >> 24 == LT) {
        printf(" %03d:\t LT\t%d\n", nummer, SIGN_EXTEND(IMMEDIATE(IR)));
    } else if (IR >> 24 == LE) {
        printf(" %03d:\t LE\t%d\n", nummer, SIGN_EXTEND(IMMEDIATE(IR)));
    } else if (IR >> 24 == GT) {
        printf(" %03d:\t GT\t%d\n", nummer, SIGN_EXTEND(IMMEDIATE(IR)));
    } else if (IR >> 24 == CALL) {
        printf(" %03d:\t call\t%d\n", nummer, SIGN_EXTEND(IMMEDIATE(IR)));
    } else if (IR >> 24 == RET) {
        printf(" %03d:\t GE\t%d\n", nummer, SIGN_EXTEND(IMMEDIATE(IR)));
    } else if (IR >> 24 == DROP) {
        printf(" %03d:\t drop\t%d\n", nummer, SIGN_EXTEND(IMMEDIATE(IR)));
    } else if (IR >> 24 == PUSHR) {
        printf(" %03d:\t pushr ge\t%d\n", nummer,SIGN_EXTEND(IMMEDIATE(IR)));
    } else if (IR >> 24 == POPR) {
        printf(" %03d:\t popr\t%d\n", nummer, SIGN_EXTEND(IMMEDIATE(IR)));
    } else {
        printf("i dont know\n");
    }
}

void show(char *msg) {
    printf("%s\n", msg);
}

void execute() {
    int IR = 0;


    do {

        IR = progSpeicher[pc];
        pc = pc + 1;
        executable(IR);
    } while (IR >> 24 != HALT);

    printf("%s", end);
}

int isbin(char *filename) {
    //
    char *sub = malloc(sizeof(char) * 5);
    int len = strlen(filename);
    int position = len - 4;

    int c = 0;
    sub[4] = '\0';

    while (position < len) {
        sub[c] = filename[position];
        c++;
        position++;
    }

    if (strcmp(sub, ".bin") == 0) {

        // printf("binary datei%s", sub);
        return 1;
    } else {
        return 0;
    }
}

int main(int argc, char *argv[]) {

    if (argc > 1) {
        if (argc > 2) {
            printf("err >2");
        } else {
            filename = argv[1];

            file = fopen(argv[1], "r"); // je lis l argument par ex prog01.bin
            if (file == NULL) {
                printf("Error cannot open file %s", argv[1]);
            } else {

                // printf("hier lesen faengt an");
                if (isbin(filename) == 1) {
                    // printf("binary file\n");

                    fread(streamsFile, sizeof(char), 4, file); // streamfile speicher les 4 premiers bytes Format

                    fread(version, sizeof(int), 3, file); //version  hat 3 plaetze version-->instruction-->statik var

                    if (strncmp(streamsFile, FORMAT, 4) == 0 && (int) version[0] == VERSION) {
                        progSpeicher = malloc(sizeof(int) * (int) version[1]);
                        if ((int) version[2] != 0) {
                            sda = malloc(sizeof(int) * (int) version[2]);
                        }
                        fread(progSpeicher, sizeof(int), version[1], file);

                        show(start);

                        execute();
                        printf("%s", end);
                    } else {
                    }
                } else {
                    printf("Error file %s is not a Ninja binary", argv[1]);
                }
            }
        }
    } else {
        printf("error : no code file specified");
    }

    return 0;
}
