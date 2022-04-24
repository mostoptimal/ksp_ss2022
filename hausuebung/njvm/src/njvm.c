#include <stdio.h>

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

#define MAX_ITEMS 100
unsigned int halt = HALT << 24; // <- 0x00000000
unsigned int push_c = PUSHC << 24; // <- 0x01000000
unsigned int add = ADD << 24; // <- 0x02000000
unsigned int sub = SUB << 24; // <- 0x03000000
unsigned int mul = MUL << 24; // <- 0x04000000
unsigned int div = DIV << 24; // <- 0x05000000
unsigned int mod = MOD << 24; // <- 0x06000000
unsigned int rdint = RDINT << 24; // <- 0x07000000
unsigned int wrint = WRINT << 24; // <- 0x08000000
unsigned int rdchr = RDCHR << 24; // <- 0x09000000
unsigned int wrchr = WRCHR << 24; // <- 0x10000000


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
//Stack End---------------------------------------------------------------


unsigned int program_memory[] = {
        0x01000002, //program_memory[0]
        0x01000003, //program_memory[1]
        0x04000000, //program_memory[2]
        0x01000005, //program_memory[3]
        0x02000000, //program_memory[4]
        0x08000000, //program_memory[5]
        0x00000000 //program_memory[6]
};

void pushc(int c) {
    if (sp >= MAX_ITEMS)
        printf("Stack is full ...");
    else
        push(c);
}

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
    a = (float) pop();
    b = (float) pop();
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
    int value;
    print_stack();
    push(5);
    print_stack();
    push(3);
    print_stack();
    value = pop();
    print_stack();
    return 0;
}