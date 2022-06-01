
#include <stdio.h>

int stack[100000];
int sp = 0;


void push(int element)
{
    stack[sp] = element;
    sp++;
}

int pop()
{
    int ele;
    //printf("pop %d\n",stack[sp-1]);

    ele = stack[--sp];

    return ele;
}
