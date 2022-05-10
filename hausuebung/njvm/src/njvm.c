#include <stdio.h>
#include <stdlib.h>
#include "stack.h"
#include "programs.h"
#include "opCodes.h"

unsigned int instruction;

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
    }
}


unsigned int program_memory[] = { //Example
        0x01000002, //program_memory[0]
        0x01000003, //program_memory[1]
        0x04000000, //program_memory[2]
        0x01000005, //program_memory[3]
        0x02000000, //program_memory[4]
        0x08000000, //program_memory[5]
        0x00000000 //program_memory[6]
};


//-------------------------------------------------------------
int main(int argc, char *argv[]) {
    FILE *fp = NULL;
    char filename[128];
    char error_msg[256];
    int read_len = 0;
    sprintf(filename, "./xy.z");
    if ((fp = fopen(filename, "r")) == NULL) {
        snprintf(error_msg, 256, "ERROR (fdopen) -> File (%s)",
                 filename);
        perror(error_msg);
        exit(1);
    }
    char c[4];
    unsigned int x;
    printf("idxHEX CHAR |idxHEX CHAR |");
    printf("idxHEX CHAR |idxHEX CHAR\n");
    while ((read_len = fread(c, 1, 4, fp)) != 0) {
        printf("c[0]=[0x%1$x] [%1$c] |", c[0]);
        printf("c[1]=[0x%1$x] [%1$c] |", c[1]);
        printf("c[2]=[0x%1$x] [%1$c] |", c[2]);
        printf("c[3]=[0x%1$x] [%1$c]\n", c[3]);
    }
    printf("\nfile position = [%lu] after 1. loop\n", ftell(fp));
    fseek(fp, 0, SEEK_SET); // seek back to beginning of file
    while ((read_len = fread(&x, sizeof(unsigned int), 1, fp)) != 0) {
        printf("read %d object [%ld bytes]: x = [0x%08x]\n",
               read_len, read_len * sizeof(unsigned int), x);
    }
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
