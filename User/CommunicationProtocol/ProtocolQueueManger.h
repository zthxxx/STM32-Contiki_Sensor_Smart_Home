#ifndef __PROTOCOL_QUEUE_MANGER_H__
#define __PROTOCOL_QUEUE_MANGER_H__

#include "CommunicationProtocol.h"
#include "CommunicationConfig.h"
#include "FIFO.h"

typedef struct PacketBlock PacketBlock;

typedef struct Uint8PacketNode
{
    uint8_t* packet;
    PacketBlock* packetBlock;
    struct Uint8PacketNode* next;
    uint16_t index;
    uint8_t resendCount;
    uint8_t resendTime;
}Uint8PacketNode;

typedef struct Uint8PacketQueue
{
     Uint8PacketNode* head;
     Uint8PacketNode* last;   
}Uint8PacketQueue;


extern  Uint8PacketQueue* UnsentPacketQueueHandle;
extern  Uint8PacketQueue* UnackedPacketQueueHandle;
extern  Uint8PacketQueue* ReceivedPacketBlockQueueHandle;
extern  Uint8FIFOQueue* ReceiveBytesFIFOQueueHandle;



Uint8PacketQueue* CreatUint8PacketQueue(void);
void Uint8PacketQueuePush(Uint8PacketQueue* Uint8PacketQueueHandle, Uint8PacketNode* uint8PacketNodePointer);
Uint8PacketNode* CreatUint8PacketNode(uint8_t* packet,  PacketBlock* packetBlock);
void Uint8PacketQueuePushBlock(Uint8PacketQueue* Uint8PacketQueueHandle, PacketBlock* packetBlock);
void Uint8PacketQueuePushStreamData(Uint8PacketQueue* Uint8PacketQueueHandle,uint8_t* packet);
Uint8PacketNode* Uint8PacketQueuePop(Uint8PacketQueue* Uint8PacketQueueHandle);
void PushReceiveByteDataIntoReceiveFIFO(uint8_t streamByteData);





#endif






