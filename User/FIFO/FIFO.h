#ifndef __FIFO_H
#define __FIFO_H	 
#include "sys.h"
#include <string.h>

typedef struct Uint8FIFONODE
{
    uint8_t data;
    struct Uint8FIFONODE* next;
} Uint8FIFO;

typedef struct Uint8FIFOQUEUE
{
    Uint8FIFO* head;
    Uint8FIFO* last;
}Uint8FIFOQueue;


Uint8FIFOQueue* CreatUint8FIFOQueue(void);
void Uint8FIFOPush(Uint8FIFOQueue* Uint8FIFOQueueHand,uint8_t data);
uint8_t Uint8FIFOPop(Uint8FIFOQueue* Uint8FIFOQueueHand);





#endif
