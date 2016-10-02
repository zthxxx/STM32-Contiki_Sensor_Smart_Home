#ifndef __FIFO_H
#define __FIFO_H	 
#include "sys.h"
#include <string.h>
#include "stdio.h"
#include "stdlib.h"


typedef struct Uint8FIFONODE
{
    uint8_t dataByte;
    struct Uint8FIFONODE* next;
} Uint8FIFO;

typedef struct Uint8FIFOQUEUE
{
    Uint8FIFO* head;
    Uint8FIFO* last;
    uint16_t queueSize;
}Uint8FIFOQueue;


Uint8FIFOQueue* CreatUint8FIFOQueue(void);
void Uint8FIFOPush(Uint8FIFOQueue* uint8FIFOQueueHandle,uint8_t dataByte);
uint8_t Uint8FIFOPop(Uint8FIFOQueue* uint8FIFOQueueHandle);
void Uint8FIFOPopToStream(Uint8FIFOQueue* uint8FIFOQueueHandle, uint8_t* uint8BytesStream, uint16_t length);
uint16_t Uint8FIFOGetQueueSize(Uint8FIFOQueue* uint8FIFOQueueHandle);



#endif
