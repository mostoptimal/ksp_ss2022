//
// Created by mohamad on 7/8/22.
//
#include <stdio.h>
#include <stdlib.h>
#include "support.h"

ObjRef newPrimObject(int dataSize) {
    int objSize = sizeof(int) + sizeof(char) /*+ sizeof(ObjRef*)*/ + dataSize;
    if (dataSize < 8) objSize = sizeof(int) + sizeof(char) + sizeof(void *);
    //round up to multiples of 4
    //if(objSize%4>0) objSize = ((objSize/4)*4)+4;
    ObjRef objRef;
    objRef = malloc(objSize);
    objRef->size = objSize;
    //objRef->brokenHeart_flag = 0;
    return objRef;
}

void fatalError(char *msg) {
    printf("Fatal error: %s\n", msg);
    exit(1);
}