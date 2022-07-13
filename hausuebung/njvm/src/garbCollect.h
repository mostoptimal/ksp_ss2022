//
// Created by mohamad on 7/9/22.
//

#ifndef HAUSUEBUNG_GARBCOLLECT_H
#define HAUSUEBUNG_GARBCOLLECT_H

#include "./bigint/include/support.h"
#include "stack.h"
ObjRef allocate(int size);
void garbageCollector(void);
void scan(void);
void purgePassiveHalfMemory(void);
void countObj();
void root();
void swap();
ObjRef copyObjectToFreeMem(ObjRef orig);
ObjRef relocate(ObjRef ptr);

#endif //HAUSUEBUNG_GARBCOLLECT_H
