//
// Created by mohamad on 5/6/22.
//

#ifndef HAUSUEBUNG_OPCODES_H
#define HAUSUEBUNG_OPCODES_H
#define HALT 0
#define PUSHC 1
#define ADD 2
#define SUB 3
#define MUL 4
#define DIV 5
#define MOD 6
#define RDINT 7
#define WRINT 8
#define RDCHR 9
#define WRCHR 10
#define PUSHG <n> 11
#define POPG <n> 12
#define ASF <n> 13
#define RSF 14
#define PUSHL <n> 15
#define POPL <n> 16
#define EQ 17
#define NE 18
#define LT 19
#define LE 20
#define GT 21
#define GE 22
#define JMP <target> 23
#define BRF <target> 24
#define BRT <target> 25
#define CALL <target> 26
#define RET 27
#define DROP  <n> 28
#define PUSHR 29
#define POPR 30
#define DUP 31

#define IMMEDIATE(x) ((x)&0x00FFFFFF)
#define SIGN_EXTEND(i) ((i) & 0x00800000 ? (i) | 0xFF000000 : (i))
#endif //HAUSUEBUNG_OPCODES_H
