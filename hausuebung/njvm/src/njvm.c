#include <stdio.h>
#include <string.h>
#include "opCodes.h"
#include <stdlib.h>
#include <stdbool.h>
//#include "support.h"
//#include <bigint.h>
#include "./bigint/build/include/bigint.h"

/***** Declarations ***********/
int purge = 0;
int gcStats = 0;
int objCount = 0;
ObjRef returnValueRegister = NULL;
unsigned int *instPtr = NULL;
ObjRef *sdaPointer = NULL;
char *heapPtr = NULL;
char *targetPtr = NULL;
char *targetEndPtr = NULL;
char *freePointer = NULL;
unsigned int numberOfInstructions = 0;
unsigned int sdaSize = 0;
unsigned int stackSize = 64; // Default value
unsigned int heapSize = 8192;

int stackPointer = 0;
int framePointer = 0;
int programCounter = 0;
int debug = 0;
int breakpoint = -1;

/**** Struct ****/
typedef struct {
    bool isObjRef;
    union {
        ObjRef objRef;
        int number;
    } u;
} StackSlot;
/**** Struct ****/
/***** Declarations End********/

/***** Stack ******************/

StackSlot *stack;

void pushInt(int value) {
    //vale from vorlesung 1024 mal size of stacslot
    unsigned int defaultmaxStackSize = stackSize * 1024;
    int numOfObjects = (defaultmaxStackSize) / sizeof(StackSlot);
    if (stackPointer == numOfObjects) {
        fprintf(stderr, "ERROR: stack overflow - maximum stack size ");
        exit(EXIT_FAILURE);
    }
    StackSlot intAsObject;
    intAsObject.isObjRef = false;
    intAsObject.u.number = value;
    stack[stackPointer++] = intAsObject;
}

void pushObj(ObjRef objRef) {


    unsigned int defaultStackSize = stackSize * 1024;
    if (stackPointer == (defaultStackSize) / sizeof(StackSlot)) {
        fprintf(stderr, "ERROR: stack overflow - maximum stack size \n");
        exit(EXIT_FAILURE);
    }
    StackSlot s;
    s.isObjRef = true;
    s.u.objRef = objRef;
    stack[stackPointer++] = s;

}

StackSlot pop() {
    // Pop values are not being deleted, they are set to 0
    if (stackPointer == 0) {
        fprintf(stderr, "ERROR: stack underflow - no elements in stack");
        exit(EXIT_FAILURE);
    }
    StackSlot output = stack[--stackPointer];
    return output;
}

/***** Stack End **************/

/*****Memory management********/
void fatalError(char *msg) {
    fprintf(stderr, "%s", msg);
    exit(EXIT_FAILURE);
}

//switches the used half of the heap
void swap(void) {
    //change pointer from gc
    if (targetPtr == heapPtr) {
        targetPtr = targetEndPtr;
        //targetPtr = targetPtr + heapSize*1024/2;
        targetEndPtr = heapPtr + (heapSize * 1024);
    } else {
        targetPtr = heapPtr;
        targetEndPtr = heapPtr + (heapSize * 1024 / 2);
    }
    freePointer = targetPtr;
}

void copyMemory(char *dest, ObjRef src, int count) {
    //printf("dest %p\nsrc %p\ncount: %d\nisPrimitive: %d\nbrokenHeart: %d\n", dest, src, count, IS_PRIMITIVE(src), src->brokenHeart);
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
    //memcpy(freePointer, orig, origSize);
    copyMemory(freePointer, orig, origSize);
    ObjRef returnPtr = (ObjRef) freePointer;
    freePointer += origSize;
    return returnPtr;
}

ObjRef relocate(ObjRef orig) {
    ObjRef copy;
    if (orig == NULL) {
        copy = NULL;
    } else {
        if (orig->brokenHeart != 0) {
            copy = ((ObjRef *) orig->data)[0];
        } else {
            copy = copyObjectToFreeMem(orig);
            orig->brokenHeart = 1;
            ((ObjRef *) orig->data)[0] = copy;
        }
    }
    return copy;
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
    char *scan = targetPtr;
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

void purgePassiveHalfMemory(void) {
    char *killPtr;
    if (targetPtr == heapPtr) killPtr = targetEndPtr;
    else killPtr = heapPtr;
    memset(killPtr, 0x0, heapSize * 1024 / 2);
}

void countObj(void) {
    objCount = 0;
    char *countPtr = targetPtr;
    while (countPtr < freePointer) {
        countPtr += IS_PRIMITIVE((ObjRef) countPtr) ? ((ObjRef) countPtr)->size : sizeof(int) + sizeof(char *) +
                                                                                  GET_ELEMENT_COUNT((ObjRef) countPtr) *
                                                                                  sizeof(void *);
        objCount++;
    }
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
        printf("Memory used by vivid objects: %ld\n", (long) freePointer - (long) targetPtr);
        printf("Free memory: %ld\n\n", (long) targetEndPtr - (long) freePointer);
    }
}

ObjRef alloc(int dataSize) {
    //printf("free: %p, size: %d, target: %p, end: %p\n", freePointer, dataSize, freePointer+dataSize, targetEndPtr);
    if (freePointer + dataSize >= targetEndPtr) {
        garbageCollector();
        if (freePointer + dataSize >= targetEndPtr) {
            fprintf(stderr, "Error: Not enough heap space to allocate new object\n");
            exit(EXIT_FAILURE);
        }
    }
    ObjRef obj;
    obj = (ObjRef) freePointer;
    freePointer += dataSize;
    return obj;
}

ObjRef newCompoundObject(int numObjRefs) {
    ObjRef compObj;
    int objSize = sizeof(int) + sizeof(char) /*+ sizeof(ObjRef*)*/ + numObjRefs * sizeof(void *);
    //round up to multiples of 4
    //if(objSize%4>0) objSize = ((objSize/4)*4)+4;
    compObj = alloc(objSize);
    compObj->size = MSB | numObjRefs;
    compObj->brokenHeart = 0;
    for (int i = 0; i < numObjRefs; i++) {
        ((ObjRef *) compObj->data)[i] = NULL;
    }
    return compObj;
}

ObjRef newPrimObject(int dataSize) {
    int objSize = sizeof(int) + sizeof(char) + dataSize;
    if (dataSize < 8) objSize = sizeof(int) + sizeof(char) + sizeof(void *);
    //round up to multiples of 4
    //if(objSize%4>0) objSize = ((objSize/4)*4)+4;
    ObjRef objRef;
    objRef = alloc(objSize);
    objRef->size = objSize;
    objRef->brokenHeart = 0;
    return objRef;

}
/*****Memory management End****/

/***** Instructions **********/

void pushc(int x) {
    bigFromInt(x);
    pushObj(bip.res);
}

void add(void) {
    StackSlot n2 = pop();
    StackSlot n1 = pop();
    if (!(n1.isObjRef && n2.isObjRef)) {
        fprintf(stderr, "ERROR: n1 or n2 is not an object.");
        exit(EXIT_FAILURE);
    }
    if (n2.u.objRef == NULL || n1.u.objRef == NULL) {
        fprintf(stderr, "Error: Trying to access NIL-pointer\n");
        exit(EXIT_FAILURE);
    }
    if (!(IS_PRIMITIVE(n2.u.objRef) && IS_PRIMITIVE(n1.u.objRef))) {
        fprintf(stderr, "Error: Non-primitive object in arithmetic operation\n");
        exit(EXIT_FAILURE);
    }
    bip.op1 = n1.u.objRef;
    bip.op2 = n2.u.objRef;
    bigAdd();
    pushObj(bip.res);
}

void sub(void) {
    StackSlot n2 = pop();
    StackSlot n1 = pop();
    if (!(n1.isObjRef && n2.isObjRef)) {
        fprintf(stderr, "ERROR: n1 or n2 is not an object.");
        exit(EXIT_FAILURE);
    }
    if (n2.u.objRef == NULL || n1.u.objRef == NULL) {
        fprintf(stderr, "Error: Trying to access NIL-pointer\n");
        exit(EXIT_FAILURE);
    }
    if (!(IS_PRIMITIVE(n2.u.objRef) && IS_PRIMITIVE(n1.u.objRef))) {
        fprintf(stderr, "Error: Non-primitive object in arithmetic operation\n");
        exit(EXIT_FAILURE);
    }
    bip.op1 = n1.u.objRef;
    bip.op2 = n2.u.objRef;
    bigSub();
    pushObj(bip.res);
}

void mul(void) {
    StackSlot n2 = pop();
    StackSlot n1 = pop();
    if (!(n1.isObjRef && n2.isObjRef)) {
        fprintf(stderr, "ERROR: n1 or n2 is not an object.");
        exit(EXIT_FAILURE);
    }
    if (n2.u.objRef == NULL || n1.u.objRef == NULL) {
        fprintf(stderr, "Error: Trying to access NIL-pointer\n");
        exit(EXIT_FAILURE);
    }
    if (!(IS_PRIMITIVE(n2.u.objRef) && IS_PRIMITIVE(n1.u.objRef))) {
        fprintf(stderr, "Error: Non-primitive object in arithmetic operation\n");
        exit(EXIT_FAILURE);
    }
    bip.op1 = n1.u.objRef;
    bip.op2 = n2.u.objRef;
    bigMul();
    pushObj(bip.res);
}

void divid(void) {
    StackSlot n2 = pop();
    StackSlot n1 = pop();
    if (!(n1.isObjRef && n2.isObjRef)) {
        fprintf(stderr, "ERROR: n1 or n2 is not an object.");
        exit(EXIT_FAILURE);
    }
    if (n2.u.objRef == NULL || n1.u.objRef == NULL) {
        fprintf(stderr, "Error: Trying to access NIL-pointer\n");
        exit(EXIT_FAILURE);
    }
    if (!(IS_PRIMITIVE(n2.u.objRef) && IS_PRIMITIVE(n1.u.objRef))) {
        fprintf(stderr, "Error: Non-primitive object in arithmetic operation\n");
        exit(EXIT_FAILURE);
    }
    bip.op1 = n1.u.objRef;
    bip.op2 = n2.u.objRef;
    bigDiv();
    pushObj(bip.res);
}

void mod(void) {
    StackSlot n2 = pop();
    StackSlot n1 = pop();
    if (!(n1.isObjRef && n2.isObjRef)) {
        fprintf(stderr, "ERROR: n1 or n2 is not an object.");
        exit(EXIT_FAILURE);
    }
    if (n2.u.objRef == NULL || n1.u.objRef == NULL) {
        fprintf(stderr, "Error: Trying to access NIL-pointer\n");
        exit(EXIT_FAILURE);
    }
    if (!(IS_PRIMITIVE(n2.u.objRef) && IS_PRIMITIVE(n1.u.objRef))) {
        fprintf(stderr, "Error: Non-primitive object in arithmetic operation\n");
        exit(EXIT_FAILURE);
    }
    bip.op1 = n1.u.objRef;
    bip.op2 = n2.u.objRef;
    bigDiv();
    pushObj(bip.rem);
}

void rdint(void) {
    bigRead(stdin);
    pushObj(bip.res);
}

void wrint(void) {
    StackSlot n = pop();
    if (!n.isObjRef) {
        fprintf(stderr, "ERROR: n is not an object.");
        exit(EXIT_FAILURE);
    }
    if (n.u.objRef == NULL) {
        fprintf(stderr, "Error: Trying to access NIL-pointer\n");
        exit(EXIT_FAILURE);
    }
    if (!IS_PRIMITIVE(n.u.objRef)) {
        fprintf(stderr, "Error: Object is not a primitive object\n");
        exit(EXIT_FAILURE);
    }
    bip.op1 = n.u.objRef;
    //printf("writing: %d\n", bigToInt());
    bigPrint(stdout);
}

void rdchr(void) {
    char input;
    scanf(" %c", &input);
    //char inputBuffer[15];
    //fgets(inputBuffer, sizeof(inputBuffer), stdin);
    //sscanf(inputBuffer, "%c", &input);
    bigFromInt((int) input);
    pushObj(bip.res);
}

void wrchr(void) {
    StackSlot n = pop();
    if (!n.isObjRef) {
        fprintf(stderr, "ERROR: n is not an object.");
        exit(EXIT_FAILURE);
    }
    if (n.u.objRef == NULL) {
        fprintf(stderr, "Error: Trying to access NIL-pointer\n");
        exit(EXIT_FAILURE);
    }
    if (!IS_PRIMITIVE(n.u.objRef)) {
        fprintf(stderr, "Error: Object is not a primitive object\n");
        exit(EXIT_FAILURE);
    }
    bip.op1 = n.u.objRef;
    printf("%c", bigToInt());
}

void pushg(int value) {
    if (value >= sdaSize || value < 0) {
        fprintf(stderr, "Invalid position %d in static data area", value);
        exit(EXIT_FAILURE);
    }
    pushObj(*(sdaPointer + value));
}

void popg(int value) {
    if (value >= sdaSize || value < 0) {
        fprintf(stderr, "Invalid position %d in static data area", value);
        exit(EXIT_FAILURE);
    }
    StackSlot n = pop();
    if (!n.isObjRef) {
        fprintf(stderr, "Stackslot doesnt hold ObjRef");
        exit(EXIT_FAILURE);
    }
    *(sdaPointer + value) = n.u.objRef;
}

void asf(int value) {
    unsigned int defaultStackSize = stackSize * 1024;
    // Push frame pointer to stack
    pushInt(framePointer);
    framePointer = stackPointer;
    if (framePointer + value >= (defaultStackSize) / sizeof(StackSlot)) {
        fprintf(stderr, "Error: Stack overflow on stack frame allocation\n");
        exit(EXIT_FAILURE);
    }
    stackPointer = framePointer + value;
    //when creating space of local variables, they are all initialized with NULL
    for (int i = framePointer; i < stackPointer; i++) {
        StackSlot s;
        s.isObjRef = true;
        s.u.objRef = NULL;
        stack[i] = s;
    }
}

void rsf(void) {
    stackPointer = framePointer;
    StackSlot n = pop();
    if (n.isObjRef) {
        fprintf(stderr, "Error: Unexpected ObjRef instead of FP on stack");
        exit(EXIT_FAILURE);
    }
    framePointer = n.u.number;
}

void pushl(int value) {
    if (!stack[framePointer + value].isObjRef) {
        fprintf(stderr, "Error: Local variable to be pushed didnt contain object");
        exit(EXIT_FAILURE);
    }
    pushObj(stack[framePointer + value].u.objRef);
}

void popl(int value) {
    stack[framePointer + value] = pop();
}

void equal(void) {
    StackSlot n2 = pop();
    StackSlot n1 = pop();
    if (!(n1.isObjRef && n2.isObjRef)) {
        fprintf(stderr, "ERROR: n1 or n2 is not an object.");
        exit(EXIT_FAILURE);
    }
    if (n2.u.objRef == NULL || n1.u.objRef == NULL) {
        fprintf(stderr, "Error: Trying to access NIL-pointer\n");
        exit(EXIT_FAILURE);
    }
    if (!(IS_PRIMITIVE(n2.u.objRef) && IS_PRIMITIVE(n1.u.objRef))) {
        fprintf(stderr, "Error: Non-primitive object in arithmetic comparison\n");
        exit(EXIT_FAILURE);
    }
    bip.op1 = n1.u.objRef;
    bip.op2 = n2.u.objRef;
    if (bigCmp() == 0) bigFromInt(1);
    else bigFromInt(0);
    pushObj(bip.res);
}

void notEqual(void) {
    StackSlot n2 = pop();
    StackSlot n1 = pop();
    if (!(n1.isObjRef && n2.isObjRef)) {
        fprintf(stderr, "ERROR: n1 or n2 is not an object.");
        exit(EXIT_FAILURE);
    }
    if (n2.u.objRef == NULL || n1.u.objRef == NULL) {
        fprintf(stderr, "Error: Trying to access NIL-pointer\n");
        exit(EXIT_FAILURE);
    }
    if (!(IS_PRIMITIVE(n2.u.objRef) && IS_PRIMITIVE(n1.u.objRef))) {
        fprintf(stderr, "Error: Non-primitive object in arithmetic comparison\n");
        exit(EXIT_FAILURE);
    }
    bip.op1 = n1.u.objRef;
    bip.op2 = n2.u.objRef;
    if (bigCmp() != 0) bigFromInt(1);
    else bigFromInt(0);
    pushObj(bip.res);
}

void lessThan(void) {
    StackSlot n2 = pop();
    StackSlot n1 = pop();
    if (!(n1.isObjRef && n2.isObjRef)) {
        fprintf(stderr, "ERROR: n1 or n2 is not an object.");
        exit(EXIT_FAILURE);
    }
    if (n2.u.objRef == NULL || n1.u.objRef == NULL) {
        fprintf(stderr, "Error: Trying to access NIL-pointer\n");
        exit(EXIT_FAILURE);
    }
    if (!(IS_PRIMITIVE(n2.u.objRef) && IS_PRIMITIVE(n1.u.objRef))) {
        fprintf(stderr, "Error: Non-primitive object in arithmetic comparison\n");
        exit(EXIT_FAILURE);
    }
    bip.op1 = n1.u.objRef;
    bip.op2 = n2.u.objRef;
    if (bigCmp() < 0) bigFromInt(1);
    else bigFromInt(0);
    pushObj(bip.res);
}

void lessEqual(void) {
    StackSlot n2 = pop();
    StackSlot n1 = pop();
    if (!(n1.isObjRef && n2.isObjRef)) {
        fprintf(stderr, "ERROR: n1 or n2 is not an object.");
        exit(EXIT_FAILURE);
    }
    if (n2.u.objRef == NULL || n1.u.objRef == NULL) {
        fprintf(stderr, "Error: Trying to access NIL-pointer\n");
        exit(EXIT_FAILURE);
    }
    if (!(IS_PRIMITIVE(n2.u.objRef) && IS_PRIMITIVE(n1.u.objRef))) {
        fprintf(stderr, "Error: Non-primitive object in arithmetic comparison\n");
        exit(EXIT_FAILURE);
    }
    bip.op1 = n1.u.objRef;
    bip.op2 = n2.u.objRef;
    if (bigCmp() <= 0) bigFromInt(1);
    else bigFromInt(0);
    pushObj(bip.res);
}

void greaterThan(void) {
    StackSlot n2 = pop();
    StackSlot n1 = pop();
    if (!(n1.isObjRef && n2.isObjRef)) {
        fprintf(stderr, "ERROR: n1 or n2 is not an object.");
        exit(EXIT_FAILURE);
    }
    if (n2.u.objRef == NULL || n1.u.objRef == NULL) {
        fprintf(stderr, "Error: Trying to access NIL-pointer\n");
        exit(EXIT_FAILURE);
    }
    if (!(IS_PRIMITIVE(n2.u.objRef) && IS_PRIMITIVE(n1.u.objRef))) {
        fprintf(stderr, "Error: Non-primitive object in arithmetic comparison\n");
        exit(EXIT_FAILURE);
    }
    bip.op1 = n1.u.objRef;
    bip.op2 = n2.u.objRef;
    if (bigCmp() > 0) bigFromInt(1);
    else bigFromInt(0);
    pushObj(bip.res);
}

void greaterEqual(void) {
    StackSlot n2 = pop();
    StackSlot n1 = pop();
    if (!(n1.isObjRef && n2.isObjRef)) {
        fprintf(stderr, "ERROR: n1 or n2 is not an object.");
        exit(EXIT_FAILURE);
    }
    if (n2.u.objRef == NULL || n1.u.objRef == NULL) {
        fprintf(stderr, "Error: Trying to access NIL-pointer\n");
        exit(EXIT_FAILURE);
    }
    if (!(IS_PRIMITIVE(n2.u.objRef) && IS_PRIMITIVE(n1.u.objRef))) {
        fprintf(stderr, "Error: Non-primitive object in arithmetic comparison\n");
        exit(EXIT_FAILURE);
    }
    bip.op1 = n1.u.objRef;
    bip.op2 = n2.u.objRef;
    if (bigCmp() >= 0) bigFromInt(1);
    else bigFromInt(0);
    pushObj(bip.res);
}

void jmp(int target) {
    if (target >= numberOfInstructions) {
        fprintf(stderr, "Error: Jump target is higher than amount of instructions");
        exit(EXIT_FAILURE);
    }
    programCounter = target;
}

void brf(int target) {
    if (target >= numberOfInstructions) {
        fprintf(stderr, "Error: Jump target is higher than amount of instructions");
        exit(EXIT_FAILURE);
    }
    StackSlot n = pop();
    if (!n.isObjRef) {
        fprintf(stderr, "ERROR: n is not an int object.");
        exit(EXIT_FAILURE);
    }
    if (n.u.objRef == NULL) {
        fprintf(stderr, "Error: Trying to access NIL-pointer\n");
        exit(EXIT_FAILURE);
    }
    if (!IS_PRIMITIVE(n.u.objRef)) {
        fprintf(stderr, "Error: Object is not a primitive object\n");
        exit(EXIT_FAILURE);
    }
    bip.op1 = n.u.objRef;
    int condition = bigToInt();
    if (!(condition == 0 || condition == 1)) {
        fprintf(stderr, "Error: Condition for instruction BRF is not a boolean value");
        exit(EXIT_FAILURE);
    }
    if (condition == 0) programCounter = target;
}

void brt(int target) {
    if (target >= numberOfInstructions) {
        fprintf(stderr, "Error: Jump target is higher than amount of instructions");
        exit(EXIT_FAILURE);
    }
    StackSlot n = pop();
    if (!n.isObjRef) {
        fprintf(stderr, "ERROR: n is not an int object.");
        exit(EXIT_FAILURE);
    }
    if (n.u.objRef == NULL) {
        fprintf(stderr, "Error: Trying to access NIL-pointer\n");
        exit(EXIT_FAILURE);
    }
    if (!IS_PRIMITIVE(n.u.objRef)) {
        fprintf(stderr, "Error: Object is not a primitive object\n");
        exit(EXIT_FAILURE);
    }
    bip.op1 = n.u.objRef;
    int condition = bigToInt();
    if (!(condition == 0 || condition == 1)) {
        fprintf(stderr, "Error: Condition for instruction BRF is not a boolean value");
        exit(EXIT_FAILURE);
    }
    if (condition == 1) programCounter = target;
}

void call(int target) {
    if (target >= numberOfInstructions) {
        fprintf(stderr, "Error: Jump target for CALL is higher than amount of instructions");
        exit(EXIT_FAILURE);
    }
    pushInt(programCounter);
    programCounter = target;
}

void ret(void) {
    StackSlot n = pop();
    if (n.isObjRef) {
        fprintf(stderr, "Error: Unexpected ObjRef instead of RA on stack");
        exit(EXIT_FAILURE);
    }
    programCounter = n.u.number;
}

void drop(int value) {
    for (int i = 0; i < value; i++) {
        pop();
    }
}

void pushr(void) {
    pushObj(returnValueRegister);
}

void popr(void) {
    StackSlot n = pop();
    if (!n.isObjRef) {
        fprintf(stderr, "Error: Stackslot doesnt hold ObjRef\n");
        exit(EXIT_FAILURE);
    }
    returnValueRegister = n.u.objRef;
}

void duplicate(void) {
    //alternative: push(stack[stackPointer-1])
    StackSlot n = pop();
    if (n.isObjRef) {
        pushObj(n.u.objRef);
        pushObj(n.u.objRef);
    } else {
        pushInt(n.u.number);
        pushInt(n.u.number);
    }
}

void new(int value) {
    pushObj(newCompoundObject(value));
}

void getf(int value) {
    StackSlot n = pop();
    if (!n.isObjRef) {
        fprintf(stderr, "Error: Stackslot doesnt hold ObjRef\n");
        exit(EXIT_FAILURE);
    }
    if (n.u.objRef == NULL) {
        fprintf(stderr, "Error: Trying to access NIL-pointer\n");
        exit(EXIT_FAILURE);
    }
    if (IS_PRIMITIVE(n.u.objRef)) {
        fprintf(stderr, "Error: Object is not a compound object\n");
        exit(EXIT_FAILURE);
    }
    if (value >= GET_ELEMENT_COUNT(n.u.objRef) || value < 0) {
        fprintf(stderr, "Error: Index is higher than number of objects\n");
        exit(EXIT_FAILURE);
    }
    pushObj(((ObjRef *) n.u.objRef->data)[value]);
}

void putf(int val) {
    StackSlot value = pop();
    StackSlot object = pop();
    if (!(value.isObjRef && object.isObjRef)) {
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
    ((ObjRef *) object.u.objRef->data)[val] = value.u.objRef;
}

void newa(void) {
    StackSlot n = pop();
    if (!n.isObjRef) {
        fprintf(stderr, "Error: Stackslot doesnt hold ObjRef\n");
        exit(EXIT_FAILURE);
    }
    if (n.u.objRef == NULL) {
        fprintf(stderr, "Error: Trying to access NIL-pointer\n");
        exit(EXIT_FAILURE);
    }
    if (!IS_PRIMITIVE(n.u.objRef)) {
        fprintf(stderr, "Error: Object is not a primitive object\n");
        exit(EXIT_FAILURE);
    }
    bip.op1 = n.u.objRef;
    pushObj(newCompoundObject(bigToInt()));
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
    pushObj(((ObjRef *) array.u.objRef->data)[x]);
}

void putfa(void) {
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
    if (x >= GET_ELEMENT_COUNT(array.u.objRef) || x < 0) {
        fprintf(stderr, "Error: Index is higher than number of objects\n");
        exit(EXIT_FAILURE);
    }
    ((ObjRef *) array.u.objRef->data)[x] = value.u.objRef;
}

void getsz(void) {
    StackSlot n = pop();
    if (!n.isObjRef) {
        fprintf(stderr, "Error: Stackslot doesnt hold ObjRef\n");
        exit(EXIT_FAILURE);
    }
    if (n.u.objRef == NULL) {
        fprintf(stderr, "Error: Trying to access NIL-pointer\n");
        exit(EXIT_FAILURE);
    }
    if (IS_PRIMITIVE(n.u.objRef)) bigFromInt(-1);
    else bigFromInt(GET_ELEMENT_COUNT(n.u.objRef));
    pushObj(bip.res);
}

void pushn(void) {
    pushObj(NULL);
}

void refeq(void) {
    StackSlot n1 = pop();
    StackSlot n2 = pop();
    if (!(n1.isObjRef && n2.isObjRef)) {
        fprintf(stderr, "Error: Stackslot doesnt hold ObjRef\n");
        exit(EXIT_FAILURE);
    }
    if (n1.u.objRef == n2.u.objRef) bigFromInt(1);
    else bigFromInt(0);
    pushObj(bip.res);
}

void refne(void) {
    StackSlot n1 = pop();
    StackSlot n2 = pop();
    if (!(n1.isObjRef && n2.isObjRef)) {
        fprintf(stderr, "Error: Stackslot doesnt hold ObjRef\n");
        exit(EXIT_FAILURE);
    }
    if (n1.u.objRef == n2.u.objRef) bigFromInt(0);
    else bigFromInt(1);
    pushObj(bip.res);
}

/***** Instructions End ******/

/***** Execute ***************/

int decodeImmediate(unsigned int code) {
    return SIGN_EXTEND(IMMEDIATE(code));
}

void callInstruction(int immediatevalue, unsigned int opcode) {
    switch (opcode) {
        case HALT:
            if (gcStats != 0) garbageCollector();
            break;
        case PUSHC:
            pushc(immediatevalue);
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
            rdint();
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
            pushg(immediatevalue);
            break;
        case POPG:
            popg(immediatevalue);
            break;
        case ASF:
            asf(immediatevalue);
            break;
        case RSF:
            rsf();
            break;
        case PUSHL:
            pushl(immediatevalue);
            break;
        case POPL:
            popl(immediatevalue);
            break;
        case EQ:
            equal();
            break;
        case NE:
            notEqual();
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
            greaterEqual();
            break;
        case JMP:
            jmp(immediatevalue);
            break;
        case BRF:
            brf(immediatevalue);
            break;
        case BRT:
            brt(immediatevalue);
            break;
        case CALL:
            call(immediatevalue);
            break;
        case RET:
            ret();
            break;
        case DROP:
            drop(immediatevalue);
            break;
        case PUSHR:
            pushr();
            break;
        case POPR:
            popr();
            break;
        case DUP:
            duplicate();
            break;
        case NEW:
            new(immediatevalue);
            break;
        case GETF:
            getf(immediatevalue);
            break;
        case PUTF:
            putf(immediatevalue);
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
        default:
            fprintf(stderr, "Error: Unknown opcode %d", opcode);
            exit(EXIT_FAILURE);
    }
}

void printInstruction(int immediatevalue, unsigned int opcode, int printpc) {
    printf("%03d: ", printpc);
    switch (opcode) {
        case HALT:
            printf("HALT\n");
            break;
        case PUSHC:
            printf("PUSHC %d\n", immediatevalue);
            break;
        case ADD:
            printf("ADD\n");
            break;
        case SUB:
            printf("SUB\n");
            break;
        case MUL:
            printf("MUL\n");
            break;
        case DIV:
            printf("DIV\n");
            break;
        case MOD:
            printf("MOD\n");
            break;
        case RDINT:
            printf("RDINT\n");
            break;
        case WRINT:
            printf("WRINT\n");
            break;
        case RDCHR:
            printf("RDCHR\n");
            break;
        case WRCHR:
            printf("WRCHR\n");
            break;
        case PUSHG:
            printf("PUSHG %d\n", immediatevalue);
            break;
        case POPG:
            printf("POPG %d\n", immediatevalue);
            break;
        case ASF:
            printf("ASF %d\n", immediatevalue);
            break;
        case RSF:
            printf("RSF\n");
            break;
        case PUSHL:
            printf("PUSHL %d\n", immediatevalue);
            break;
        case POPL:
            printf("POPL %d\n", immediatevalue);
            break;
        case EQ:
            printf("EQ\n");
            break;
        case NE:
            printf("NE\n");
            break;
        case LT:
            printf("LT\n");
            break;
        case LE:
            printf("LE\n");
            break;
        case GT:
            printf("GT\n");
            break;
        case GE:
            printf("GE\n");
            break;
        case JMP:
            printf("JMP %d\n", immediatevalue);
            break;
        case BRF:
            printf("BRF %d\n", immediatevalue);
            break;
        case BRT:
            printf("BRT %d\n", immediatevalue);
            break;
        case CALL:
            printf("CALL %d\n", immediatevalue);
            break;
        case RET:
            printf("RET\n");
            break;
        case DROP:
            printf("DROP %d\n", immediatevalue);
            break;
        case PUSHR:
            printf("PUSHR\n");
            break;
        case POPR:
            printf("POPR\n");
            break;
        case DUP:
            printf("DUP\n");
            break;
        case NEW:
            printf("NEW %d\n", immediatevalue);
            break;
        case GETF:
            printf("GETF %d\n", immediatevalue);
            break;
        case PUTF:
            printf("PUTF %d\n", immediatevalue);
            break;
        case NEWA:
            printf("NEWA\n");
            break;
        case GETFA:
            printf("GETFA\n");
            break;
        case PUTFA:
            printf("PUTFA\n");
            break;
        case GETSZ:
            printf("GETSZ\n");
            break;
        case PUSHN:
            printf("PUSHN\n");
            break;
        case REFEQ:
            printf("REFEQ\n");
            break;
        case REFNE:
            printf("REFNE\n");
            break;
        default:
            fprintf(stderr, "Error: Unknown opcode %d", opcode);
            exit(EXIT_FAILURE);
    }
}

void printProgram(unsigned int *program) {
    int pc = 0;
    unsigned int opcode = 256;
    while (pc < numberOfInstructions) {
        unsigned int code = program[pc];
        opcode = OPCODE(code);
        int immediate = IMMEDIATE(code);
        printInstruction(immediate, opcode, pc);
        pc++;
    }
}
/***** Execute End************/

/***** File     ***********/
void readInput(char *path) {
    unsigned int defaultStackSize = stackSize * 1024;
    //open the file
    FILE *fp = NULL;
    if ((fp = fopen(path, "r")) == NULL) {
        fprintf(stderr, "Failed to open file '%s'", path);
        exit(EXIT_FAILURE);
    }

    unsigned int x = 0;
    //read the file format
    unsigned int format = 0;
    for (int i = 0; i <= 3; i++) {
        if (fread(&x, 1, 1, fp) == 0) {
            fprintf(stderr, "Error: failed to read input file");
            exit(EXIT_FAILURE);
        } else format |= x << (24 - 8 * i);
    }
    if (debug) printf("Format: %08x\n", format);
    if (format != 0x4e4a4246) {
        fprintf(stderr, "No NJBF!\n");
        exit(EXIT_FAILURE);
    }

    //check version
    if (fread(&x, 1, 4, fp) == 0) {
        fprintf(stderr, "Error: failed to read input file");
        exit(EXIT_FAILURE);
    }
    if (debug) printf("Version: %d\n", x);
    if (x != VERSION) {
        fprintf(stderr, "Version not compatible mit this NVJM!");
        exit(EXIT_FAILURE);
    }

    // Allocate memory for instructions
    if (fread(&x, 1, 4, fp) == 0) {
        fprintf(stderr, "Error: failed to read input file");
        exit(EXIT_FAILURE);
    }
    if (debug) printf("Instructions: %d\n", x);
    numberOfInstructions = x;
    instPtr = malloc(numberOfInstructions * sizeof(unsigned int));
    if (numberOfInstructions != 0 && instPtr == NULL) {
        fprintf(stderr, "Error: memory allocation for instructions failed");
        exit(EXIT_FAILURE);
    }

    // Allocate memory for static variables
    if (fread(&x, 1, 4, fp) == 0) {
        fprintf(stderr, "Error: failed to read input file");
        exit(EXIT_FAILURE);
    }
    if (debug) printf("Static variables: %d\n", x);
    sdaSize = x;
    sdaPointer = malloc(sdaSize * sizeof(ObjRef));
    if (sdaSize != 0 && sdaPointer == NULL) {
        fprintf(stderr, "Error: memory allocation for static data area failed");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < sdaSize; i++) sdaPointer[i] = NULL;

    //read all instructions and save in program memory
    if ((fread(instPtr, 4, numberOfInstructions, fp)) != numberOfInstructions) {
        fprintf(stderr, "Error reading instructions, count does not match.");
        exit(EXIT_FAILURE);
    }

    // Closing file
    if (fclose(fp) != 0) {
        fprintf(stderr, "Failed to close file.");
        exit(EXIT_FAILURE);
    }

    //Debug: Check read instructions
    if (debug) {
        for (int i = 0; i < numberOfInstructions; i++) {
            printf("Instruction %d: Hex %08x / Opcode %d / Immediate: %d\n", i, *(instPtr + i),
                   OPCODE(*(instPtr + i)), SIGN_EXTEND(IMMEDIATE(*(instPtr + i))));

        }
    }

    // Allocate heap
    if ((heapPtr = malloc(heapSize * 1024)) == NULL) {
        fprintf(stderr, "ERROR: Failed allocating heap.\n");
        exit(EXIT_FAILURE);
    }

    // Allocate stack
    if ((stack = malloc(defaultStackSize)) == NULL) {
        fprintf(stderr, "ERROR: Failed allocating stack.\n");
        exit(EXIT_FAILURE);
    }

    //set pointers
    targetPtr = heapPtr;
    targetEndPtr = targetPtr + heapSize * 1024 / 2;
    freePointer = targetPtr;
    //printf("heapPtr: %p, targetPtr: %p, targetEndPtr: %p, freePointer: %p", heapPtr, targetPtr, targetEndPtr, freePointer);


    //set all BIP registers to NULL
    bip.op1 = NULL;
    bip.op2 = NULL;
    bip.res = NULL;
    bip.rem = NULL;
}
/***** File End ***********/

/***** Debugger ***************/
char *getInput(void) {
    char inputBuffer[30];
    fgets(inputBuffer, sizeof(inputBuffer), stdin);
    static char command[30];
    sscanf(inputBuffer, "%s", command);
    return command;
}

void inspectObjRef(ObjRef obj, bool complex) {
    if (obj == NULL) {
        printf("NIL\n");
    } else if (IS_PRIMITIVE(obj)) {
        bip.op1 = obj;
        printf("Primitive object: value ");
        bigPrint(stdout);
        printf("\n");
    } else {
        int elementCount = GET_ELEMENT_COUNT(obj);
        printf("Compound object: holds %d references\n", elementCount);
        if (complex) {
            for (int i = 0; i < elementCount; i++) {
                printf("    ");
                inspectObjRef(((ObjRef *) obj->data)[i], false);
            }
        }
    }
}

void printSDA(void) {
    if (sdaSize == 0) printf("No static data in this program.\n");
    else printf("Static Data Area:\n");
    for (int i = 0; i < sdaSize; i++) {
        if (*(sdaPointer + i) == NULL) printf("%04d: NULL\n", i);
        else {
            printf("%04d: ", i);
            inspectObjRef(*(sdaPointer + i), true);
        }
    }
}

void printRVR(void) {
    printf("RVR: (first level of sub-objects if existing)\n");
    inspectObjRef(returnValueRegister, true);
}

void inspectStackSlot(void) {
    int inputloop = 1;
    while (inputloop) {
        printf("Please enter StackSlot number or type abort for no quitting.\n");
        char inputBuffer[30];
        fgets(inputBuffer, sizeof(inputBuffer), stdin);
        char *rest[30];
        int input;
        if ((input = strtol(inputBuffer, rest, 10)) > 0) {
            if (input >= stackPointer) {
                printf("Requested number is higher than elements on stack.\n");
            } else {
                inputloop = 0;
                if (stack[input].isObjRef) {
                    printf("Type 'complex' if you also want to print information object contained references:\n");
                    char *command = getInput();
                    bool b = false;
                    if (strcmp(command, "complex") == 0) b = true;
                    inspectObjRef(stack[input].u.objRef, b);
                } else {
                    printf("%04d: meta data, value: %d\n", input, stack[input].u.number);
                }
            }
        } else {
            sscanf(inputBuffer, "%s", *rest);
            if (strcmp(*rest, "abort") == 0) {
                inputloop = 0;
            } else {
                printf("Invalid input\n");
            }
        }
    }
}

void setBreakpoint(void) {
    int inputloop = 1;
    while (inputloop) {
        printf("Please enter breakpoint or type abort for no change (Setting a new breakpoint will remove the current breakpoint).\n");
        char inputBuffer[30];
        fgets(inputBuffer, sizeof(inputBuffer), stdin);
        char *rest[30];
        int input;
        if ((input = strtol(inputBuffer, rest, 10)) > 0) {
            inputloop = 0;
            breakpoint = input;
        } else {
            sscanf(inputBuffer, "%s", *rest);
            if (strcmp(*rest, "abort") == 0) {
                inputloop = 0;
            } else {
                printf("Breakpoint has to be a number greater than 0!\n");
            }
        }
    }
}

void removeBreakpoint(void) {
    if (breakpoint == -1) printf("No breakpoint to remove.\n");
    else {
        breakpoint = -1;
        printf("Removed breakpoint.\n");
    }
}

void printBreakpoint(void) {
    if (breakpoint == -1) printf("No breakpoint is set currently.\n");
    else printf("Breakpoint is currently set at: %d\n", breakpoint);
}

void printStack() {
    printf("Stack:\n");
    if (stackPointer == framePointer) printf("%04d: SP, FP ->\n", stackPointer);
    else printf("%04d: SP ->\n", stackPointer);
    for (int i = stackPointer - 1; i >= 0; i--) {
        if (stack[i].isObjRef) {
            if (stackPointer == framePointer) {
                printf("%04d:           ", i);
                inspectObjRef(stack[i].u.objRef, false);
            } else if (i == framePointer) {
                printf("%04d: FP -> ", i);
                inspectObjRef(stack[i].u.objRef, false);
            } else {
                printf("%04d:       ", i);
                inspectObjRef(stack[i].u.objRef, false);
            }
        } else {
            if (stackPointer == framePointer) printf("%04d:        -> meta data, value: %d\n", i, stack[i].u.number);
            else if (i == framePointer) printf("%04d: FP -> meta data, value: %d\n", i, stack[i].u.number);
            else printf("%04d:       meta data, value: %d\n", i, stack[i].u.number);
        }
    }
}

void debugProgram(int immediatevalue, int opcode, int printpc) {
    int debugloop = 1;
    while (debugloop) {
        printf("\n");
        printInstruction(immediatevalue, opcode, printpc);
        printf("Enter debug command (\"help\" to list all commands): \n");
        char *input = getInput();
        if (strcmp(input, "help") == 0) printf("%s", DEBUGHELP);
        else if (strcmp(input, "list") == 0) printProgram(instPtr);
        else if (strcmp(input, "quit") == 0) {
            printf("Quitting NJVM\n");
            exit(EXIT_SUCCESS);
        } else if (strcmp(input, "step") == 0) debugloop = 0;
        else if (strcmp(input, "ps") == 0) printStack();
        else if (strcmp(input, "pd") == 0) printSDA();
        else if (strcmp(input, "prvr") == 0) printRVR();
        else if (strcmp(input, "insp") == 0) inspectStackSlot();
        else if (strcmp(input, "run") == 0) {
            debug = 0;
            debugloop = 0;
        } else if (strcmp(input, "bs") == 0) setBreakpoint();
        else if (strcmp(input, "br") == 0) removeBreakpoint();
        else if (strcmp(input, "bl") == 0) printBreakpoint();
    }
}
/***** Debugger End************/
// Execute
void executeProgram(unsigned int *program) {
    programCounter = stackPointer = framePointer = 0;
    unsigned int opcode = 256;
    while (programCounter < numberOfInstructions && opcode != HALT) {
        unsigned int code = program[programCounter];
        opcode = OPCODE(code);
        int immediatevalue = decodeImmediate(code);
        //when the breakpoint is the next command, activate debug and remove the breakpoint
        if (breakpoint == programCounter) {
            debug = 1;
            breakpoint = -1;
        }
        if (debug) {
            debugProgram(immediatevalue, opcode, programCounter);
        }
        programCounter++;
        callInstruction(immediatevalue, opcode);
    }
}
// Execute
/***** main *******************/
int main(int argc, char *argv[]) {
    //checks if there was a command line arguments and responds to it
    if (argc >= 2) {
        for (int i = 1; i < argc - 1; i++) {
            if (strcmp(argv[i], "--version") == 0) {
                printf("Ninja Virtual Machine version %d (compiled %s at %s)\n", VERSION, __DATE__, __TIME__);
                return 0;
            } else if (strcmp(argv[i], "--help") == 0) {
                printf("Usage: ./njvm [option]\n");
                printf("  --help          Show this help and exits\n");
                printf("  --version       Show NJVM version and exits\n");
                printf("  [file]          Loads the file and runs it (filename cannot start with \"--\"\n");
                printf("  [file] --debug  Loads the file and runs it in debug mode\n");
                return 0;
            } else if (strcmp(argv[i], "--debug") == 0) debug = 1;
            else if (strcmp(argv[i], "--stack") == 0) {
                if (argc >= i + 1) {
                    stackSize = atoi(argv[++i]);
                } else {
                    fprintf(stderr, "Expected number after argument '--stack'");
                    exit(EXIT_FAILURE);
                }
            } else if (strcmp(argv[i], "--heap") == 0) {
                if (argc >= i + 1) {
                    heapSize = atoi(argv[++i]);
                } else {
                    fprintf(stderr, "Expected number after argument '--heap'");
                    exit(EXIT_FAILURE);
                }
            } else if (strcmp(argv[i], "--gcStats") == 0) gcStats = 1;
            else if (strcmp(argv[i], "--gcpurge") == 0) purge = 1;
            else {
                printf("unknown argument '%s', try './njvm --help'", argv[1]);
                return 1;
            }
        }
        printf("Ninja Virtual Machine started\n");
        char *path = argv[argc - 1];
        if (debug) {
            printf("----------------------------\n");
            printf("Reading file:\n\n");
        }
        readInput(path);
        if (debug) {
            printf("----------------------------\n");
            printf("Executing program:\n\n");
        }
        executeProgram(instPtr);
        if (debug) printf("----------------------------\n");

        printf("Ninja Virtual Machine stopped\n");
        free(instPtr);
        free(sdaPointer);
        free(heapPtr);
        free(stack);
        return 0;
    } else {
        //default behavior when no arguments where given
        printf("no arguments given, try './njvm --help'\n");
        return 1;
    }
}
