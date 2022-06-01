
#include <stdio.h>

int stack[100000];
int sp = 0;


void push(int element) {
    stack[sp] = element;
    sp++;
}

int pop() {
    int value;
    value = stack[--sp];
    return value;
}
