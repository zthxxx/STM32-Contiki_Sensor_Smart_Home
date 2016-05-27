#ifndef __LIFO_H
#define __LIFO_H	 
#include "sys.h"
#include <string.h>
#include "stdlib.h"

typedef struct Uint8LIFONODE
{
    uint8_t dataByte;
    struct Uint8LIFONODE* next;
} Uint8LIFO;

typedef struct Uint8LIFOQUEUE
{
    Uint8LIFO* head;
    Uint8LIFO* last;
    uint16_t queueSize;
}Uint8LIFOQueue;


Uint8LIFOQueue* CreatUint8LIFOQueue(void);
void Uint8LIFOPush(Uint8LIFOQueue* uint8LIFOQueueHandle,uint8_t dataByte);
uint8_t Uint8LIFOPop(Uint8LIFOQueue* uint8LIFOQueueHandle);
void Uint8LIFOPopToStream(Uint8LIFOQueue* uint8LIFOQueueHandle, uint8_t* uint8BytesStream, uint16_t length);
uint16_t Uint8LIFOGetQueueSize(Uint8LIFOQueue* uint8LIFOQueueHandle);



#endif
