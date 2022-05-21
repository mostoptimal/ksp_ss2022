#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stack.h"
#include "programs.h"
#include "opCodes.h"


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
    int a, b;
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
    char cbb[4];
    fp = fopen("t.bin", "r");
    //printf("File:%s \n",fp);
    if (fp == NULL) {
        snprintf(error_msg, 256, "ERROR (fopen) -> File ()");
        perror(error_msg);
        exit(1);
    }

    /** 1st Step: read the format of Binary File "NJBF" */
    //1) Read the first 4 bytes of the file.
    //read_len = fread(&cbb, 1 * sizeof(char), 10, fp);
    read_len = fscanf(fp,"F",cbb);
    //read_len = fread(formatValue, 4 * sizeof(char *), 1, fp);
    printf("cbb[0]=%c , cbb[1]=%c , cbb[2]=%c , cbb[3]=%c\n",cbb[0],cbb[1],cbb[2],cbb[3]);
    if (strcmp(cbb, "NJBF") != 0) {
        printf("the Format in File is not exact!");
        exit(1);
    }

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
