
typedef struct {
    int nd;
    unsigned char sign;
    unsigned char digits[1];
} Big;

BIP bip = {
        NULL,
        NULL,
        NULL,
        NULL,
};
/*********************************/
#define BIG_PTR(objRef)            ((Big *) ((objRef)->data))
#define GET_ND(objRef)            (BIG_PTR(objRef)->nd)
#define SET_ND(objRef, val)        (BIG_PTR(objRef)->nd = (val))
#define GET_SIGN(objRef)        (BIG_PTR(objRef)->sign)
#define SET_SIGN(objRef, val)        (BIG_PTR(objRef)->sign = (val))
#define GET_DIGIT(objRef, i)        (BIG_PTR(objRef)->digits[i])
#define SET_DIGIT(objRef, i, val)    (BIG_PTR(objRef)->digits[i] = (val))

/*********************************/
static ObjRef newBig(int nd) {
    int dataSize;
    ObjRef objRef;
    dataSize = sizeof(int) + 1 + nd;
    objRef = newPrimObject(dataSize);
    return objRef;
}

static void bigSwap(void) {
    ObjRef tmp;
    tmp = bip.op1;
    bip.op1 = bip.op2;
    bip.op2 = tmp;
}

static int bigUnsignedCompare(void) {
    int nd1;
    int nd2;
    int diff;
    // comparing sizes of BigInt
    nd1 = GET_ND(bip.op1);
    nd2 = GET_ND(bip.op2);
    if (nd1 != nd2) {
        // Not Equal Sizes , return the Difference (for bigger one)
        return nd1 - nd2;
    }
    // Equal Sizes ,// we must look at the digits
    while (nd1--) {
        diff = (int) GET_DIGIT(bip.op1, nd1) - (int) GET_DIGIT(bip.op2, nd1);
        if (diff != 0) {
            return diff;
        }
    }
    // same Sizes
    return 0;
}

static void bigUnsignedAddition(void) {
    int nd1;
    int nd2;
    int i;
    unsigned short carry;
    unsigned short aux;
    int xchgFlag;

    // make sure op1 has at least as many digits as op2
    nd1 = GET_ND(bip.op1);
    nd2 = GET_ND(bip.op2);
    if (nd1 < nd2) {
        // exchange operands
        bigSwap();
        i = nd1;
        nd1 = nd2;
        nd2 = i;
        xchgFlag = 1;
    } else {
        // don't exchange operands
        xchgFlag = 0;
    }
    // allocate result
    bip.res = newBig(nd1 + 1);
    // copy op2 to result
    for (i = 0; i < nd2; i++) {
        SET_DIGIT(bip.res, i, GET_DIGIT(bip.op2, i));
    }
    // fill result with 0 up to size of op1
    for (; i < nd1; i++) {
        SET_DIGIT(bip.res, i, 0);
    }
    // res = op1 + res
    carry = 0x00;
    for (i = 0; i < nd1; i++) {
        aux = (unsigned short) GET_DIGIT(bip.op1, i) +
              (unsigned short) GET_DIGIT(bip.res, i) +
              carry;
        SET_DIGIT(bip.res, i, aux & 0xFF);
        carry = aux >> 8;
    }
    SET_DIGIT(bip.res, i, carry);
    // determine actual size of result
    i = nd1 + 1;
    while (--i >= 0 && GET_DIGIT(bip.res, i) == 0);
    SET_ND(bip.res, i + 1);
    // restore operands
    if (xchgFlag) {
        bigSwap();
    }
}

static void bigUnsignedSubtraction(void) {
    int nd1;
    int nd2;
    int i;
    unsigned short carry;
    unsigned short aux;

    // op1 must have at least as many digits as op2
    nd1 = GET_ND(bip.op1);
    nd2 = GET_ND(bip.op2);
    if (nd1 < nd2) {
        // unsigned subtraction would yield negative result
        fatalError("internal library error #1 - THIS SHOULD NEVER HAPPEN!");
    }
    // allocate result
    bip.res = newBig(nd1);
    // copy op2 to result
    for (i = 0; i < nd2; i++) {
        SET_DIGIT(bip.res, i, GET_DIGIT(bip.op2, i));
    }
    // fill result with 0 up to size of op1
    for (; i < nd1; i++) {
        SET_DIGIT(bip.res, i, 0);
    }
    // res = op1 - res
    carry = 0x01;
    for (i = 0; i < nd1; i++) {
        aux = (unsigned short) GET_DIGIT(bip.op1, i) - (unsigned short) GET_DIGIT(bip.res, i) + carry + 0xFF;
        SET_DIGIT(bip.res, i, aux & 0xFF);
        carry = aux >> 8;
    }
    if (carry != 0x01) {
        // unsigned subtraction would yield negative result
        fatalError("internal library error #2 - THIS SHOULD NEVER HAPPEN!");
    }
    // determine actual size of result
    i = nd1;
    while (--i >= 0 && GET_DIGIT(bip.res, i) == 0);
    SET_ND(bip.res, i + 1);
}

static void bigUnsignedMultiplication(void) {
    int nd1;
    int nd2;
    int i, j, k;
    unsigned short carry;
    unsigned short aux;

    /* get sizes of operands */
    nd1 = GET_ND(bip.op1);
    nd2 = GET_ND(bip.op2);
    /* allocate result */
    bip.res = newBig(nd1 + nd2);
    /* reset lower nd1 digits of result */
    for (i = 0; i < nd1; i++) {
        SET_DIGIT(bip.res, i, 0);
    }
    /* res = op1 * op2 */
    for (j = 0; j < nd2; j++) {
        carry = 0x00;
        for (k = j, i = 0; i < nd1; k++, i++) {
            aux = (unsigned short) GET_DIGIT(bip.op1, i) *
                  (unsigned short) GET_DIGIT(bip.op2, j) +
                  (unsigned short) GET_DIGIT(bip.res, k) +
                  carry;
            SET_DIGIT(bip.res, k, aux & 0xFF);
            carry = aux >> 8;
        }
        SET_DIGIT(bip.res, k, carry);
    }
    /* determine actual size of result */
    i = nd1 + nd2;
    while (--i >= 0 && GET_DIGIT(bip.res, i) == 0);
    SET_ND(bip.res, i + 1);
}

static unsigned char bigUdiv1(unsigned char divisor) {
    ObjRef tmp;
    int nd;
    int i;
    unsigned short d, r;
    unsigned short aux;

    /* get size of dividend */
    nd = GET_ND(bip.rem);
    /* check for division by zero */
    d = (unsigned short) divisor;
    if (d == 0) {
        fatalError("internal library error #3 - THIS SHOULD NEVER HAPPEN!");
    }
    /* allocate result */
    tmp = newBig(nd);
    /* tmp = dividend / divisor, r = dividend % divisor */
    r = 0;
    for (i = nd - 1; i >= 0; i--) {
        aux = (r << 8) | (unsigned short) GET_DIGIT(bip.rem, i);
        SET_DIGIT(tmp, i, aux / d);
        r = aux % d;
    }
    /* determine actual size of quotient */
    i = nd;
    while (--i >= 0 && GET_DIGIT(tmp, i) == 0);
    SET_ND(tmp, i + 1);
    /* store quotient */
    bip.rem = tmp;
    /* return remainder */
    return (unsigned char) r;
}

static void bigUdiv(void) {
    ObjRef tmp;
    int nd1;
    int nd2;
    int nd3;
    int i, j, k, l;
    unsigned char r;
    unsigned short scale;
    unsigned short carry;
    unsigned short aux;
    unsigned short qhat;
    unsigned short v1, v2;
    unsigned short uj0, uj1, uj2, two;

    /* get sizes of operands */
    nd1 = GET_ND(bip.op1);
    nd2 = GET_ND(bip.op2);
    /* check for division by zero */
    if (nd2 == 0) {
        fatalError("division by zero");
    }
    /* check for small dividend */
    if (bigUnsignedCompare() < 0) {
        /* res = 0 */
        bip.res = newBig(0);
        SET_ND(bip.res, 0);
        /* rem = op1; BUT THIS HAS TO BE A COPY! */
        bip.rem = newBig(nd1);
        for (i = 0; i < nd1; i++) {
            SET_DIGIT(bip.rem, i, GET_DIGIT(bip.op1, i));
        }
        SET_ND(bip.rem, nd1);
        return;
    }
    /* check for single digit divisor */
    if (nd2 == 1) {
        /* yes - use simple division by single digit divisor */
        bip.rem = bip.op1;
        r = bigUdiv1(GET_DIGIT(bip.op2, 0));
        bip.res = bip.rem;
        if (r == 0) {
            bip.rem = newBig(0);
            SET_ND(bip.rem, 0);
        } else {
            bip.rem = newBig(1);
            SET_ND(bip.rem, 1);
            SET_DIGIT(bip.rem, 0, r);
        }
        return;
    }
    /*
     * now for the general case
     */
#if DIV_CHK_01
    printf("div_chk #01: division, general case\n");
  printf("             dividend = ");
  bigDump(stdout, bip.op1);
  printf("\n");
  printf("             divisor  = ");
  bigDump(stdout, bip.op2);
  printf("\n");
#endif
    /* determine scale factor for normalization */
    scale = (unsigned short) 256 /
            ((unsigned short) GET_DIGIT(bip.op2, nd2 - 1) + 1);
#if DIV_CHK_02
    printf("div_chk #02: scale factor = %02X\n", scale);
#endif
    /* normalize dividend, result is in bip.rem */
    bip.rem = newBig(nd1 + 1);
    carry = 0x00;
    for (i = 0; i < nd1; i++) {
        aux = (unsigned short) GET_DIGIT(bip.op1, i) * scale +
              carry;
        SET_DIGIT(bip.rem, i, aux & 0xFF);
        carry = aux >> 8;
    }
    SET_DIGIT(bip.rem, i, carry);
    SET_ND(bip.rem, nd1 + 1);
#if DIV_CHK_03
    printf("div_chk #03: normalized dividend = ");
  bigDump(stdout, bip.rem);
  printf("\n");
#endif
    /* normalize divisor, result is in bip.res */
    bip.res = newBig(nd2);
    carry = 0x00;
    for (i = 0; i < nd2; i++) {
        aux = (unsigned short) GET_DIGIT(bip.op2, i) * scale +
              carry;
        SET_DIGIT(bip.res, i, aux & 0xFF);
        carry = aux >> 8;
    }
    if (carry != 0x00) {
        /* overflow in divisor normalization */
        fatalError("internal library error #4 - THIS SHOULD NEVER HAPPEN!");
    }
    SET_ND(bip.res, nd2);
#if DIV_CHK_04
    printf("div_chk #04: normalized divisor  = ");
  bigDump(stdout, bip.res);
  printf("\n");
#endif
    /* allocate quotient */
    nd3 = nd1 - nd2 + 1;
    tmp = newBig(nd3);
    /* extract the two most significand digits of divisor */
    v1 = (unsigned short) GET_DIGIT(bip.res, nd2 - 1);
    v2 = (unsigned short) GET_DIGIT(bip.res, nd2 - 2);
    /* loop on digits of dividend and compute digits of quotient */
    /* j is index into dividend, k is index into quotient */
    for (j = nd1, k = nd3 - 1; k >= 0; j--, k--) {
#if DIV_CHK_05
        printf("div_chk #05: j = %d, k = %d\n", j, k);
#endif
        /* calculate qhat */
        uj0 = (unsigned short) GET_DIGIT(bip.rem, j);
        uj1 = (unsigned short) GET_DIGIT(bip.rem, j - 1);
        uj2 = (unsigned short) GET_DIGIT(bip.rem, j - 2);
        two = (uj0 << 8) | uj1;
        if (uj0 == v1) {
            qhat = (unsigned short) 255;
#if DIV_CHK_06
            printf("div_chk #06a: qhat = %02X\n", qhat);
#endif
        } else {
            qhat = two / v1;
#if DIV_CHK_06
            printf("div_chk #06b: qhat = %02X\n", qhat);
#endif
        }
        while (qhat * v2 > (((two - qhat * v1) << 8) | uj2)) {
            qhat--;
#if DIV_CHK_07
            printf("div_chk #07: qhat decremented, is now %02X\n", qhat);
#endif
        }
        /* multiply and subtract */
        /* l is index into dividend, i is index into divisor */
        carry = 0xFF;
        for (l = j - nd2, i = 0; i < nd2; l++, i++) {
            aux = (unsigned short) GET_DIGIT(bip.rem, l) -
                  (unsigned short) GET_DIGIT(bip.res, i) * qhat +
                  carry + 0xFE01;
            SET_DIGIT(bip.rem, l, aux & 0xFF);
            carry = aux >> 8;
        }
        aux = (unsigned short) GET_DIGIT(bip.rem, l) +
              carry + 0xFE01;
        SET_DIGIT(bip.rem, l, aux & 0xFF);
        carry = aux >> 8;
#if DIV_CHK_08
        printf("div_chk #08: remainder = ");
    bigDump(stdout, bip.rem);
    printf("\n");
#endif
        /* test remainder and possibly add back */
        if (carry != 0xFF) {
            /* qhat is one too large */
            qhat--;
#if DIV_CHK_09
            printf("div_chk #09: qhat final correction, is now %02X\n", qhat);
#endif
            /* add back */
            /* l is index into dividend, i is index into divisor */
            carry = 0x00;
            for (l = j - nd2, i = 0; i < nd2; l++, i++) {
                aux = (unsigned short) GET_DIGIT(bip.rem, l) +
                      (unsigned short) GET_DIGIT(bip.res, i) +
                      carry;
                SET_DIGIT(bip.rem, l, aux & 0xFF);
                carry = aux >> 8;
            }
            aux = (unsigned short) GET_DIGIT(bip.rem, l) +
                  carry;
            SET_DIGIT(bip.rem, l, aux & 0xFF);
            carry = aux >> 8;
            if (carry != 0x01) {
                /* missing carry in add-back sum */
                fatalError("internal library error #5 - THIS SHOULD NEVER HAPPEN!");
            }
#if DIV_CHK_10
            printf("div_chk #10: remainder = ");
      bigDump(stdout, bip.rem);
      printf("\n");
#endif
        }
        /* store quotient digit */
        SET_DIGIT(tmp, k, qhat);
#if DIV_CHK_11
        printf("div_chk #11: quotient digit = %02X\n", qhat);
#endif
    }
    /* finish quotient */
    i = nd3;
    while (--i >= 0 && GET_DIGIT(tmp, i) == 0);
    SET_ND(tmp, i + 1);
    bip.res = tmp;
    /* finish and unnormalize remainder */
    i = nd1 + 1;
    while (--i >= 0 && GET_DIGIT(bip.rem, i) == 0);
    SET_ND(bip.rem, i + 1);
    r = bigUdiv1(scale);
    if (r != 0) {
        /* non-zero remainder in unnormalization */
        fatalError("internal library error #6 - THIS SHOULD NEVER HAPPEN!");
    }
}

static void nilRefException(void) {
    fatalError("big integer library detected illegal nil reference");
}

int bigSgn(void) {
    if (bip.op1 == NULL) {
        nilRefException();
    }
    if (GET_ND(bip.op1) == 0) {
        return 0;
    }
    if (GET_SIGN(bip.op1) == BIG_POSITIVE) {
        return 1;
    } else {
        return -1;
    }
}

int bigCompare(void) {
    if (bip.op1 == NULL || bip.op2 == NULL) {
        nilRefException();
    }
    if (GET_SIGN(bip.op1) == BIG_POSITIVE) {
        if (GET_SIGN(bip.op2) == BIG_POSITIVE) {
            // op1 >= 0 and op2 >= 0
            return bigUnsignedCompare();
        } else {
            // op1 >= 0 and op2 < 0
            return 1;
        }
    } else {
        if (GET_SIGN(bip.op2) == BIG_POSITIVE) {
            // op1 < 0 and op2 >= 0
            return -1;
        } else {
            // op1 < 0 and op2 < 0
            return -bigUnsignedCompare();
        }
    }
}

void bigNeg(void) {
    int nd;
    int i;

    if (bip.op1 == NULL) {
        nilRefException();
    }
    // make copy of operand
    nd = GET_ND(bip.op1);
    bip.res = newBig(nd);
    for (i = 0; i < nd; i++) {
        SET_DIGIT(bip.res, i, GET_DIGIT(bip.op1, i));
    }
    SET_ND(bip.res, nd);
    // store inverted sign
    if (GET_SIGN(bip.op1) == BIG_NEGATIVE || nd == 0) {
        SET_SIGN(bip.res, BIG_POSITIVE);
    } else {
        SET_SIGN(bip.res, BIG_NEGATIVE);
    }
}

void bigAddition(void) {
    if (bip.op1 == NULL || bip.op2 == NULL) {
        nilRefException();
    }
    if (GET_SIGN(bip.op1) == BIG_POSITIVE) {
        if (GET_SIGN(bip.op2) == BIG_POSITIVE) {
            /* op1 >= 0 and op2 >= 0 */
            bigUnsignedAddition();
            SET_SIGN(bip.res, BIG_POSITIVE);
        } else {
            /* op1 >= 0 and op2 < 0 */
            if (bigUnsignedCompare() >= 0) {
                /* |op1| >= |op2| */
                bigUnsignedSubtraction();
                SET_SIGN(bip.res, BIG_POSITIVE);
            } else {
                /* |op1| < |op2| */
                bigSwap();
                bigUnsignedSubtraction();
                SET_SIGN(bip.res, BIG_NEGATIVE);
                bigSwap();
            }
        }
    } else {
        if (GET_SIGN(bip.op2) == BIG_POSITIVE) {
            /* op1 < 0 and op2 >= 0 */
            if (bigUnsignedCompare() <= 0) {
                /* |op1| <= |op2| */
                bigSwap();
                bigUnsignedSubtraction();
                SET_SIGN(bip.res, BIG_POSITIVE);
                bigSwap();
            } else {
                /* |op1| > |op2| */
                bigUnsignedSubtraction();
                SET_SIGN(bip.res, BIG_NEGATIVE);
            }
        } else {
            /* op1 < 0 and op2 < 0 */
            bigUnsignedAddition();
            SET_SIGN(bip.res, BIG_NEGATIVE);
        }
    }
}

void bigSubtraction(void) {
    if (bip.op1 == NULL || bip.op2 == NULL) {
        nilRefException();
    }
    if (GET_SIGN(bip.op1) == BIG_POSITIVE) {
        if (GET_SIGN(bip.op2) == BIG_POSITIVE) {
            /* op1 >= 0 and op2 >= 0 */
            if (bigUnsignedCompare() >= 0) {
                /* |op1| >= |op2| */
                bigUnsignedSubtraction();
                SET_SIGN(bip.res, BIG_POSITIVE);
            } else {
                /* |op1| < |op2| */
                bigSwap();
                bigUnsignedSubtraction();
                SET_SIGN(bip.res, BIG_NEGATIVE);
                bigSwap();
            }
        } else {
            /* op1 >= 0 and op2 < 0 */
            bigUnsignedAddition();
            SET_SIGN(bip.res, BIG_POSITIVE);
        }
    } else {
        if (GET_SIGN(bip.op2) == BIG_POSITIVE) {
            /* op1 < 0 and op2 >= 0 */
            bigUnsignedAddition();
            SET_SIGN(bip.res, BIG_NEGATIVE);
        } else {
            /* op1 < 0 and op2 < 0 */
            if (bigUnsignedCompare() <= 0) {
                /* |op1| <= |op2| */
                bigSwap();
                bigUnsignedSubtraction();
                SET_SIGN(bip.res, BIG_POSITIVE);
                bigSwap();
            } else {
                /* |op1| > |op2| */
                bigUnsignedSubtraction();
                SET_SIGN(bip.res, BIG_NEGATIVE);
            }
        }
    }
}

void bigMultiplication(void) {
    if (bip.op1 == NULL || bip.op2 == NULL) {
        nilRefException();
    }
    bigUnsignedMultiplication();
    if (GET_SIGN(bip.op1) == GET_SIGN(bip.op2) || GET_ND(bip.res) == 0) {
        SET_SIGN(bip.res, BIG_POSITIVE);
    } else {
        SET_SIGN(bip.res, BIG_NEGATIVE);
    }
}

void bigDivision(void) {
    if (bip.op1 == NULL || bip.op2 == NULL) {
        nilRefException();
    }
    bigUdiv();
    if (GET_SIGN(bip.op1) == GET_SIGN(bip.op2) || GET_ND(bip.res) == 0) {
        SET_SIGN(bip.res, BIG_POSITIVE);
    } else {
        SET_SIGN(bip.res, BIG_NEGATIVE);
    }
    if (GET_SIGN(bip.op1) == BIG_POSITIVE || GET_ND(bip.rem) == 0) {
        SET_SIGN(bip.rem, BIG_POSITIVE);
    } else {
        SET_SIGN(bip.rem, BIG_NEGATIVE);
    }
}

void bigFromInt(int value) {
    int i;
    bip.res = newBig(sizeof(int));
    if (value < 0) {
        value = -value;
        SET_SIGN(bip.res, BIG_NEGATIVE);
    } else {
        SET_SIGN(bip.res, BIG_POSITIVE);
    }
    for (i = 0; i < sizeof(int); i++) {
        SET_DIGIT(bip.res, i, value & 0xFF);
        value >>= 8;
    }
    while (--i >= 0 && GET_DIGIT(bip.res, i) == 0);
    SET_ND(bip.res, i + 1);
}

int bigToInt(void) {
    int nd;
    int i;
    int res;

    if (bip.op1 == NULL) {
        nilRefException();
    }
    nd = GET_ND(bip.op1);
    if (nd > 4 || (nd == 4 && GET_DIGIT(bip.op1, 3) >= 0x80)) {
        fatalError("big integer too big for conversion to int");
    }
    res = 0;
    for (i = nd - 1; i >= 0; i--) {
        res <<= 8;
        res |= (unsigned int) GET_DIGIT(bip.op1, i);
    }
    if (GET_SIGN(bip.op1) == BIG_NEGATIVE) {
        res = -res;
    }
    return res;
}

void bigDump(FILE *out, ObjRef objRef) {
    int nd;
    unsigned char sign;
    int i;

    if (objRef == NULL) {
        nilRefException();
    }
    nd = GET_ND(objRef);
    sign = GET_SIGN(objRef);
    fprintf(out, "[%d %c", nd, sign == BIG_POSITIVE ? '+' : '-');
    for (i = 0; i < nd; i++) {
        fprintf(out, " %02X", GET_DIGIT(objRef, i));
    }
    fprintf(out, "]");
}

void bigPrint(FILE *out) {
    int nd;
    unsigned char r;
    int skipZero;

    if (bip.op1 == NULL) {
        nilRefException();
    }
    nd = GET_ND(bip.op1);
    if (nd == 0) {
        fprintf(out, "0");
        return;
    }
    if (GET_SIGN(bip.op1) == BIG_NEGATIVE) {
        fprintf(out, "-");
    }
    /* number of digits in base 10 = number of digits
       in base 256 * log10(256), and log10(256) < 2.5  */
    nd = 2 * nd + nd / 2;
    bip.rem = bip.op1;
    bigFromInt(10);
    bip.op2 = bip.res;
    bigFromInt(1);
    while (nd != 0) {
        bip.op1 = bip.res;
        bigUnsignedMultiplication();
        nd--;
    }
    bip.op1 = bip.rem;
    bip.op2 = bip.res;
    skipZero = 1;
    do {
        bigUdiv();
        if (GET_ND(bip.res) == 0) {
            if (!skipZero) {
                fprintf(out, "0");
            }
        } else {
            if (GET_ND(bip.res) != 1) {
                fatalError("internal library error #7 - THIS SHOULD NEVER HAPPEN!");
            }
            fprintf(out, "%c", GET_DIGIT(bip.res, 0) + '0');
            skipZero = 0;
        }
        bip.op1 = bip.rem;
        bip.rem = bip.op2;
        r = bigUdiv1(10);
        bip.op2 = bip.rem;
    } while (r == 0);
}

void bigRead(FILE *in) {
    int c;
    int positive;

    c = fgetc(in);
    while (isspace(c)) {
        c = fgetc(in);
    }
    if (c == '-') {
        positive = 0;
        c = fgetc(in);
    } else {
        positive = 1;
        if (c == '+') {
            c = fgetc(in);
        }
    }
    if (!isdigit(c)) {
        fatalError("no digits in input");
    }
    bigFromInt(10);
    bip.rem = bip.res;
    bigFromInt(0);
    while (isdigit(c)) {
        bip.op1 = bip.res;
        bip.op2 = bip.rem;
        bigUnsignedMultiplication();
        bip.op1 = bip.res;
        bigFromInt(c - '0');
        bip.op2 = bip.res;
        bigUnsignedAddition();
        c = fgetc(in);
    }
    ungetc(c, in);
    if (positive || GET_ND(bip.res) == 0) {
        SET_SIGN(bip.res, BIG_POSITIVE);
    } else {
        SET_SIGN(bip.res, BIG_NEGATIVE);
    }
}