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
#define pushg <n> 11
#define popg  <n> 12
#define asf   <n> 13
#define rsf       14
#define pushl <n> 15
#define popl  <n> 16
#define eq 17
#define ne 18
#define lt 19
#define le 20
#define gt 21
#define ge 22
#define jmp <target> 23
#define brf <target> 24
#define brt <target> 25
#define IMMEDIATE(x) ((x)&0x00FFFFFF)
#define SIGN_EXTEND(i) ((i) & 0x00800000 ? (i) | 0xFF000000 : (i))
#endif //HAUSUEBUNG_OPCODES_H
