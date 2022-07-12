#include <stdio.h>
#include "opCodes.h"
extern ObjRef rvr;
int sp = 0;
//unsigned int stackSize = 100;

extern unsigned int stackSize;

typedef struct {
    bool isObjRef;
    union {
        ObjRef objRef;
        int number;
    } u;
} StackSlot;


StackSlot *stack;
extern ObjRef *sdaPtr;

void pushInt(int x) {
    if (sp == (stackSize * 1024) / sizeof(StackSlot)) {
        fprintf(stderr, "ERROR: stack overflow - maximum stack size exceeded");
        exit(EXIT_FAILURE);
    }
    StackSlot s;
    s.isObjRef = false;
    s.u.number = x;
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


StackSlot pop() {
    if (sp == 0) {
        fprintf(stderr, "ERROR: stack underflow - no elements in stack");
        exit(EXIT_FAILURE);
    }
    return stack[--sp];
}
