//
// Created by mohamad on 5/6/22.
//
#include "opCodes.h"
#include "programs.h"

unsigned int program_1_memory[] = { //Program_1
        (PUSHC << 24) | IMMEDIATE(3),
        (PUSHC << 24) | IMMEDIATE(4),
        (ADD << 24),
        (PUSHC << 24) | IMMEDIATE(10),
        (PUSHC << 24) | IMMEDIATE(6),
        (SUB << 24),
        (MUL << 24),
        (WRINT << 24),
        (PUSHC << 24) | IMMEDIATE(10),
        (WRCHR << 24),
        (HALT << 24)
};
unsigned int program_2_memory[] = { //Program_2
        (PUSHC << 24) | SIGN_EXTEND(-2),
        (RDINT << 24),
        (MUL << 24),
        (PUSHC << 24) | IMMEDIATE(3),
        (ADD << 24),
        (WRINT << 24),
        (PUSHC << 24) | IMMEDIATE('\n'),
        (WRCHR << 24),
        (HALT << 24)
};

unsigned int program_3_memory[] = {
        (RDCHR << 24),
        (WRINT << 24),
        (PUSHC << 24) | IMMEDIATE('\n'),
        (WRCHR << 24),
        (HALT << 24)
};
unsigned int program_dumbIdea[] = {
        (MOD << 24),
        (DIV << 24),
        (RSF << 24),
        (EQ << 24),
        (NE << 24),
        (NE << 24),
        (LT << 24),
        (LE << 24),
        (GT << 24),
        (GE << 24)
};

unsigned int program_simpleIF[]={
        (PUSHC<<24),
        (PUSHC<<24),
        (EQ<<24)
};

unsigned int program_memory[] = { //Example
        0x01000002, //program_memory[0]
        0x01000003, //program_memory[1]
        0x04000000, //program_memory[2]
        0x01000005, //program_memory[3]
        0x02000000, //program_memory[4]
        0x08000000, //program_memory[5]
        0x00000000  //program_memory[6]
};