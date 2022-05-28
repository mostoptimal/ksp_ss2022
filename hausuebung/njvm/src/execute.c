//
// Created by mohamad on 5/27/22.
//
#include <stdio.h>
#include <stdlib.h>
#include "opCodes.h"
#include "execute.h"

void printInstruct(unsigned int opCode, int immediatevalue) {
    unsigned int oc = opCode >> 24;
    switch (oc) {
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
            fprintf(stderr, "Error: Unknown opcode %d", oc);
            exit(EXIT_FAILURE);
    }
}