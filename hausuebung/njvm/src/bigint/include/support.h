//
// Created by mohamad on 7/5/22.
//

#ifndef HAUSUEBUNG_SUPPORT_H
#define HAUSUEBUNG_SUPPORT_H

/* object representation */
typedef struct {
    unsigned int size;            /* byte count of payload data */
    void *forwardPointer;
    unsigned char data[1];        /* payload data, size as needed */
} *ObjRef;

/* support functions */
void fatalError(char *msg);        /* print a message and exit */
ObjRef newPrimObject(int dataSize);    /* create a new primitive object */

#endif //HAUSUEBUNG_SUPPORT_H
