//
// Created by mohamad on 7/13/22.
//
#include <stdio.h>
#include <stdlib.h>
#include "stack.h"


int sp = 0;
int fp = 0;
unsigned int stackSize;
StackSlot *stack;
ObjRef *sdaPtr;

void pushInt(int someThing) {
    if (sp == (stackSize * 1024) / sizeof(StackSlot)) {
        fprintf(stderr, "ERROR: stack overflow - maximum stack size exceeded");
        exit(EXIT_FAILURE);
    }
    StackSlot s;
    s.isObjRef = false;
    s.u.number = someThing;
    stack[sp++] = s;
}

void pushObj(ObjRef objRef) {
    if (sp == (stackSize * 1024) / sizeof(StackSlot)) {
        fprintf(stderr, "ERROR: stack overflow - maximum stack size exceeded\n");
        exit(EXIT_FAILURE);
    }
    StackSlot s;
    s.isObjRef = true;
    s.u.objRef = objRef;
    stack[sp++] = s;
}


StackSlot pop(void) {
    if (sp == 0) {
        fprintf(stderr, "ERROR: stack underflow - no elements in stack");
        exit(EXIT_FAILURE);
    }
    return stack[--sp];
}