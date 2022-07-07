//
// Created by mohamad on 7/5/22.
//

#ifndef HAUSUEBUNG_BIGINT_H

#include <stdio.h>
#include "support.h"

/* big integer processor registers */
typedef struct {
    ObjRef op1;            // first operand
    ObjRef op2;            // second operand
    ObjRef res;            // result of operation
    ObjRef rem;            // remainder in case of division
} BIP;

extern BIP bip;            // registers of the processor

/* big integer functions */
int bigSgn(void);            /* sign */
int bigCmp(void);            /* comparison */
void bigNeg(void);            /* negation */
void bigAdd(void);            /* addition */
void bigSub(void);            /* subtraction */
void bigMul(void);            /* multiplication */
void bigDiv(void);            /* division */

void bigFromInt(int n);            /* conversion int --> big */
int bigToInt(void);            /* conversion big --> int */

void bigRead(FILE *in);            /* read a big integer */
void bigPrint(FILE *out);        /* print a big integer */

void bigDump(FILE *out, ObjRef objRef);    /* dump a big integer object */

#define HAUSUEBUNG_BIGINT_H

#endif //HAUSUEBUNG_BIGINT_H
