//
// Created by mohamad on 4/25/22.
//
#include <stdio.h>
#include <stdlib.h>
#include "stack.h"

#define MAX_ITEMS 100

//Stack Begin---------------------------------------------------------------------
int sp = 0;
unsigned int stack[MAX_ITEMS];
int frptr = 0;

void isEmpty(){
if(sp<0){

}
}


void push(int x) {
    if(!(sp>MAX_ITEMS)) {
        stack[sp] = x;
        sp++;
    }else {
        fprintf(stderr, "Error: StackOverflow\n");
        exit(EXIT_FAILURE);
    }
}

int pop(void) {
    if(!(sp<0)) {
        sp--;
        int tmp = (int) stack[sp];
        return tmp;
    }else{
        fprintf(stderr,"Error: stackUnderFlow\n");
        exit(EXIT_FAILURE);
    }
}

void asf(int n) {
    push(frptr);
    frptr = sp;
    sp = sp + n;
}

void rsf() {
    sp = frptr;
    frptr = pop();
}

void pushl(int immediate) {
    push(stack[frptr + immediate]);
}

void popl(int immediate) {
    stack[frptr + immediate] = pop();
}

/*void print_stack(void) {
    printf("\n   Stack\n");
    printf(".-------+--------.\n");
    for (int i = sp; i >= 0; i--) {
        if (i == sp)
            printf("|sp %3d| <empty>|\n", i);
        else
            printf("|%7d| %5d |\n", i, stack[i]);
    }
    printf("'-------+--------'\n\n");
}*/
//Stack End-----------------------------------------------------------------------