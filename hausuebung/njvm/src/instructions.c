//
// Created by mohamad on 5/23/22.
//
#include <stdio.h>
#include "instructions.h"

int equal(int v1, int v2) {
    if (v1 == v2) {//or v1-v2==0
        return 1;//1 for True
    } else return 0;//0 for False
}

int nequal(int v1, int v2) {
    if (equal(v1, v2) == 0) return 1;
    else return 0;
}

int lessThan(int v1, int v2) {
    if (v2 - v1 > 0) return 1;
    else return 0;
}

int lessEqual(int v1, int v2) {
    if (v2 >= v1) return 1;
    else return 0;
}

int greaterThan(int v1, int v2) {
    if (v1 - v2 > 0) return 1;
    else return 0;
}

int greaterEqual(int v1, int v2) {
    if (v1 - v2 >= 0) return 1;
    else return 0;
}

int jump(int label ) {
// zu implementieren
    return label;
}

int branchFalse(int condition, int target) {
    if (condition == 0) return target;
}

int branchTrue(int condition, int target) {
    if (condition == 1) return target;
}

 int call (int inmediate, int pc){
    int alt_pc = pc ;
    pc = inmediate;
    //execute ,....
    // kommst du zuruck

    return alt_pc;
}