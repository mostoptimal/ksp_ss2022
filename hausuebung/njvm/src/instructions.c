//
// Created by mohamad on 5/23/22.
//
#include <stdio.h>
#include "instructions.h"
#include "stack.h"
int a,b,res;
void add(){
    a = pop();
    b = pop();
    a = a + b;
    push(a);
    printf("ADD %d + %d", a, b);
}

void sub(){
    a = pop();
    b = pop();
    a = a - b;
    printf("SUB %d - %d", a, b);
    push(a);
}

void mul(){
    a = pop();
    b = pop();
    a = a * b;
    printf("MUL %d * %d", a, b);
    push(a);
}

void divid(){
    a = pop();
    b = pop();
    a = a / b;
    printf("DIV %d / %d", a, b);
    push(a);
}

void mod(){
    a = pop();
    b = pop();
    a = a % b;
    printf("MOD %d %d", a, b);
    push(a);
}

void equal() {
    a = pop();
    b = pop();
    if (a == b) {//or v1-v2==0
        res= 1;//1 for True
    } else res= 0;//0 for False
    push(res);
}

void nequal() {
    a = pop();
    b = pop();
    if (a!=b) res= 1;
    else res= 0;
    push(res);
}

void lessThan() {
    a = pop();
    b = pop();
    if (b - a > 0) res= 1;
    else res= 0;
    push(res);
}

void lessEqual() {
    a = pop();
    b = pop();
    if (b >= a) res= 1;
    else res= 0;
    push(res);
}

void greaterThan() {
    a = pop();
    b = pop();
    if (a - b > 0) res= 1;
    else res= 0;
    push(res);
}

void greaterEqual() {
    a = pop();
    b = pop();
    if (a - b >= 0) res= 1;
    else res= 0;
    push(res);
}

int jump(int label) {
// zu implementieren
    return label;
}

void readInt(){
    int input=0;
    scanf("%d",&input);
    push(input);

}
void readChar(){
    char inputC;
    scanf("%c",&inputC);
    push(inputC);
}
int writeInt(){
    int output=pop();
    return output;
}
char writeChar(){
    char output=pop();
    return output;
}


/**
int branchFalse(int condition, int target) {
    int ziel=target;
    if (condition == 0)
        return ziel;

}

int branchTrue(int condition, int target) {
    int ziel=target;
    if (condition == 1)
        return ziel;
}*/

int call(int immediate, int pc) {
    int alt_pc = pc;
    pc = immediate;
    //execute ,....
    // kommst du zuruck

    return alt_pc;
}