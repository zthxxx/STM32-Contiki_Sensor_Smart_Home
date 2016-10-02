#include "FIFO.h"



Uint8FIFOQueue* CreatUint8FIFOQueue()
{
    Uint8FIFOQueue*  uint8FIFOQueueHandle;
    uint8FIFOQueueHandle = (Uint8FIFOQueue*)malloc(sizeof(Uint8FIFOQueue));
    if(!uint8FIFOQueueHandle)return NULL;
    uint8FIFOQueueHandle->head=NULL;
    uint8FIFOQueueHandle->last=NULL;
    uint8FIFOQueueHandle->queueSize=0;
    return uint8FIFOQueueHandle;
}



void Uint8FIFOPush(Uint8FIFOQueue* uint8FIFOQueueHandle,uint8_t dataByte)
{
    Uint8FIFO *newNode;
    if(!uint8FIFOQueueHandle)return;
    newNode = (Uint8FIFO*)malloc(sizeof(Uint8FIFO));
    if(newNode == NULL)
    {
        return;
    }
    newNode->dataByte = dataByte;
    newNode->next = NULL;
    if(uint8FIFOQueueHandle->head == NULL)
    {
        uint8FIFOQueueHandle->head = newNode;
    }
    else
    {
        uint8FIFOQueueHandle->last->next = newNode;
    }
    uint8FIFOQueueHandle->last = newNode;
    uint8FIFOQueueHandle->queueSize++;
}


uint8_t Uint8FIFOPop(Uint8FIFOQueue* uint8FIFOQueueHandle)
{
    Uint8FIFO *headNode;
    uint8_t dataByte = 0;
    if(!uint8FIFOQueueHandle)return NULL;
    if(uint8FIFOQueueHandle->head == NULL)
    {
        return NULL;
    }
    headNode = uint8FIFOQueueHandle->head;
    dataByte = headNode->dataByte;
    uint8FIFOQueueHandle->head = uint8FIFOQueueHandle->head->next;
    uint8FIFOQueueHandle->queueSize--;
    free(headNode);
    return dataByte;
}


uint16_t Uint8FIFOGetQueueSize(Uint8FIFOQueue* uint8FIFOQueueHandle)
{
    if(!uint8FIFOQueueHandle)return NULL;
    return uint8FIFOQueueHandle->queueSize;
}

void Uint8FIFOPopToStream(Uint8FIFOQueue* uint8FIFOQueueHandle, uint8_t* uint8BytesStream, uint16_t length)
{
    uint16_t count = 0;
    if(!uint8FIFOQueueHandle)return;
    if(!uint8BytesStream)return;
    if(Uint8FIFOGetQueueSize(uint8FIFOQueueHandle) < length)return;
    
    for(count=0;count<length;count++)
    {
        uint8BytesStream[count] = Uint8FIFOPop(uint8FIFOQueueHandle);
    }   
}



















