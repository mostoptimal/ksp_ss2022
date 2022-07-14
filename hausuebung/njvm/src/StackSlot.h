//
// Created by mohamad on 7/13/22.
//

#ifndef HAUSUEBUNG_STACKSLOT_H
#define HAUSUEBUNG_STACKSLOT_H

#include <stdbool.h>

typedef struct {
    bool isObjRef;
    union {
        ObjRef objRef;
        int number;
    } u;
} StackSlot;

#endif //HAUSUEBUNG_STACKSLOT_H
