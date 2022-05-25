//
// Created by mohamad on 5/23/22.
//

#ifndef HAUSUEBUNG_INSTRUCTIONS_H
#define HAUSUEBUNG_INSTRUCTIONS_H

int equal(int v1, int v2);

int jump(int label);

int nequal(int v1, int v2);

int lessThan(int v1, int v2);

int lessEqual(int v1, int v2);

int greaterThan(int v1, int v2);

int greaterEqual(int v1, int v2);

int jump(int label);

int branchFalse(int condition, int target);

int branchTrue(int condition, int target);

int call(int immediate, int pc);

#endif //HAUSUEBUNG_INSTRUCTIONS_H
