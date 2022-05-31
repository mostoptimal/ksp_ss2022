//
// Created by mohamad on 5/23/22.
//
#include <stdio.h>
#include "instructions.h"
#include "stack.h"

int a, b, res;

void add(void) {
    a = pop();
    b = pop();
    res = a + b;
    push(res);
}

void sub(void) {
    b = pop();
    a = pop();
    res = a - b;
    push(res);
}

void mul(void) {
    a = pop();
    b = pop();
    res = a * b;
    push(res);
}

void divid(void) {
    a = pop();
    b = pop();
    res = a / b;
    push(res);
}

void mod(void) {
    a = pop();
    b = pop();
    res = a % b;
    push(res);
}

void equal(void) {
    a = pop();
    b = pop();
    if (a == b) {//or v1-v2==0
        res = 1;//1 for True
    } else res = 0;//0 for False
    push(res);
}

void nequal(void) {
    a = pop();
    b = pop();
    if (a != b) res = 1;
    else res = 0;
    push(res);
}

void lessThan(void) {
    a = pop();
    b = pop();
    if (a < b) res = 1;
    else res = 0;
    push(res);
}

void lessEqual(void) {
    a = pop();
    b = pop();
    if (b >= a) res = 1;
    else res = 0;
    push(res);
}

void greaterThan(void) {
    a = pop();
    b = pop();
    if (a - b > 0) res = 1;
    else res = 0;
    push(res);
}

void greaterEqual(void) {
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

void readInt(void) {
    int input = 0;
    scanf("%d", &input);
    push(input);
}

void readChar(void) {
    char inputC;
    scanf("%c", &inputC);
    push(inputC);
}

int writeInt(void) {
    int output = pop();
    return output;
}

char writeChar(void) {
    char output = (char) pop();
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

int callInstruction(int immediate, int pc) {
    int old_pc = pc + 1; //frptr
    push(pc);
    pc = immediate;
    //inemediate werte instruction

    // pc = jump()
    //  instruction anrufen
    //execute ,....
    // kommst du zuruck
    return old_pc;
}

/*
char intToChar(int v) {
    char result = v + '0';
    return result;
}

int charToInt(char v) {
    int result = (int) v;
    return result;
}*/
