
#ifndef STACK_H
#define STACK_H

#include "StackSlot.h"

extern ObjRef rvr;
extern int sp;
extern int fp;
extern StackSlot *stack;
extern ObjRef *sdaPtr;
void pushInt(int someThing);
void pushObj(ObjRef objRef);
StackSlot pop(void);

#endif //STACK_H
