#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stack.h"
#include "programs.h"
#include "opCodes.h"
#include "instructions.h" //Contains VM Instructions

#define DEBUG

char *runOption;
FILE *fp;
unsigned int instruction;
int *sda; //static Data Area
int version;
int numberOfInstructions;
int numberOfVariables;
int sdaIndex = 0;
int pc = 0;
unsigned int *allInstruct;
unsigned int *allVariable;

/** todo list
 * 1)  run instruction testen
 * 2) execute testen
 *  3) pushl und pushg popg popl testen
 *  4) implementieren call , andere methode drop,
 *  5) test die ausgabe von aufgabe 2 ,3 ,4
 *  6) debbug implementieren als option ,
   */

void execute(unsigned int instruct, int immediate) {
    int a, b, res;
    int target = immediate;
    unsigned int IR = (instruct >> 24);

    switch (IR) {
        case HALT://HALT
            break;
        case PUSHC://PUSHC
            printf("PUSHC[ %d ]", instruct);
            push(instruct);

        case ADD://ADD
            a = pop();
            b = pop();
            a = a + b;
            printf("ADD %d + %d", a, b);
            push(a);

        case SUB://SUB
            a = pop();
            b = pop();
            a = a - b;
            printf("SUB %d - %d", a, b);
            push(a);

        case MUL://MUL
            a = pop();
            b = pop();
            a = a * b;
            printf("MUL %d * %d", a, b);
            push(a);

        case DIV://DIV
            a = pop();
            b = pop();
            a = a / b;
            printf("DIV %d / %d", a, b);
            push(a);

        case MOD://MOD
            a = pop();
            b = pop();
            a = a % b;
            printf("MOD %d %d", a, b);
            push(a);

            case PUSHG: //PUSHG
            //a = sda[n];
            //pushg < n > (a);

        case EQ://eq
            a = pop();
            b = pop();
            res = equal(a, b);
            push(res);

        case NE://ne
            a = pop();
            b = pop();
            res = nequal(a, b);
            push(res);

        case LT://lt
            a = pop();
            b = pop();
            res = lessThan(a, b);
            push(res);

        case LE://le
            a = pop();
            b = pop();
            res = lessEqual(a, b);
            push(res);

        case GT://gt
            a = pop();
            b = pop();
            res = greaterThan(a, b);
            push(res);

        case GE://ge
            a = pop();
            b = pop();
            res = greaterEqual(a, b);
            push(res);

        case JMP://jmp <target>
            pc = jump(target);

        case BRF://brf <target>
            b = pop();
            if(!b) pc=target;
            //pc = branchFalse(b, target);

        case BRT://brt <target>
            b = pop();
            if(b) pc=target;
            //pc = branchTrue(b, target);

        case CALL:
            //inemediate werte instruction
            push(pc);
            pc = immediate;
            // pc = jump()
            //  instruction anrufen

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

        case POPR:

        case DUP:
            // sp +1
            // lege ich da die gleiche variable
            a = pop();
            push(a);
            push(a);

        default:
            printf("not on the List of the Instructions in VM...!!\n");
    }
}


void RunInstructionToAssemble(unsigned int programMemory[]) {
    int i = 0;
    while (programMemory[i] != 0) {// HALT
        execute(programMemory[i], 1); //TODO 2nd Parameter
        i++;
        pc++;
    }
}

void readInputInTerminal(int argc, char *argv[]) {
    //read info for data
    //argumente
    //  char allarguments [argc];
    if (argc == 0) {
        printf("u dont have any parameter");
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

    char *path;
    path = argv[1];

    fp = fopen(path, "r");

    //printf("after fopen() before if NULL \n");
    if (fp == NULL) {
        perror("Error File Open!!\n");
        exit(1);
    }

    /** 1st Step: read the format of Binary File "NJBF" */
    //1) Read the first 4 bytes of the file.

    char *njbf = (char *) malloc(sizeof(char) * 4);
    fread(njbf, sizeof(char), 4, fp); // streamfile speicher les 4 premiers bytes Format
    //printf("das ist %d\n", s1);

    if (strcmp(njbf, "NJBF") == 0) {
        //printf("Format is Correct :) \n");
    } else {
        //printf("Format is inCorrect :( !!!");
        exit(1);
    }

    //printf("after fread(&x, 1, 1, fp);\n");

    /** 2nd Step: Read the version number. **********/
    //2) Read the version number.
    //printf("2nd Step\n");
    fread(&version, sizeof(int), 1, fp);
    //printf("after 2nd fread()\n");
    if (version != 2) {
        printf("Error: file %s",path," has wrong version number %d\n", version);
        exit(1);
    } else {
        //printf("Version is right :)\n");
    }
    //printf("after if version \n");

    /** 3rd Step: Read the Number of Instructions */
    // 3) Read the number of instructions.
    //printf("3rd Step\n");
    fread(&numberOfInstructions, sizeof(int), 1, fp);
    //printf("after fread(3)\n");
    allInstruct = malloc(numberOfInstructions * sizeof(unsigned int));

    printf("instutNmber: %d\n", numberOfInstructions);

    /** 4th Step: read the Number of Global-Variables in SDA */
    //4) Read the number of variables in the static data area.
    fread(&numberOfVariables, sizeof(int), 1, fp);

    if (numberOfVariables > 0) {
        printf("Number of Vars: %d\n", numberOfVariables);
        //SDA Allocate for Public Variables
        sda = malloc(numberOfVariables * sizeof(int));
    }

    /**5th Step: reading the rest of File */
    //5) Read the rest of the file into the memory allocated in step 3).
    int fileReadValue = fread(allInstruct, sizeof(unsigned int), numberOfInstructions, fp);
    printf("fileReadVal: %d",fileReadValue);
    RunInstructionToAssemble(allInstruct);

    if (fclose(fp) != 0) {
        perror("ERROR (fclose)");
    }
    printf("Virtual Machine stopped\n");
    return 0;
}