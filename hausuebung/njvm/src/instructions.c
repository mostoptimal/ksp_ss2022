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
    res = b / a;
    push(res);
}

void mod(void) {
    a = pop();
    b = pop();
    res = b % a;
    push(res);
}

void equal(void) {
    a = pop();
    b = pop();
    if (a == b) { push(1); }
    else { push(0); }
}

void nequal(void) {
    a = pop();
    b = pop();
    if (a != b) { push(1); }
    else { push(0); }
}

void lessThan(void) {
    a = pop();
    b = pop();
    if (b < a) { push(1); }
    else { push(0); }
}

void lessEqual(void) {
    a = pop();
    b = pop();
    if (b <= a) { push(1); }
    else { push(0); }
}

void greaterThan(void) {
    a = pop();
    b = pop();
    if (b > a) { push(1); }
    else { push(0); }
}

void greaterEqual(void) {
    a = pop();
    b = pop();
    if (b >= a) { push(1); }
    else { push(0); }
}

void readInt(void) {
    int input;
    scanf("%d", &input);
    push(input);
}

void readChar(void) {
    char inputC;
    scanf("%c", &inputC);
    push(inputC);
}

void writeInt(void) {
    int output = pop();
    printf("%d",output);
}

void writeChar(void) {
    char output =(char) pop();
    printf("%c",output);
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
