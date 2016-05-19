#ifndef __PROTOCOL_QUEUE_MANGER_H__
#define __PROTOCOL_QUEUE_MANGER_H__
#include "FIFO.h"
#include <stdbool.h>


typedef struct Uint8PacketNode
{
    uint8_t* packet;
    uint16_t packetLength;
    void* packetBlock;
    struct Uint8PacketNode* next;
    uint16_t index;
    uint8_t resendCount;
    uint8_t resendTime;
}Uint8PacketNode;

typedef struct Uint8PacketQueue
{
    Uint8PacketNode* head;
    Uint8PacketNode* last;
    Uint8PacketNode* (*CreatUint8PacketNode)(uint8_t* packet, void* packetBlock);
    void (*FreePacketNoedItem)(Uint8PacketNode* uint8PacketNodePointer);
}Uint8PacketQueue;


Uint8PacketQueue* CreatUint8PacketQueue(Uint8PacketNode* (*CreatUint8PacketNode)(uint8_t* packet, void* packetBlock));
void Uint8PacketQueuePush(Uint8PacketQueue* Uint8PacketQueueHandle, Uint8PacketNode* uint8PacketNodePointer);
Uint8PacketNode*  CreatUint8PacketNode(uint8_t* packet,  void* packetBlock);
Uint8PacketNode*  SetUint8PacketNodeLength(Uint8PacketNode* uint8PacketNodePointer,uint16_t packetLength);
void Uint8PacketQueuePushBlock(Uint8PacketQueue* Uint8PacketQueueHandle, void* packetBlock);
void Uint8PacketQueuePushStreamData(Uint8PacketQueue* Uint8PacketQueueHandle,uint8_t* packet,uint16_t packetLength);
Uint8PacketNode*  Uint8PacketQueuePop(Uint8PacketQueue* Uint8PacketQueueHandle);
void FreePacketNoedItem(Uint8PacketNode* uint8PacketNodePointer);
void DeletPacketQueueCurrentItem(Uint8PacketQueue* PacketQueueHandle, Uint8PacketNode** uint8PacketNodePointer, Uint8PacketNode** uint8PacketNodePreviousPointer);
void DeletPacketQueueConditionalItem(Uint8PacketQueue* PacketQueueHandle, bool (*PacketQueueItemCondition)(Uint8PacketNode* uint8PacketNodePointer));



#endif






