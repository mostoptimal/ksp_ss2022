//
// Created by mohamad on 4/25/22.
//
#include <stdio.h>
#include <stdlib.h>
#include "stack.h"

#define MAX_ITEMS 100

//Stack Begin---------------------------------------------------------------------
int sp = 0;
int stack[MAX_ITEMS];
int frptr = 0;


void push(int x) {
    if (sp < MAX_ITEMS) {
        stack[sp] = x;
        sp++;
    } else {
        fprintf(stderr, "Error: StackOverflow\n");
        exit(EXIT_FAILURE);
    }
}

int pop(void) {
    int tmp;
    if ((sp != 0)) {
        tmp = stack[sp];
        sp--;
    } else {
        fprintf(stderr, "Error: stackUnderFlow\n");
        exit(EXIT_FAILURE);
    }
    return tmp;
}

void asf(int n) {
    push(frptr);
    frptr = sp;
    sp = sp + n;
}

void rsf() {
    //Rücksprung zu main:
    // SP := FP ;
    // FP := FP-alt ;
    sp = frptr;
    frptr = pop();
}

void pushl(int immediate) {
    push(stack[frptr + immediate]);
}

void popl(int immediate) {
    stack[frptr + immediate] = pop();
}

//Stack End-----------------------------------------------------------------------