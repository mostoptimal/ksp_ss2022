//
// Created by mohamad on 5/23/22.
//

#ifndef HAUSUEBUNG_INSTRUCTIONS_H
#define HAUSUEBUNG_INSTRUCTIONS_H

void add();

void sub();

void mul();

void divid();

void mod();

void equal();

int jump(int label);

void nequal();

void lessThan();

void lessEqual();

void greaterThan();

void greaterEqual();

void readInt();

int jump(int label);



int branchFalse(int condition, int target);

int branchTrue(int condition, int target);

int call(int immediate, int pc);

#endif //HAUSUEBUNG_INSTRUCTIONS_H
