//
// Created by mohamad on 5/23/22.
//
#include <stdio.h>
#include "instructions.h"
#include "stack.h"

int a, b, res;

void add() {
    a = pop();
    b = pop();
    res = a + b;
    push(res);
    printf("ADD %d + %d\n", a, b);
}

void sub() {
    a = pop();
    b = pop();
    res = a - b;
    printf("SUB %d - %d\n", a, b);
    push(res);
}

void mul() {
    a = pop();
    b = pop();
    res = a * b;
    printf("MUL %d * %d\n", a, b);
    push(res);
}

void divid() {
    a = pop();
    b = pop();
    res = a / b;
    printf("DIV %d / %d\n", a, b);
    push(res);
}

void mod() {
    a = pop();
    b = pop();
    res = a % b;
    printf("MOD %d %d\n", a, b);
    push(res);
}

void equal() {
    a = pop();
    b = pop();
    if (a == b) {//or v1-v2==0
        res = 1;//1 for True
    } else res = 0;//0 for False
    push(res);
}

void nequal() {
    a = pop();
    b = pop();
    if (a != b) res = 1;
    else res = 0;
    push(res);
}

void lessThan() {
    a = pop();
    b = pop();
    if (a < b) res = 1;
    else res = 0;
    push(res);
}

void lessEqual() {
    a = pop();
    b = pop();
    if (b >= a) res = 1;
    else res = 0;
    push(res);
}

void greaterThan() {
    a = pop();
    b = pop();
    if (a - b > 0) res = 1;
    else res = 0;
    push(res);
}

void greaterEqual() {
    a = pop();
    b = pop();
    if (a - b >= 0) res = 1;
    else res = 0;
    push(res);
}

int jump(int label) {
// zu implementieren
    return label;
}

void readInt() {
    int input = 0;
    scanf("%d", &input);
    push(input);


}

void readChar() {
    char inputC;
    scanf("%c", &inputC);
    push(inputC);
}

int writeInt() {
    int output = pop();
    printf("%d", output);
    return output;
}

char writeChar() {
    char output = (char) pop();
    printf("%c", output);
    return output;
}


/**
int branchFalse(int condition, int target) {
    int ziel=target;
    if (condition == 0)
        return ziel;

}

int branchTrue(int condition, int target) {
    int ziel=target;
    if (condition == 1)
        return ziel;
}*/

int call(int immediate, int pc) {
    int old_pc = pc;
    pc = immediate;
    //inemediate werte instruction
    push(pc);
    // pc = jump()
    //  instruction anrufen
    //execute ,....
    // kommst du zuruck

    return old_pc;
}