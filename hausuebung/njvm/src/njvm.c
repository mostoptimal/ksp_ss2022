#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "stack.h"
#include <stdbool.h>
#include "../bigint/build/include/bigint.h"
#include "../bigint/include/support.h"
#include "garbCollect.h"
#include "opCodes.h"

// todo  all  function  is i code from aufgaben

//#define STACK_SIZE 100;
FILE *file;
char *fileName;
int version;
int *programMemory;
//int *sda;
ObjRef *sdaPointer;
int pc = 0;
//unsigned int counter = 0;
//int fp = 0;
int numberOfInstructions;
int numberOfVariables;
unsigned int sda_size = 0;
// RVR
//int returnValue;
//ObjRef rvr = NULL;
//ObjRef rvr;

//StackSlot *stack;
unsigned int stackSize;

StackSlot op1, op2, result, object, v;

void halt(void) {
    exit(0);
}

ObjRef newCompoundObj(int numObjRefs) {
    ObjRef compObj;
    int objSize =  sizeof(char)+sizeof(int)  /*+ sizeof(ObjRef*)*/ + numObjRefs * sizeof(void *);
    compObj = allocate(objSize);
    compObj->size = MSB | numObjRefs;
    //all date to null intizialize wie in der aufgabe
    for (int i = 0; i < numObjRefs; i++) {
        ((ObjRef *) compObj->data)[i] = NULL;
    }
    return compObj;
}

void add(void) {
    //always prove of the op1 and op2 haben ein pointer from objekt and  wert
    op2 = pop();
    op1 = pop();
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
    pushObj(bip.res);
}

void sub(void) {
    op2 = pop();
    op1 = pop();
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
    pushObj(bip.res);
}

void mul(void) {
    op2 = pop();
    op1 = pop();
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
    pushObj(bip.res);
}

void divid(void) {
    op2 = pop();
    op1 = pop();
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
    pushObj(bip.res);
}

void mod(void) {
    op2 = pop();
    op1 = pop();
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
    pushObj(bip.rem);
}

void readInt(void) {
    bigRead(stdin);
    pushObj(bip.res);
}

void wrint(void) {
    result = pop();
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
    //printf("writing: %d\result", bigToInt());
    bigPrint(stdout);
}

void rdchr(void) {
    char eingabe;
    scanf(" %c", &eingabe);
    //char inputBuffer[15];
    //fgets(inputBuffer, sizeof(inputBuffer), stdin);
    //sscanf(inputBuffer, "%c", &input);
    bigFromInt((int) eingabe);
    pushObj(bip.res);
}

void wrchr(void) {
    result = pop();
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
    pushObj(bip.res);
}

void pushg(int element) {
    if (element >= sda_size || element < 0) {
        fprintf(stderr, "Invalid position %d in static data area", element);
        exit(EXIT_FAILURE);
    }
    pushObj(*(sdaPtr + element));
}

void popg(int element) {

    if (element >= sda_size || element < 0) {
        fprintf(stderr, "Invalid position %d in static data area", element);
        exit(EXIT_FAILURE);
    }
    result = pop();
    if (!result.isObjRef) {
        fprintf(stderr, "Stackslot doesnt hold ObjRef");
        exit(EXIT_FAILURE);
    }
    *(sdaPtr + element) = result.u.objRef;
}

//Allocate Release:
void asf(int n) {
    pushInt(fp);
    fp = sp;
    if (fp + n >= (stackSize * 1024) / sizeof(StackSlot)) {
        fprintf(stderr, "Error: Stack overflow on stack frame allocation\n");
        exit(EXIT_FAILURE);
    }
    sp = fp + n;
    for (int i = fp; i < sp; i++) {
        StackSlot s;
        s.isObjRef = true;
        s.u.objRef = NULL;
        stack[i] = s;
    }
}

void rsf(void) {
    sp = fp;
    result = pop();
    if (result.isObjRef) {
        fprintf(stderr, "Error: Unexpected ObjRef instead of FP on stack");
        exit(EXIT_FAILURE);
    }
    fp = result.u.number;
}

void pushl(int lokVar) {
    if (!stack[fp + lokVar].isObjRef) {
        fprintf(stderr, "Error: Local variable to be pushed didnt contain object");
        exit(EXIT_FAILURE);
    }
    pushObj(stack[fp + lokVar].u.objRef);
}

void popl(int lokVar) {
    stack[fp + lokVar] = pop();
}

//Logic
void equal(void) {
    op2 = pop();
    op1 = pop();
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
    pushObj(bip.res);
}

void nequal(void) {
    op1 = pop();
    op2 = pop();
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
    pushObj(bip.res);
}

void lessThan(void) {
    op1 = pop();
    op2 = pop();
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
    pushObj(bip.res);
}

void lessEqual(void) {
    op2 = pop();
    op1 = pop();
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
    pushObj(bip.res);
}

void greaterThan(void) {
    op2 = pop();
    op1 = pop();
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
    pushObj(bip.res);
}

void greatEqual(void) {
    op2 = pop();
    op1 = pop();
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
    pushObj(bip.res);
}

void branchIfTrue(int target) {
    if (target >= numberOfInstructions) {
        fprintf(stderr, "Error: Jump target is higher than amount of instructions");
        exit(EXIT_FAILURE);
    }
    result = pop();
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
        pc = target;
    }
}

void branchIfFalse(int target) {
    if (target >= numberOfInstructions) {
        fprintf(stderr, "Error: Jump target is higher than amount of instructions");
        exit(EXIT_FAILURE);
    }
    result = pop();
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
    if (condition == 0) pc = target;
}

//jump
void jmp(int target) {
    if (target >= numberOfInstructions) {
        fprintf(stderr, "Error: Jump target is higher than amount of instructions");
        exit(EXIT_FAILURE);
    }
    pc = target;
}

//call & return
void call(int target) {
    if (target >= numberOfInstructions) {
        fprintf(stderr, "Error: Jump target for CALL is higher than amount of instructions");
        exit(EXIT_FAILURE);
    }
    pushInt(pc);
    pc = target;
}

void ret() {
    result = pop();
    if (result.isObjRef) {
        fprintf(stderr, "Error: Unexpected ObjRef instead of RA on stack");
        exit(EXIT_FAILURE);
    }
    pc = result.u.number;
}

void drop(int n) {
    int ct;
    for (ct = 0; ct < n; ct++) {
        pop();
    }
}

void pushr(void) {
    pushObj(rvr);
}

void popr(void) {
    result = pop();
    if (!result.isObjRef) {
        fprintf(stderr, "Error: Stackslot doesnt hold ObjRef\n");
        exit(EXIT_FAILURE);
    }
    rvr = result.u.objRef;
}

void dup(void) {
    result = pop();
    if (!result.isObjRef) {
        pushInt(result.u.number);
        pushInt(result.u.number);

    } else {
        pushObj(result.u.objRef);
        pushObj(result.u.objRef);
    }
}

// Hausuebung 02 Instructions
void new(int val) {
    pushObj(newCompoundObj(val));
}

void getf(int val) {
    result = pop();
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
    pushObj(((ObjRef *) result.u.objRef->data)[val]);
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
    result = pop();
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
    pushObj(newCompoundObj(bigToInt()));
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
    pushObj(((ObjRef *) array.u.objRef->data)[x]);
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

void pushn(void) {
    pushObj(NULL);
}

void refeq(void) {
    op1 = pop();
    op2 = pop();
    if (!(op1.isObjRef && op2.isObjRef)) {
        fprintf(stderr, "Error: Stackslot doesnt hold ObjRef\n");
        exit(EXIT_FAILURE);
    }
    if (op1.u.objRef == op2.u.objRef) bigFromInt(1);
    else bigFromInt(0);
    pushObj(bip.res);
}

void refne(void) {
    op1 = pop();
    op2 = pop();
    if (!(op1.isObjRef && op2.isObjRef)) {
        fprintf(stderr, "Error: Stackslot doesnt hold ObjRef\n");
        exit(EXIT_FAILURE);
    }
    if (op1.u.objRef == op2.u.objRef) bigFromInt(0);
    else bigFromInt(1);
    pushObj(bip.res);
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
            halt();
            break;
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
        IR = programMemory[pc];
        pc = pc + 1;
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
            sdaPtr[i] = NULL;
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
