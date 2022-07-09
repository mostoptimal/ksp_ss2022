//
// Created by mohamad on 7/5/22.
//

#ifndef HAUSUEBUNG_BIGINT_H

#include <stdio.h>
#include "support.h"

// big integer processor registers
typedef struct {
    ObjRef op1;            // first operand
    ObjRef op2;            // second operand
    ObjRef res;            // result of operation
    ObjRef rem;            // remainder in case of division
} BIP;

extern BIP bip;            // registers of the processor

/* big integer functions */
int bigSgn(void);            //Sign
int bigCmp(void);            //compare
void bigNeg(void);            //negate
void bigAdd(void);
void bigSub(void);
void bigMul(void);
void bigDiv(void);

void bigFromInt(int value);            // from int to Big
int bigToInt(void);            // from Big to int

void bigRead(FILE *in);            // read Big from File
void bigPrint(FILE *out);        // print Big into File

void bigDump(FILE *out, ObjRef objRef);    /* dump a big integer object */

#define HAUSUEBUNG_BIGINT_H

#endif //HAUSUEBUNG_BIGINT_H
