//
// Created by Daniel Spengler on 28.11.2020.
//

#ifndef KSP_WS20_21_JB_DS_DEFINE_H
#define KSP_WS20_21_JB_DS_DEFINE_H

// General defines
#define VERSION 8


//methods for decoding and encoding of instructions
#define IMMEDIATE(x) ((x) & 0x00FFFFFF)
#define SIGN_EXTEND(i) ((i) & 0x00800000 ? (i) | 0xFF000000 : (i))

#define OPCODE(instr) ((instr)>>24)
//methods for managing MSB on compound objects
#define MSB (1 << (8 * sizeof(unsigned int) - 1))
#define IS_PRIMITIVE(objRef) (((objRef)->size & MSB) == 0)
#define GET_ELEMENT_COUNT(objRef) ((objRef)->size & ~MSB)

// Defines for Debug
#define DEBUGHELP " [list]: Lists all program instructions\n [step]: Executes next instruction\n [run]: Runs through the rest of the programm or until the next breakpoint\n [ps]: Prints Stack with stack pointer and frame pointer\n [pd]: Prints all variables in static data area\n [prvr]: Prints the value stored in return value register\n [insp]: Inspect a slot on the stack and display its information\n [bs] Sets a new breakpoint\n [br] Removes the current breakpoint\n [bl] Lists the current breakpoint\n [quit]: Quits NJVM\n [help]: Shows this help\n"

// OP-Codes
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
#define PUSHG 11
#define POPG 12
#define ASF 13
#define RSF 14
#define PUSHL 15
#define POPL 16
#define EQ 17
#define NE 18
#define LT 19
#define LE 20
#define GT 21
#define GE 22
#define JMP 23
#define BRF 24
#define BRT 25
#define CALL 26
#define RET 27
#define DROP 28
#define PUSHR 29
#define POPR 30
#define DUP 31
#define NEW 32
#define GETF 33
#define PUTF 34
#define NEWA 35
#define GETFA 36
#define PUTFA 37
#define GETSZ 38
#define PUSHN 39
#define REFEQ 40
#define REFNE 41
#endif //KSP_WS20_21_JB_DS_DEFINE_H
