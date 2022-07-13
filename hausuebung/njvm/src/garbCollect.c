//
// Created by mohamad on 7/9/22.
//
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "garbCollect.h"
#include "opCodes.h"
#include "../bigint/build/include/bigint.h"


char *freePtr;
char *targetEndPtr;
char *targetPtr;
char *heapPtr;
unsigned int heapSize;
unsigned int sdaSize;
ObjRef *sdaPtr;

int purge = 0;
int gcStats = 0;
int objCount = 0;

ObjRef allocate(int size) {
    if (freePtr + size >= targetEndPtr) {
        garbageCollector();
        if (freePtr + size >= targetEndPtr) {
            fprintf(stderr, "Error: Not enough heap space to allocate new object\n");
            exit(EXIT_FAILURE);
        }
    }
    ObjRef obj;
    obj = (ObjRef) freePtr;
    freePtr += size;
    return obj;
}

void garbageCollector(void) {
    if (gcStats != 0) {
        printf("Garbage Collector triggered!\n");
        int oldCount = objCount;
        countObj();
        printf("New objects since last run: %d\n", objCount - oldCount);
    }
    swap();
    root();
    scan();
    if (purge != 0) purgePassiveHalfMemory();
    if (gcStats != 0) {
        countObj();
        printf("Number of vivid objects: %d\n", objCount);
        printf("Memory used by vivid objects: %ld\n", (long) freePtr - (long) targetPtr);
        printf("Free memory: %ld\n\n", (long) targetEndPtr - (long) freePtr);
    }
}

void swap() {
    if (targetPtr == heapPtr) {
        targetPtr = targetEndPtr;
        targetEndPtr = heapPtr + (heapSize * 1024);
    } else {
        targetPtr = heapPtr;
        targetEndPtr = heapPtr + (heapSize * 1024 / 2);
    }
    freePtr = targetPtr;
}

void root(void) {
    for (int i = 0; i < sdaSize; i++) {
        sdaPtr[i] = relocate(sdaPtr[i]);
    }
    for (int i = 0; i < sp; i++) {
        if (stack[i].isObjRef) {
            stack[i].u.objRef = relocate(stack[i].u.objRef);
        }
    }
    rvr = relocate(rvr);
    bip.op1 = relocate(bip.op1);
    bip.op2 = relocate(bip.op2);
    bip.res = relocate(bip.res);
    bip.rem = relocate(bip.rem);
}

void scan(void) {
    char *scan = targetPtr;
    while (scan < freePtr) {
        ObjRef compObject = (ObjRef) scan;
        if (!IS_PRIMITIVE(compObject)) {
            for (int i = 0; i < GET_ELEMENT_COUNT(compObject); i++) {
                ((ObjRef *) compObject->data)[i] = relocate(((ObjRef *) compObject->data)[i]);
            }
            scan += sizeof(int) + sizeof(char) + GET_ELEMENT_COUNT(compObject) * (sizeof(ObjRef *));
        } else {
            scan += compObject->size;
        }
    }
}

ObjRef relocate(ObjRef orig) {
    ObjRef copy;
    if (orig == NULL) {
        copy = NULL;
    } else {
        if (orig->brokenHeart_flag != 0) {
            copy = ((ObjRef *) orig->data)[0];
        } else {
            copy = copyObjectToFreeMem(orig);
            orig->brokenHeart_flag = 1;
            ((ObjRef *) orig->data)[0] = copy;
        }
    }
    return copy;
}

void purgePassiveHalfMemory(void) {
    char *killPtr;
    if (targetPtr == heapPtr) killPtr = targetEndPtr;
    else killPtr = heapPtr;
    memset(killPtr, 0x0, heapSize * 1024 / 2);
}

void copyMemory(char *dest, ObjRef src, unsigned int count) {
    for (int i = 0; i < count; i++) {
        dest[i] = ((char *) src)[i];
    }
}

void countObj(void) {
    objCount = 0;
    char *countPtr = targetPtr;
    while (countPtr < freePtr) {
        countPtr += IS_PRIMITIVE((ObjRef) countPtr) ? ((ObjRef) countPtr)->size : sizeof(int) + sizeof(char *) +
                                                                                  GET_ELEMENT_COUNT((ObjRef) countPtr) *
                                                                                  sizeof(void *);
        objCount++;
    }
}

ObjRef copyObjectToFreeMem(ObjRef orig) {
    unsigned int origSize;
    if (IS_PRIMITIVE(orig)) {
        origSize = orig->size;
    } else {
        origSize = sizeof(int) + sizeof(char) + GET_ELEMENT_COUNT(orig) * (sizeof(ObjRef *));
    }
    copyMemory(freePtr, orig, origSize);
    ObjRef returnPtr = (ObjRef) freePtr;
    freePtr += origSize;
    return returnPtr;
}

