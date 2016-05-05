#include "stdlib.h"
#include "FIFO.h"



Uint8FIFOQueue* CreatUint8FIFOQueue()
{
    Uint8FIFOQueue*  Uint8FIFOQueueHand;
    Uint8FIFOQueueHand = (Uint8FIFOQueue*)malloc(sizeof(Uint8FIFOQueue));
    return Uint8FIFOQueueHand;
}



void Uint8FIFOPush(Uint8FIFOQueue* Uint8FIFOQueueHand,uint8_t data)
{
    Uint8FIFO *newNode;
    newNode = (Uint8FIFO*)malloc(sizeof(Uint8FIFO));
    newNode->data = data;
    newNode->next = NULL;
    if(Uint8FIFOQueueHand->head == NULL)
    {
        Uint8FIFOQueueHand->head = newNode;
    }
    else
    {
        Uint8FIFOQueueHand->last->next = newNode;
    }
    Uint8FIFOQueueHand->last = newNode;
}


uint8_t Uint8FIFOPop(Uint8FIFOQueue* Uint8FIFOQueueHand)
{
    Uint8FIFO *head;
    uint8_t data = 0;
    if(Uint8FIFOQueueHand->head == NULL)
    {
        return NULL;
    }
    head = Uint8FIFOQueueHand->head;
    data = head->data;
    Uint8FIFOQueueHand->head = head->next;
    free(head);
    return data;
}

