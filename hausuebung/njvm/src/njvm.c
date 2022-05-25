#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "stack.h"
#include "programs.h"
#include "opCodes.h"
#include "instructions.h" //Contains VM Instructions

#define DEBUG
#define FORMAT "NJBF"
char *runOption;
FILE *fp;
unsigned int instruction;
int *sda; //static Data Area
char *formatValue;
int version;
int instructionNumber;
int variablesNumber;
int sdaCounter = 0;
int pc = 0;
int sp=0;
unsigned int *allInstruct;
unsigned int *allVariable;

//char filename[] = "t.bin";
char error_msg[256];
int read_len = 0;
/** todo list
 * 1)  run instruction testen
 * 2) execute testen
 *  3) pushl und pushg popg popl testen
 *  4) implementieren call , andere methode drop,
 *  5) test die ausgabe von aufgabe 2 ,3 ,4
 *  6) debbug implementieren als option ,
   */

void execute(unsigned int instruct, int immediate) {
    int a, b, res, target;
    int jmpTarget = immediate;
    instruct=SIGN_EXTEND(instruct);

    switch (instruct) {
        case 0://HALT
            break;
        case 1://PUSHC
            printf("PUSHC[ %d ]", instruct);
            push(instruct);

        case 2://ADD
            a = pop();
            b = pop();
            a = a + b;
            printf("ADD %d + %d", a, b);
            push(a);

        case 3://SUB
            a = pop();
            b = pop();
            a = a - b;
            printf("SUB %d - %d", a, b);
            push(a);

        case 4://MUL
            a = pop();
            b = pop();
            a = a * b;
            printf("MUL %d * %d", a, b);
            push(a);

        case 5://DIV
            a = pop();
            b = pop();
            a = a / b;
            printf("DIV %d / %d", a, b);
            push(a);

        case 6://MOD
            a = pop();
            b = pop();
            a = a % b;
            printf("MOD %d %d", a, b);
            push(a);

            //case 11: //PUSHG
            //a = sda[n];
            //pushg < n > (a);

        case 17://eq
            a = pop();
            b = pop();
            res = equal(a, b);
            push(res);

        case 18://ne
            a = pop();
            b = pop();
            res = nequal(a, b);
            push(res);

        case 19://lt
            a = pop();
            b = pop();
            res = lessThan(a, b);
            push(res);

        case 20://le
            a = pop();
            b = pop();
            res = lessEqual(a, b);
            push(res);

        case 21://gt
            a = pop();
            b = pop();
            res = greaterThan(a, b);
            push(res);

        case 22://ge
            a = pop();
            b = pop();
            res = greaterEqual(a, b);
            push(res);

        case 23://jmp <target>
            pc = jump(jmpTarget);

        case 24://brf <target>
            b = pop();
            pc = branchFalse(b, target);

        case 25://brt <target>
            b = pop();
            pc = branchTrue(b, target);

        case CALL:
             //inemediate werte instruction
             push(pc);
             pc = immediate;
            // pc = jump()
            //  instruction anrufen

        case RET:
           pc = pop();
        case DROP:
        {
           int k =0;
           while(k<immediate){
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
            printf("not on the List of the Instructions in VM...!!");
    }
}




 void RuninstructiontoAssemble(int porgrammemory[]){
    int i = 0;
    while (porgrammemory[i]!=0){// HALT
        execute(porgrammemory[i], 1);
        i++;
        pc++;
    }
 }

/*unsigned int program_memory[] = { //Example
        0x01000002, //program_memory[0]
        0x01000003, //program_memory[1]
        0x04000000, //program_memory[2]
        0x01000005, //program_memory[3]
        0x02000000, //program_memory[4]
        0x08000000, //program_memory[5]
        0x00000000  //program_memory[6]
};*/
void readInputInTerminal(char *argv[], int argc) {
    //read info for data
    //argumente
    //  char allarguments [argc];
    if (argv < 0) {
        printf("u dont have any parameter");
    }
    for (int i = 1; i < argc; i++) {
        //
        if (strcmp(argv[i], "--debug")==0) {
            //call program , option
            runOption = "debug";
            char debugOption;
            char i, l, b, s, r, q;
            scanf("%c", debugOption);

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
    //TODO: file path as Argument
    char *path;
    path = argv[1];
    /**
   if (argc >= 2) {
        for (int i = 2; i < argc ; i++) {
            if (strcmp(argv[i], "--version") == 0) {
                printf("Ninja Virtual Machine (compiled %s at %s)\n", __DATE__, __TIME__);
                return 0;
            } else if (strcmp(argv[i], "--help") == 0) {
                printf("Usage: ./njvm [option]\n");
                printf("  --help          Show this help and exits\n");
                printf("  --version       Show NJVM version and exits\n");
                printf("  [file]          Loads the file and runs it (filename cannot start with \"--\"\n");
                printf("  [file] --debug  Loads the file and runs it in debug mode\n");
                return 0;
            } else if (strcmp(argv[i], "--debug") == 0) {
                //debug = 1;
            } else {
                printf("unknown argument '%s', try './njvm --help'", argv[1]);
                return 1;
            }
        }

    }
// to do later
#define DEBUG
*/

    fp = fopen(path, "r");

    printf("after fopen() before if NULL \n");
    if (fp == NULL) {
        perror("Error File Open!!\n");
        exit(1);
    }

    /** 1st Step: read the format of Binary File "NJBF" */
    //1) Read the first 4 bytes of the file.
    //fscanf(fp, "%s", strFormatIdentifier);
    char *njbf = (char *) malloc(sizeof(char) * 4);
    int s1 = fread(njbf, sizeof(char), 4, fp); // streamfile speicher les 4 premiers bytes Format
    printf("das ist %d\n", s1);

    if (strcmp(njbf, "NJBF") == 0) {
        printf("Format is Correct :) \n");
    } else {
        printf("Format is inCorrect :( !!!");
        exit(1);
    }

    printf("after fread(&x, 1, 1, fp);\n");

    /** 2nd Step: Read the version number. **********/
    //2) Read the version number.
    printf("2nd Step\n");
    fread(&version, sizeof(int), 1, fp);
    printf("after 2nd fread()\n");
    if (version != 2) {
        printf("the file does not have the right Version, %d \n", version);
        exit(1);
    } else {
        printf("Version is right :)\n");
    }
    printf("after if version \n");

    /** 3rd Step: Read the Number of Instructions */
    // 3) Read the number of instructions.
    printf("3rd Step\n");
    fread(&instructionNumber, sizeof(int), 1, fp);
    printf("after fread(3)\n");
    allInstruct = malloc(instructionNumber * sizeof(unsigned int));

    printf("instutNmber: %d\n", instructionNumber);

    /** 4th Step: read the Number of Global-Variables in SDA */
    //4) Read the number of variables in the static data area.
    fread(&variablesNumber, sizeof(int), 1, fp);

    if (variablesNumber > 0) {
        printf("Number of Vars: %d\n", variablesNumber);
        //SDA Allocate for Public Variables
        sda = malloc(variablesNumber * sizeof(int));
    }

    /**5th Step: reading the rest of File */
    //5) Read the rest of the file into the memory allocated in step 3).
    //int fileReadValue = fread(allInstruct, sizeof(unsigned int), instructionNumber, fp);

    if (fclose(fp) != 0) {
        perror("ERROR (fclose)");
    }
    printf("Virtual Machine stopped\n");
    return 0;
}