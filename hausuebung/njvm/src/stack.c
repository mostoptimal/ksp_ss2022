//
// Created by mohamad on 4/25/22.
//
#include <stdio.h>
#include "stack.h"

#define MAX_ITEMS 100

//Stack Begin---------------------------------------------------------------------
int sp = 0;
int stack[MAX_ITEMS];
int frptr=0;
void push(unsigned int x) {
    printf("\n%4s-> pushing %d onto stack @sp %d\n", __func__, x, sp);
    if (sp > MAX_ITEMS) {
        printf("Error Stack Overflow :( \n");
    } else {
        stack[sp] = x;
        printf("\n-%4s-> inc stack pointer %d -> \n", __func__, sp);
        sp++;
        printf("%d\n", sp);
    }
}

int pop(void) {
    int tmp;
    if (sp == 0)
        printf("Error Stack underFlow :( !!\n");

    else {
        printf("\n%4s-> dec stack pointer %d -> \n", __func__, sp);
        sp--;
        printf("%d\n", sp);
        tmp = stack[sp];
        printf("\n%4s-> popping %d from stack @sp %d\n", __func__, tmp, sp);
    }
    return tmp;
}
void asf( int n){
    push(frptr);
    frptr = sp;
    sp = sp + n;
}
void rsf(){
    sp = frptr;
    frptr = pop();
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