//
// Created by mohamad on 7/5/22.
//
#include <stdio.h>
#include <stdlib.h>
#include "stack.h"
#include "memory.h"
#include "opCodes.h"
#include "bigint/include/support.h"
#include "bigint/include/bigint.h"

ObjRef rvr = NULL;

ObjRef newCompoundObj(int numObjRefs) {
    ObjRef compObj;
    int objSize = sizeof(int) + sizeof(char) /*+ sizeof(ObjRef*)*/ + numObjRefs * sizeof(void *);
    //round up to multiples of 4
    //if(objSize%4>0) objSize = ((objSize/4)*4)+4;
    compObj = malloc(objSize);
    compObj->size = MSB | numObjRefs;
    //compObj->brokenHeart = 0;
    for (int i = 0; i < numObjRefs; i++) {
        ((ObjRef *) compObj->data)[i] = NULL;
    }
    return compObj;
}



