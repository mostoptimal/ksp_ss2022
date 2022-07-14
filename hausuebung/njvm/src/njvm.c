#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "./bigint/build/include/bigint.h"
#include "opCodes.h"
#include "./bigint/include/support.h"

#include "StackSlot.h"

// todo  all  function  is i code from aufgaben


FILE *file;
char *fileName;
int version;
int *programMemory;
int progCounter = 0;
int numberOfInstructions;
int numberOfVariables;
unsigned int sda_size = 0;

ObjRef returnValueRegister = NULL; // RVR

unsigned int stackSize = 64;

StackSlot object, v;

/** Stack */
int stackPointer = 0;
int framePointer = 0;
StackSlot *stack;
ObjRef *sdaPointer;

void pushInt(int someThing) {
    /* Check if the Stack already full N = How Big is Stack / How Big is one Object */
    if (stackPointer == (stackSize * 1024) / sizeof(StackSlot)) {
        fprintf(stderr, "ERROR: stack overflow");
        exit(EXIT_FAILURE);
    }
    // Stack is not full
    StackSlot s;
    s.isObjRef = false;
    s.u.number = someThing;
    stack[stackPointer++] = s;
}

void push_Object(ObjRef objRef) {
    /* Check if the Stack already full N = How Big is Stack / How Big is one Object */
    if (stackPointer == (stackSize * 1024) / sizeof(StackSlot)) {
        fprintf(stderr, "ERROR: stack overflow - maximum stack size exceeded\n");
        exit(EXIT_FAILURE);
    }
    // Stack is not full
    StackSlot s;
    s.isObjRef = true;
    s.u.objRef = objRef;
    stack[stackPointer++] = s;
}

StackSlot pop(void) {
    /* Check if Stack is empty */
    if (stackPointer == 0) {
        fprintf(stderr, "ERROR: stack underflow - Stack is empty");
        exit(EXIT_FAILURE);
    }
    // not empty
    return stack[--stackPointer];
}


/******************* Garbage Collector ***************/
char *freePointer;
char *targetEndPtr;
char *targetPointer;
char *heapPointer;
unsigned int heapSize = 8192;
unsigned int sdaSize;
int purge = 0;
int gcStats = 0;
int objCount = 0;

void copyMemory(char *dest, ObjRef src, unsigned int count) {
    for (int i = 0; i < count; i++) {
        dest[i] = ((char *) src)[i];
    }
}

ObjRef copyObjectToFreeMem(ObjRef orig) {
    int origSize;
    if (IS_PRIMITIVE(orig)) {
        origSize = orig->size;
    } else {
        origSize = sizeof(int) + sizeof(char) + GET_ELEMENT_COUNT(orig) * (sizeof(ObjRef *));
    }
    copyMemory(freePointer, orig, origSize);
    ObjRef returnPtr = (ObjRef) freePointer;
    freePointer += origSize;
    return returnPtr;
}

void countObj(void) {
    objCount = 0;
    char *countPtr = targetPointer;
    while (countPtr < freePointer) {
        countPtr += IS_PRIMITIVE((ObjRef) countPtr) ? ((ObjRef) countPtr)->size : sizeof(int) + sizeof(char *) +
                                                                                  GET_ELEMENT_COUNT((ObjRef) countPtr) *
                                                                                  sizeof(void *);
        objCount++;
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
    if (targetPointer == heapPointer) killPtr = targetEndPtr;
    else killPtr = heapPointer;
    memset(killPtr, 0x0, heapSize * 1024 / 2);
}

void swap(void) {
    if (targetPointer == heapPointer) {
        targetPointer = targetEndPtr;
        targetEndPtr = heapPointer + (heapSize * 1024);
    } else {
        targetPointer = heapPointer;
        targetEndPtr = heapPointer + (heapSize * 1024 / 2);
    }
    freePointer = targetPointer;
}

void root(void) {
    for (int i = 0; i < sdaSize; i++) {
        sdaPointer[i] = relocate(sdaPointer[i]);
    }
    for (int i = 0; i < stackPointer; i++) {
        if (stack[i].isObjRef) {
            stack[i].u.objRef = relocate(stack[i].u.objRef);
        }
    }
    returnValueRegister = relocate(returnValueRegister);
    bip.op1 = relocate(bip.op1);
    bip.op2 = relocate(bip.op2);
    bip.res = relocate(bip.res);
    bip.rem = relocate(bip.rem);
}

void scan(void) {
    char *scan = targetPointer;
    while (scan < freePointer) {
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

void collectGarbage(void) {
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
        printf("Memory used by vivid objects: %ld\n", (long) freePointer - (long) targetPointer);
        printf("Free memory: %ld\n\n", (long) targetEndPtr - (long) freePointer);
    }
}

ObjRef allocate(int size) {
    if (freePointer + size >= targetEndPtr) {
        collectGarbage();
        if (freePointer + size >= targetEndPtr) {
            fprintf(stderr, "Error: Not enough heap space to allocate new object\n");
            exit(EXIT_FAILURE);
        }
    }
    ObjRef obj;
    obj = (ObjRef) freePointer;
    freePointer += size;
    return obj;
}

void fatalError(char *msg) {
    fprintf(stderr, "%s", msg);
    exit(EXIT_FAILURE);
}


/**** Garbage Collector End */

ObjRef newPrimObject(int dataSize) {
    int objSize = sizeof(int) + sizeof(char) + dataSize;
    if (dataSize < 8)
        objSize = sizeof(int) + sizeof(char) + sizeof(void *);
    ObjRef objRef;
    objRef = allocate(objSize);
    objRef->size = objSize;
    objRef->brokenHeart_flag = 0;
    return objRef;
}

ObjRef newCompoundObj(int numObjRefs) {
    ObjRef compObj;
    int objSize = sizeof(char) + sizeof(int) + numObjRefs * sizeof(void *);
    compObj = allocate(objSize);
    compObj->size = MSB | numObjRefs;
    //all date to null initialize wie in der aufgabe
    for (int i = 0; i < numObjRefs; i++) {
        ((ObjRef *) compObj->data)[i] = NULL;
    }
    return compObj;
}

/****** */
void add(void) {
    //always prove of the op1 and op2 haben ein pointer from objekt and  wert
    StackSlot op2 = pop();
    StackSlot op1 = pop();
    if (!(op1.isObjRef && op2.isObjRef)) {
        fprintf(stderr, "ERROR: op1 or op2  not an object.");
        exit(EXIT_FAILURE);
    }
    if (op2.u.objRef == NULL || op1.u.objRef == NULL) {
        fprintf(stderr, "ERROR: Trying to access NIL-pointer\n");
        exit(EXIT_FAILURE);
    }
    if (!(IS_PRIMITIVE(op2.u.objRef) && IS_PRIMITIVE(op1.u.objRef))) {
        fprintf(stderr, "ERROR: Non-primitive object in arithmetic operation\n");
        exit(EXIT_FAILURE);
    }
    bip.op1 = op1.u.objRef;
    bip.op2 = op2.u.objRef;
    bigAdd();
    push_Object(bip.res);
}

void sub(void) {
    StackSlot op2 = pop();
    StackSlot op1 = pop();
    if (!(op1.isObjRef && op2.isObjRef)) {
        fprintf(stderr, "ERROR: op1 or op2  not an object.");
        exit(EXIT_FAILURE);
    }
    if (op2.u.objRef == NULL || op1.u.objRef == NULL) {
        fprintf(stderr, "ERROR: Trying to access NIL-pointer\n");
        exit(EXIT_FAILURE);
    }
    if (!(IS_PRIMITIVE(op2.u.objRef) && IS_PRIMITIVE(op1.u.objRef))) {
        fprintf(stderr, "ERROR: Non-primitive object in arithmetic operation\n");
        exit(EXIT_FAILURE);
    }
    bip.op1 = op1.u.objRef;
    bip.op2 = op2.u.objRef;
    bigSub();
    push_Object(bip.res);
}

void mul(void) {
    StackSlot op2 = pop();
    StackSlot op1 = pop();
    if (!(op1.isObjRef && op2.isObjRef)) {
        fprintf(stderr, "ERROR: op1 or op2  not an object.");
        exit(EXIT_FAILURE);
    }
    if (op2.u.objRef == NULL || op1.u.objRef == NULL) {
        fprintf(stderr, "Error: Trying to access NIL-pointer\n");
        exit(EXIT_FAILURE);
    }
    if (!(IS_PRIMITIVE(op2.u.objRef) && IS_PRIMITIVE(op1.u.objRef))) {
        fprintf(stderr, "Error: Non-primitive object in arithmetic operation\n");
        exit(EXIT_FAILURE);
    }
    bip.op1 = op1.u.objRef;
    bip.op2 = op2.u.objRef;
    bigMul();
    push_Object(bip.res);
}

void divid(void) {
    StackSlot op2 = pop();
    StackSlot op1 = pop();
    if (!(op1.isObjRef && op2.isObjRef)) {
        fprintf(stderr, "ERROR: op1 or op2 is not an object.");
        exit(EXIT_FAILURE);
    }
    if (op2.u.objRef == NULL || op1.u.objRef == NULL) {
        fprintf(stderr, "Error: Trying to access NIL-pointer\n");
        exit(EXIT_FAILURE);
    }
    if (!(IS_PRIMITIVE(op2.u.objRef) && IS_PRIMITIVE(op1.u.objRef))) {
        fprintf(stderr, "Error: Non-primitive object in arithmetic operation\n");
        exit(EXIT_FAILURE);
    }
    bip.op1 = op1.u.objRef;
    bip.op2 = op2.u.objRef;
    bigDiv();
    push_Object(bip.res);
}

void mod(void) {
    StackSlot op2 = pop();
    StackSlot op1 = pop();
    if (!(op1.isObjRef && op2.isObjRef)) {
        fprintf(stderr, "ERROR: op1 or op2 is not an object.");
        exit(EXIT_FAILURE);
    }
    if (op2.u.objRef == NULL || op1.u.objRef == NULL) {
        fprintf(stderr, "Error: Trying to access NIL-pointer\n");
        exit(EXIT_FAILURE);
    }
    if (!(IS_PRIMITIVE(op2.u.objRef) && IS_PRIMITIVE(op1.u.objRef))) {
        fprintf(stderr, "Error: Non-primitive object in arithmetic operation\n");
        exit(EXIT_FAILURE);
    }
    bip.op1 = op1.u.objRef;
    bip.op2 = op2.u.objRef;
    bigDiv();
    push_Object(bip.rem);
}

void readInt(void) {
    bigRead(stdin);
    push_Object(bip.res);
}

void wrint(void) {
    StackSlot result = pop();
    if (!result.isObjRef) {
        fprintf(stderr, "ERROR: result is not an object.");
        exit(EXIT_FAILURE);
    }
    if (result.u.objRef == NULL) {
        fprintf(stderr, "Error: Trying to access NIL-pointer\n");
        exit(EXIT_FAILURE);
    }
    if (!IS_PRIMITIVE(result.u.objRef)) {
        fprintf(stderr, "Error: Object is not a primitive object\n");
        exit(EXIT_FAILURE);
    }
    bip.op1 = result.u.objRef;
    bigPrint(stdout);
}

void rdchr(void) {
    char eingabe;
    scanf(" %c", &eingabe);
    //char inputBuffer[15];
    //fgets(inputBuffer, sizeof(inputBuffer), stdin);
    //sscanf(inputBuffer, "%c", &input);
    bigFromInt((int) eingabe);
    push_Object(bip.res);
}

void wrchr(void) {
    StackSlot result = pop();
    if (!result.isObjRef) {
        fprintf(stderr, "ERROR: result is not an object.");
        exit(EXIT_FAILURE);
    }
    if (result.u.objRef == NULL) {
        fprintf(stderr, "Error: Trying to access NIL-pointer\n");
        exit(EXIT_FAILURE);
    }
    if (!IS_PRIMITIVE(result.u.objRef)) {
        fprintf(stderr, "Error: Object is not a primitive object\n");
        exit(EXIT_FAILURE);
    }
    bip.op1 = result.u.objRef;
    printf("%c", bigToInt());
}

void pushc(int element) {
    bigFromInt(element);
    push_Object(bip.res);
}

void pushg(int element) {
    if (element >= sda_size || element < 0) {
        fprintf(stderr, "Invalid position %d in static data area", element);
        exit(EXIT_FAILURE);
    }
    push_Object(*(sdaPointer + element));
}

void popg(int element) {

    if (element >= sda_size || element < 0) {
        fprintf(stderr, "Invalid position %d in static data area", element);
        exit(EXIT_FAILURE);
    }
    StackSlot result = pop();
    if (!result.isObjRef) {
        fprintf(stderr, "Stackslot doesnt hold ObjRef");
        exit(EXIT_FAILURE);
    }
    *(sdaPointer + element) = result.u.objRef;
}

void asf(int num) {
    pushInt(framePointer);
    framePointer = stackPointer;
    if (framePointer + num >= (stackSize * 1024) / sizeof(StackSlot)) {
        fprintf(stderr, "Error: Stack overflow on stack frame allocation\n");
        exit(EXIT_FAILURE);
    }
    stackPointer = framePointer + num;
    for (int i = framePointer; i < stackPointer; i++) {
        StackSlot s;
        s.isObjRef = true;
        s.u.objRef = NULL;
        stack[i] = s;
    }
}

void rsf(void) {
    stackPointer = framePointer;
    StackSlot result = pop();
    if (result.isObjRef) {
        fprintf(stderr, "Error: Unexpected ObjRef instead of FP on stack");
        exit(EXIT_FAILURE);
    }
    framePointer = result.u.number;
}

void pushl(int lokVar) {
    if (!stack[framePointer + lokVar].isObjRef) {
        fprintf(stderr, "Error: Local variable to be pushed didnt contain object");
        exit(EXIT_FAILURE);
    }
    push_Object(stack[framePointer + lokVar].u.objRef);
}

void popl(int lokVar) {
    stack[framePointer + lokVar] = pop();
}

//Logic
void equal(void) {
    StackSlot op2 = pop();
    StackSlot op1 = pop();
    if (!(op1.isObjRef && op2.isObjRef)) {
        fprintf(stderr, "ERROR: Operand1 or Operand2 is not an object.");
        exit(EXIT_FAILURE);
    }
    if (op2.u.objRef == NULL || op1.u.objRef == NULL) {
        fprintf(stderr, "Error: Trying to access NIL-pointer\n");
        exit(EXIT_FAILURE);
    }
    if (!(IS_PRIMITIVE(op2.u.objRef) && IS_PRIMITIVE(op1.u.objRef))) {
        fprintf(stderr, "Error: Non-primitive object in arithmetic comparison\n");
        exit(EXIT_FAILURE);
    }
    bip.op1 = op1.u.objRef;
    bip.op2 = op2.u.objRef;
    if (bigCmp() == 0) bigFromInt(1);
    else bigFromInt(0);
    push_Object(bip.res);
}

void nequal(void) {
    StackSlot op1 = pop();
    StackSlot op2 = pop();
    if (!(op1.isObjRef && op2.isObjRef)) {
        fprintf(stderr, "ERROR: Operand1 or Operand2 is not an object.");
        exit(EXIT_FAILURE);
    }
    if (op2.u.objRef == NULL || op1.u.objRef == NULL) {
        fprintf(stderr, "Error: Trying to access NIL-pointer\n");
        exit(EXIT_FAILURE);
    }
    if (!(IS_PRIMITIVE(op2.u.objRef) && IS_PRIMITIVE(op1.u.objRef))) {
        fprintf(stderr, "Error: Non-primitive object in arithmetic comparison\n");
        exit(EXIT_FAILURE);
    }
    bip.op1 = op1.u.objRef;
    bip.op2 = op2.u.objRef;
    if (bigCmp() != 0) bigFromInt(1);
    else bigFromInt(0);
    push_Object(bip.res);
}

void lessThan(void) {
    StackSlot op1 = pop();
    StackSlot op2 = pop();
    if (!(op1.isObjRef && op2.isObjRef)) {
        fprintf(stderr, "ERROR: Operand1 or Operand2 is not an object.");
        exit(EXIT_FAILURE);
    }
    if (op2.u.objRef == NULL || op1.u.objRef == NULL) {
        fprintf(stderr, "Error: Trying to access NIL-pointer\n");
        exit(EXIT_FAILURE);
    }
    if (!(IS_PRIMITIVE(op2.u.objRef) && IS_PRIMITIVE(op1.u.objRef))) {
        fprintf(stderr, "Error: Non-primitive object in arithmetic comparison\n");
        exit(EXIT_FAILURE);
    }
    bip.op1 = op1.u.objRef;
    bip.op2 = op2.u.objRef;
    if (bigCmp() < 0) bigFromInt(1);
    else bigFromInt(0);
    push_Object(bip.res);
}

void lessEqual(void) {
    StackSlot op2 = pop();
    StackSlot op1 = pop();
    if (!(op1.isObjRef && op2.isObjRef)) {
        fprintf(stderr, "ERROR: op1 or op2 is not an object.");
        exit(EXIT_FAILURE);
    }
    if (op2.u.objRef == NULL || op1.u.objRef == NULL) {
        fprintf(stderr, "Error: Trying to access NIL-pointer\n");
        exit(EXIT_FAILURE);
    }
    if (!(IS_PRIMITIVE(op2.u.objRef) && IS_PRIMITIVE(op1.u.objRef))) {
        fprintf(stderr, "Error: Non-primitive object in arithmetic comparison\n");
        exit(EXIT_FAILURE);
    }
    bip.op1 = op1.u.objRef;
    bip.op2 = op2.u.objRef;
    if (bigCmp() <= 0) bigFromInt(1);
    else bigFromInt(0);
    push_Object(bip.res);
}

void greaterThan(void) {
    StackSlot op2 = pop();
    StackSlot op1 = pop();
    if (!(op1.isObjRef && op2.isObjRef)) {
        fprintf(stderr, "ERROR: op1 or op2 is not an object.");
        exit(EXIT_FAILURE);
    }
    if (op2.u.objRef == NULL || op1.u.objRef == NULL) {
        fprintf(stderr, "Error: Trying to access NIL-pointer\n");
        exit(EXIT_FAILURE);
    }
    if (!(IS_PRIMITIVE(op2.u.objRef) && IS_PRIMITIVE(op1.u.objRef))) {
        fprintf(stderr, "Error: Non-primitive object in arithmetic comparison\n");
        exit(EXIT_FAILURE);
    }
    bip.op1 = op1.u.objRef;
    bip.op2 = op2.u.objRef;
    if (bigCmp() > 0) bigFromInt(1);
    else bigFromInt(0);
    push_Object(bip.res);
}

void greatEqual(void) {
    StackSlot op2 = pop();
    StackSlot op1 = pop();
    if (!(op1.isObjRef && op2.isObjRef)) {
        fprintf(stderr, "ERROR: op1 or op2 is not an object.");
        exit(EXIT_FAILURE);
    }
    if (op2.u.objRef == NULL || op1.u.objRef == NULL) {
        fprintf(stderr, "Error: Trying to access NIL-pointer\n");
        exit(EXIT_FAILURE);
    }
    if (!(IS_PRIMITIVE(op2.u.objRef) && IS_PRIMITIVE(op1.u.objRef))) {
        fprintf(stderr, "Error: Non-primitive object in arithmetic comparison\n");
        exit(EXIT_FAILURE);
    }
    bip.op1 = op1.u.objRef;
    bip.op2 = op2.u.objRef;
    if (bigCmp() >= 0) bigFromInt(1);
    else bigFromInt(0);
    push_Object(bip.res);
}

void branchIfTrue(int target) {
    if (target >= numberOfInstructions) {
        fprintf(stderr, "Error: Jump target is higher than amount of instructions");
        exit(EXIT_FAILURE);
    }
    StackSlot result = pop();
    if (!result.isObjRef) {
        fprintf(stderr, "ERROR: result is not an int object.");
        exit(EXIT_FAILURE);
    }
    if (result.u.objRef == NULL) {
        fprintf(stderr, "Error: Trying to access NIL-pointer\n");
        exit(EXIT_FAILURE);
    }
    if (!IS_PRIMITIVE(result.u.objRef)) {
        fprintf(stderr, "Error: Object is not a primitive object\n");
        exit(EXIT_FAILURE);
    }
    bip.op1 = result.u.objRef;
    int condition = bigToInt();
    if (!(condition == 0 || condition == 1)) {
        fprintf(stderr, "Error: Condition for instruction BRF is not a boolean value");
        exit(EXIT_FAILURE);
    }
    if (condition == 1) {
        progCounter = target;
    }
}

void branchIfFalse(int target) {
    if (target >= numberOfInstructions) {
        fprintf(stderr, "Error: Jump target is higher than amount of instructions");
        exit(EXIT_FAILURE);
    }
    StackSlot result = pop();
    if (!result.isObjRef) {
        fprintf(stderr, "ERROR: n is not an int object.");
        exit(EXIT_FAILURE);
    }
    if (result.u.objRef == NULL) {
        fprintf(stderr, "Error: Trying to access NIL-pointer\n");
        exit(EXIT_FAILURE);
    }
    if (!IS_PRIMITIVE(result.u.objRef)) {
        fprintf(stderr, "Error: Object is not a primitive object\n");
        exit(EXIT_FAILURE);
    }
    bip.op1 = result.u.objRef;
    int condition = bigToInt();
    if (!(condition == 0 || condition == 1)) {
        fprintf(stderr, "Error: Condition for instruction BRF is not a boolean value");
        exit(EXIT_FAILURE);
    }
    if (condition == 0) progCounter = target;
}

//jump
void jmp(int target) {
    if (target >= numberOfInstructions) {
        fprintf(stderr, "Error: Jump target is higher than amount of instructions");
        exit(EXIT_FAILURE);
    }
    progCounter = target;
}

//call & return
void call(int target) {
    if (target >= numberOfInstructions) {
        fprintf(stderr, "Error: Jump target for CALL is higher than amount of instructions");
        exit(EXIT_FAILURE);
    }
    pushInt(progCounter);
    progCounter = target;
}

void ret(void) {
    StackSlot result = pop();
    if (result.isObjRef) {
        fprintf(stderr, "Error: Unexpected ObjRef instead of RA on stack");
        exit(EXIT_FAILURE);
    }
    progCounter = result.u.number;
}

void drop(int n) {
    int ct;
    for (ct = 0; ct < n; ct++) {
        pop();
    }
}

void pushr(void) {
    push_Object(returnValueRegister);
}

void popr(void) {
    StackSlot result = pop();
    if (!result.isObjRef) {
        fprintf(stderr, "Error: Stackslot doesnt hold ObjRef\n");
        exit(EXIT_FAILURE);
    }
    returnValueRegister = result.u.objRef;
}

void dup(void) {
    StackSlot result = pop();
    if (!result.isObjRef) {
        pushInt(result.u.number);
        pushInt(result.u.number);

    } else {
        push_Object(result.u.objRef);
        push_Object(result.u.objRef);
    }
}

// Hausuebung 02 Instructions
void new(int val) {
    push_Object(newCompoundObj(val));
}

void getf(int val) {
    StackSlot result = pop();
    if (!result.isObjRef) {
        fprintf(stderr, "Error: Stackslot doesnt hold ObjRef\n");
        exit(EXIT_FAILURE);
    }
    if (result.u.objRef == NULL) {
        fprintf(stderr, "Error: Trying to access NIL-pointer\n");
        exit(EXIT_FAILURE);
    }
    if (IS_PRIMITIVE(result.u.objRef)) {
        fprintf(stderr, "Error: Object is not a compound object\n");
        exit(EXIT_FAILURE);
    }
    if (val >= GET_ELEMENT_COUNT(result.u.objRef) || val < 0) {
        fprintf(stderr, "Error: Index is higher than number of objects\n");
        exit(EXIT_FAILURE);
    }
    push_Object(((ObjRef *) result.u.objRef->data)[val]);
}

void putf(int val) {
    v = pop();
    object = pop();
    if (!(v.isObjRef && object.isObjRef)) {
        fprintf(stderr, "Error: Stackslot doesnt hold ObjRef\n");
        exit(EXIT_FAILURE);
    }
    if (object.u.objRef == NULL) {
        fprintf(stderr, "Error: Trying to access NIL-pointer\n");
        exit(EXIT_FAILURE);
    }
    if (IS_PRIMITIVE(object.u.objRef)) {
        fprintf(stderr, "Error: Object is not a compound object\n");
        exit(EXIT_FAILURE);
    }
    if (val >= GET_ELEMENT_COUNT(object.u.objRef) || val < 0) {
        fprintf(stderr, "Error: Index is higher than number of objects\n");
        exit(EXIT_FAILURE);
    }
    ((ObjRef *) object.u.objRef->data)[val] = v.u.objRef;
}

void newa(void) {
    StackSlot result = pop();
    if (!result.isObjRef) {
        fprintf(stderr, "Error: Stackslot doesnt hold ObjRef\n");
        exit(EXIT_FAILURE);
    }
    if (result.u.objRef == NULL) {
        fprintf(stderr, "Error: Trying to access NIL-pointer\n");
        exit(EXIT_FAILURE);
    }
    if (!IS_PRIMITIVE(result.u.objRef)) {
        fprintf(stderr, "Error: Object is not a primitive object\n");
        exit(EXIT_FAILURE);
    }
    //[Stack] ... number_elements --> [Stack]... array
    bip.op1 = result.u.objRef;
    push_Object(newCompoundObj(bigToInt()));
}

void getfa(void) {
    StackSlot index = pop();
    StackSlot array = pop();
    if (!(index.isObjRef && array.isObjRef)) {
        fprintf(stderr, "Error: Stackslot doesnt hold ObjRef\n");
        exit(EXIT_FAILURE);
    }
    if (index.u.objRef == NULL || array.u.objRef == NULL) {
        fprintf(stderr, "Error: Trying to access NIL-pointer\n");
        exit(EXIT_FAILURE);
    }
    if (!IS_PRIMITIVE(index.u.objRef)) {
        fprintf(stderr, "Error: Index is not a primitive object\n");
        exit(EXIT_FAILURE);
    }
    if (IS_PRIMITIVE(array.u.objRef)) {
        fprintf(stderr, "Error: Array is not a compound object\n");
        exit(EXIT_FAILURE);
    }
    bip.op1 = index.u.objRef;
    int x = bigToInt();
    if (x >= GET_ELEMENT_COUNT(array.u.objRef) || x < 0) {
        fprintf(stderr, "Error: Index out of bounds\n");
        exit(EXIT_FAILURE);
    }
    //[Stack]... object --> [Stack]... value
    push_Object(((ObjRef *) array.u.objRef->data)[x]);
}

void putfa(void) {
    // element von stack value, index , array
    StackSlot value = pop();
    StackSlot index = pop();
    StackSlot array = pop();
    if (!(value.isObjRef && index.isObjRef && array.isObjRef)) {
        fprintf(stderr, "Error: Stackslot doesnt hold ObjRef\n");
        exit(EXIT_FAILURE);
    }
    if (index.u.objRef == NULL || array.u.objRef == NULL) {
        fprintf(stderr, "Error: Trying to access NIL-pointer\n");
        exit(EXIT_FAILURE);
    }
    if (IS_PRIMITIVE(array.u.objRef)) {
        fprintf(stderr, "Error: Array is not a compound object\n");
        exit(EXIT_FAILURE);
    }
    if (!IS_PRIMITIVE(index.u.objRef)) {
        fprintf(stderr, "Error: Index is not a primitive object\n");
        exit(EXIT_FAILURE);
    }
    bip.op1 = index.u.objRef;
    int x = bigToInt();
    //check if x out of array range (bigger or smaller)
    if (x >= GET_ELEMENT_COUNT(array.u.objRef) || x < 0) {
        fprintf(stderr, "Error: Index is higher than number of objects\n");
        exit(EXIT_FAILURE);
    }
    // [Stack]... array index value --> [Stack]...
    ((ObjRef *) array.u.objRef->data)[x] = value.u.objRef;
}

void getsz(void) {
    StackSlot result = pop();
    if (!result.isObjRef) {
        fprintf(stderr, "Error: Stackslot doesnt hold ObjRef\n");
        exit(EXIT_FAILURE);
    }
    if (result.u.objRef == NULL) {
        fprintf(stderr, "Error: Trying to access NIL-pointer\n");
        exit(EXIT_FAILURE);
    }
    if (IS_PRIMITIVE(result.u.objRef)) bigFromInt(-1);
    else bigFromInt(GET_ELEMENT_COUNT(result.u.objRef));
    push_Object(bip.res);
}

void pushn(void) {
    push_Object(NULL);
}

void refeq(void) {
    StackSlot op1 = pop();
    StackSlot op2 = pop();
    if (!(op1.isObjRef && op2.isObjRef)) {
        fprintf(stderr, "Error: Stackslot doesnt hold ObjRef\n");
        exit(EXIT_FAILURE);
    }
    if (op1.u.objRef == op2.u.objRef) bigFromInt(1);
    else bigFromInt(0);
    push_Object(bip.res);
}

void refne(void) {
    StackSlot op1 = pop();
    StackSlot op2 = pop();
    if (!(op1.isObjRef && op2.isObjRef)) {
        fprintf(stderr, "Error: Stackslot doesnt hold ObjRef\n");
        exit(EXIT_FAILURE);
    }
    if (op1.u.objRef == op2.u.objRef)
        bigFromInt(0);
    else
        bigFromInt(1);
    push_Object(bip.res);
}


void executable(unsigned int IR) {
    unsigned int instruction = IR >> 24;
    int immediate = SIGN_EXTEND(IMMEDIATE(IR));
    switch (instruction) {
        case PUSHC:
            pushc(immediate);
            break;
        case ADD:
            add();
            break;
        case SUB:
            sub();
            break;
        case MUL:
            mul();
            break;
        case DIV:
            divid();
            break;
        case MOD:
            mod();
            break;
        case RDINT:
            readInt();
            break;
        case WRINT:
            wrint();
            break;
        case RDCHR:
            rdchr();
            break;
        case WRCHR:
            wrchr();
            break;
        case PUSHG:
            pushg(immediate);
            break;
        case POPG:
            popg(immediate);
            break;
        case ASF:
            asf(immediate);
            break;
        case RSF:
            rsf();
            break;
        case PUSHL:
            pushl(immediate);
            break;
        case POPL:
            popl(immediate);
            break;
        case EQ:
            equal();
            break;
        case NE:
            nequal();
            break;
        case LT:
            lessThan();
            break;
        case LE:
            lessEqual();
            break;
        case GT:
            greaterThan();
            break;
        case GE:
            greatEqual();
            break;
        case BRF:
            branchIfFalse(immediate);
            break;
        case BRT:
            branchIfTrue(immediate);
            break;
        case JMP:
            jmp(immediate);
            break;
        case CALL:
            call(immediate);
            break;
        case RET:
            ret();
            break;
        case DROP:
            drop(immediate);
            break;
        case PUSHR:
            pushr();
            break;
        case POPR:
            popr();
            break;
        case DUP:
            dup();
            break;
        case NEW:
            new(immediate);
            break;
        case GETF:
            getf(immediate);
            break;
        case PUTF:
            putf(immediate);
            break;
        case NEWA:
            newa();
            break;
        case GETFA:
            getfa();
            break;
        case PUTFA:
            putfa();
            break;
        case GETSZ:
            getsz();
            break;
        case PUSHN:
            pushn();
            break;
        case REFEQ:
            refeq();
            break;
        case REFNE:
            refne();
            break;
        case HALT:
            exit(0);
        default:
            printf("not Ninja Instruction!");
            break;
    }
}

/**
void printInst(unsigned int IR) {
    unsigned int instruction = IR >> 24;
    switch (instruction) {

        case PUSHC:
            printf(" %03d:\t PUSHC\t%d\n", counter, SIGN_EXTEND(IMMEDIATE(IR)));
            break;

        case ADD:
            printf(" %03d:\t ADD\n", counter);

            break;

        case SUB:
            printf(" %03d:\t SUB\n", counter);
            break;

        case MUL:
            printf(" %03d:\t MUL\n", counter);
            break;

        case DIV:
            printf(" %03d:\t DIV\n", counter);
            break;

        case MOD:
            printf(" %03d:\t MOD\n", counter);
            break;

        case RDINT:
            printf(" %03d:\t RDINT\n", counter);
            break;

        case WRINT:
            printf(" %03d:\t WRINT\n", counter);
            break;

        case RDCHR:
            printf(" %03d:\t RDCHR\n", counter);
            break;

        case WRCHR:
            printf(" %03d:\t WRCHR\n", counter);
            break;

        case HALT:
            printf(" %03d:\t halt\n", counter);
            break;

        case PUSHG:
            printf(" %03d:\t PUSHG\t%d\n", counter, SIGN_EXTEND(IMMEDIATE(IR)));
            break;

        case POPG:
            printf(" %03d:\t POPG\t%d\n", counter, SIGN_EXTEND(IMMEDIATE(IR)));
            break;

        case ASF:
            printf(" %03d:\t ASF\t%d\n", counter, SIGN_EXTEND(IMMEDIATE(IR)));
            break;

        case RSF:
            printf(" %03d:\t RSF\t%d\n", counter, SIGN_EXTEND(IMMEDIATE(IR)));
            break;

        case PUSHL:
            printf(" %03d:\t PUSHL\t%d\n", counter, SIGN_EXTEND(IMMEDIATE(IR)));
            break;

        case POPL:
            printf(" %03d:\t POPL\t%d\n", counter, SIGN_EXTEND(IMMEDIATE(IR)));
            break;

        case EQ:
            printf(" %03d:\t EQ\t%d\n", counter, SIGN_EXTEND(IMMEDIATE(IR)));
            break;
        case NE:
            printf(" %03d:\t NE\t%d\n", counter, SIGN_EXTEND(IMMEDIATE(IR)));
            break;

        case LT:
            printf(" %03d:\t LT\t%d\n", counter, SIGN_EXTEND(IMMEDIATE(IR)));
            break;

        case LE:
            printf(" %03d:\t LE\t%d\n", counter, SIGN_EXTEND(IMMEDIATE(IR)));
            break;

        case GT:
            printf(" %03d:\t GT\t%d\n", counter, SIGN_EXTEND(IMMEDIATE(IR)));
            break;
        case CALL:
            printf(" %03d:\t call\t%d\n", counter, SIGN_EXTEND(IMMEDIATE(IR)));
            break;

        case RET:
            printf(" %03d:\t GE\t%d\n", counter, SIGN_EXTEND(IMMEDIATE(IR)));
            break;

        case DROP:
            printf(" %03d:\t drop\t%d\n", counter, SIGN_EXTEND(IMMEDIATE(IR)));
            break;
        case PUSHR:
            printf(" %03d:\t pushr greatEqual\t%d\n", counter, SIGN_EXTEND(IMMEDIATE(IR)));
            break;

        case POPR:
            printf(" %03d:\t popr\t%d\n", counter, SIGN_EXTEND(IMMEDIATE(IR)));
            break;
        default:
            printf("not knowed instruction\n");
    }
}
*/

void execute() {
    int IR;
    do {
        IR = programMemory[progCounter];
        progCounter = progCounter + 1;
        executable(IR);
    } while (IR >> 24 != HALT);

}

void workWithFile(int argc, char *argv[]) {
    if (argv[1] != NULL) {
        fileName = argv[1];
        file = fopen(fileName, "r");
        //printf("after fopen() before if NULL \n");
        if (file == NULL) {
            printf("Error: cannot open code file '%s'\n", fileName);
            exit(1);
        }
        /** 1st Step: read the format of Binary File "NJBF" */
        //1) Read the first 4 bytes of the file.
        char *njbf = (char *) malloc(sizeof(char) * 4);
        fread(njbf, sizeof(char), 4, file);
        if (strcmp(njbf, "NJBF") != 0) {
            //printf("Format is NOT Correct :) \n");
            exit(1);
        }

        /** 2nd Step: Read the version number. **********/
        //2) Read the version number.
        fread(&version, sizeof(int), 1, file);
        //TODO: Version always under Observation
        if (version < 2) {
            printf("Error: file %s has wrong version number %d\n", fileName, version);
            exit(1);
        }

        /** 3rd Step: Read the Number of Instructions */
        // 3) Read the number of instructions.
        fread(&numberOfInstructions, sizeof(int), 1, file);
        programMemory = malloc(numberOfInstructions * sizeof(unsigned int));

        /** 4th Step: read the Number of Global-Variables in SDA */
        //4) Read the number of variables in the static data area.
        fread(&numberOfVariables, sizeof(int), 1, file);
        if (numberOfVariables > 0) {
            //SDA Allocate for Public Variables
            sda_size = numberOfVariables;
            sdaPointer = malloc(sda_size * sizeof(ObjRef));
        }
        for (int i = 0; i < sda_size; i++) {
            sdaPointer[i] = NULL;
        }

        /**5th Step: reading the rest of File */
        //5) Read the rest of the file into the memory allocated in step 3).
        fread(programMemory, sizeof(unsigned int), numberOfInstructions, file);
        execute();
        /**6th Step: Closing the File */
        if (fclose(file) != 0) {
            perror("ERROR (fclose)");
        }
    }
}

int main(int argc, char *argv[]) {

    printf("Ninja Virtual Machine started\n");

    workWithFile(argc, argv);

    printf("Ninja Virtual Machine stopped\n");
    return 0;
}
