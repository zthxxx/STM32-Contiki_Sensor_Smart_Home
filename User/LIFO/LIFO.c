#include "LIFO.h"



Uint8LIFOQueue* CreatUint8LIFOQueue()
{
    Uint8LIFOQueue*  uint8LIFOQueueHandle;
    uint8LIFOQueueHandle = (Uint8LIFOQueue*)malloc(sizeof(Uint8LIFOQueue));
    if(!uint8LIFOQueueHandle)return NULL;
    uint8LIFOQueueHandle->head=NULL;
    uint8LIFOQueueHandle->last=NULL;
    uint8LIFOQueueHandle->queueSize=0;
    return uint8LIFOQueueHandle;
}



void Uint8LIFOPush(Uint8LIFOQueue* uint8LIFOQueueHandle,uint8_t dataByte)
{
    Uint8LIFO *newNode;
    if(!uint8LIFOQueueHandle)return;
    newNode = (Uint8LIFO*)malloc(sizeof(Uint8LIFO));
    newNode->dataByte = dataByte;
    newNode->next = NULL;
    if(uint8LIFOQueueHandle->head == NULL)
    {
        uint8LIFOQueueHandle->head = newNode;
    }
    else
    {
		newNode->next = uint8LIFOQueueHandle->last;
    }
    uint8LIFOQueueHandle->last = newNode;
    uint8LIFOQueueHandle->queueSize++;
}


uint8_t Uint8LIFOPop(Uint8LIFOQueue* uint8LIFOQueueHandle)
{
    Uint8LIFO *lastNode;
    uint8_t dataByte = 0;
    if(!uint8LIFOQueueHandle)return NULL;
    if(uint8LIFOQueueHandle->last == NULL)
    {
        return NULL;
    }
    lastNode = uint8LIFOQueueHandle->last;
    dataByte = lastNode->dataByte;
    uint8LIFOQueueHandle->last = uint8LIFOQueueHandle->last->next;
    uint8LIFOQueueHandle->queueSize--;
    free(lastNode);
    return dataByte;
}


uint16_t Uint8LIFOGetQueueSize(Uint8LIFOQueue* uint8LIFOQueueHandle)
{
    if(!uint8LIFOQueueHandle)return NULL;
    return uint8LIFOQueueHandle->queueSize;
}

void Uint8LIFOPopToStream(Uint8LIFOQueue* uint8LIFOQueueHandle, uint8_t* uint8BytesStream, uint16_t length)
{
    uint16_t count = 0;
    if(!uint8LIFOQueueHandle)return;
    if(!uint8BytesStream)return;
    if(Uint8LIFOGetQueueSize(uint8LIFOQueueHandle) < length)return;
    
    for(count=0;count<length;count++)
    {
        uint8BytesStream[count] = Uint8LIFOPop(uint8LIFOQueueHandle);
    }   
}

uint8_t Uint8LIFO_Query_StackTop(Uint8LIFOQueue* uint8LIFOQueueHandle)
{
    if(uint8LIFOQueueHandle->last == NULL)return NULL;
    return uint8LIFOQueueHandle->last->dataByte;
}


















