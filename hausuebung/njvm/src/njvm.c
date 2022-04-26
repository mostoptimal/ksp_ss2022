#include <stdio.h>


#define MAX_ITEMS 100
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
#define IMMEDIATE(x) ((x)&0x00FFFFFF)
#define SIGN_EXTEND(i) ((i) & 0x00800000 ? (i) | 0xFF000000 : (i))

unsigned int instruction;


//Stack Begin---------------------------------------------------------------------
int sp = 0;
int stack[MAX_ITEMS];

void push(int x) {
    printf("-[%4s]-> pushing [%d] onto stack @sp [%d]\n", __func__, x, sp);
    stack[sp] = x;
    printf("-[%4s]-> inc stack pointer [%d -> ", __func__, sp);
    sp++;
    printf("%d]\n", sp);
}

int pop(void) {
    printf("-[%4s]-> dec stack pointer [%d -> ", __func__, sp);
    sp--;
    printf("%d]\n", sp);
    int tmp = stack[sp];
    printf("-[%4s]-> popping [%d] from stack @sp [%d]\n", __func__, tmp, sp);
    return tmp;
}

void print_stack(void) {
    printf("\n   Stack\n");
    printf(".-------+--------.\n");
    for (int i = sp; i >= 0; i--) {
        if (i == sp)
            printf("|sp->%3d| <empty>|\n", i);
        else
            printf("|%7d| %5d |\n", i, stack[i]);
    }
    printf("'-------+--------'\n\n");
}
//Stack End-----------------------------------------------------------------------

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
            printf("MOD %d % %d", a, b);
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

unsigned int program_1_memory[] = { //Program_1
        (PUSHC << 24) | IMMEDIATE(3),
        (PUSHC << 24) | IMMEDIATE(4),
        (ADD << 24),
        (PUSHC << 24) | IMMEDIATE(10),
        (PUSHC << 24) | IMMEDIATE(6),
        (SUB << 24),
        (MUL << 24),
        (WRINT << 24),
        (PUSHC << 24) | IMMEDIATE(10),
        (WRCHR << 24),
        (HALT << 24)
};
unsigned int program_2_memory[] = { //Program_2
        (PUSHC << 24) | SIGN_EXTEND(-2),
        (RDINT << 24),
        (MUL << 24),
        (PUSHC << 24) | IMMEDIATE(3),
        (ADD << 24),
        (WRINT << 24),
        (PUSHC << 24) | IMMEDIATE('\n'),
        (WRCHR << 24),
        (HALT << 24)
};

unsigned int program_3_memory[] = {
        (RDCHR << 24),
        (WRINT << 24),
        (PUSHC << 24) | IMMEDIATE('\n'),
        (WRCHR << 24),
        (HALT << 24)
};

// Instructions ----------------------------------------------
void add_() {
    int a, b;
    a = pop();
    b = pop();
    a = a + b;
    push(a);
}

void sub_() {
    int a, b;
    a = pop();
    b = pop();
    a = a - b;
    push(a);
}

void div_() {
    float a, b;
    a = pop();
    b = pop();
    if (b != 0)
        a = a / b;
    else
        printf("Error Dividing by Zero!");
    push(a);
}

void mul_() {
    int a, b;
    a = pop();
    b = pop();
    a = a * b;
    push(a);
}

//-------------------------------------------------------------
int main(int argc, char *argv[]) {
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

/*
 *  int value;
    print_stack();
    push(5);
    print_stack();
    push(3);
    print_stack();
    value = pop();
    print_stack();*/
    return 0;
}