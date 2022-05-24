#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stack.h"
#include "programs.h"
#include "opCodes.h"
#include "instructions.h" //Contains VM Instructions

#define FORMAT "NJBF"

FILE *fp;
unsigned int instruction;
int *sda; //static Data Area
char *formatValue;
int version;
int instructionNumber;
int variablesNumber;
int sdaCounter = 0;
int pc = 0;
unsigned int *allInstruct;
unsigned int *allVariable;

//char filename[] = "t.bin";
char error_msg[256];
int read_len = 0;

void execute(int instruct) {
    int a, b, res, target;
    int jmpTarget;
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

        default:
            printf("not on the List of the Instructions in VM...!!");
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


//-------------------------------------------------------------
int main(int argc, char *argv[]) {
    unsigned int x;
    char strFormatIdentifier[4];

    fp = fopen("prog1.bin", "r");

    printf("after fopen() before if NULL \n");
    if (fp == NULL) {
        perror("Error File Open!!\n");
        exit(1);
    }

    /** 1st Step: read the format of Binary File "NJBF" */
    //1) Read the first 4 bytes of the file.
    //fscanf(fp, "%s", strFormatIdentifier);

    fread(&strFormatIdentifier, sizeof(char), 4, fp); // streamfile speicher les 4 premiers bytes Format

    if(strncmp(strFormatIdentifier, FORMAT, 4) == 0){
        printf("Format is Correct :) \n");
    }

    printf("after fread(&x, 1, 1, fp);\n");
/*    for (int i = 0; i <= 3; i++) {
        printf("%.2x\t", strFormatIdentifier); //2x pr l hexadecimal
    }*/
    printf("\n");

/*    if (strFormatIdentifier == 0x4e4a4246) {
        printf("matched!!!");
    } else {
        printf("NOT matched!!!");
        exit(1);
    }*/

    /** 2nd Step: Read the version number. */
    //2) Read the version number.

    fread(&version, sizeof(int), 1, fp);
    if (version != 2) {
        printf("the file does not have the right Version, %d \n", version);
        exit(1);
    }

    /** 3rd Step: Read the Number of Instructions */
    // 3) Read the number of instructions.
    fread(&instructionNumber, sizeof(int), 1, fp);
    allInstruct = malloc(instructionNumber * sizeof(unsigned int));

    /** 4th Step: read the Number of Global-Variables in SDA */
    //4) Read the number of variables in the static data area.
    fread(&variablesNumber, sizeof(int), 1, fp);
    if (variablesNumber > 0) {
        sda = malloc(variablesNumber * sizeof(int));
        //SDA Allocate for Public Variables
    }

    /**5th Step: reading the rest of File */
    //5) Read the rest of the file into the memory allocated in step 3).
    int fileReadValue = fread(allInstruct, sizeof(unsigned int), instructionNumber, fp);


    // now reading the Code from file
    /**
    while (fread(c, 1, 4, fp) != EOF) {
        instruction = program_memory[pc];
        printf("PC = %d   ", instruction);

        printf("OPCode = %d \n", opcode);
        pc++;
        opcode = program_1_memory[pc];
        execute(instruction);
    }
*/

    if (fclose(fp) != 0) {
        perror("ERROR (fclose)");
    }
    return 0;
}


/*
int main(int argc, char *argv[]) {
    //file Operations......
    FILE *fp;
    fp = fopen("./xy.z", "w+");
    if (fp == NULL) {
        printf("...");
        exit(99);
    }
    fprintf(fp, "This is testing for fprintf...\n");
    fputs("This is testing for fputs...\n", fp);
    fclose(fp);
    //file Operations End....
    int pc = 0;
    unsigned int opcode = program_1_memory[0];
    while (opcode != HALT) {
        instruction = program_memory[pc];
        printf("PC = %d   ", instruction);

        printf("OPCode = %d \n", opcode);
        pc++;
        opcode = program_1_memory[pc];
        execute(instruction);
    }

*/
/*
 *  int value;
    print_stack();
    push(5);
    print_stack();
    push(3);
    print_stack();
    value = pop();
    print_stack();*//*

    return 0;
}*/
