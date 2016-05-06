#ifndef __COMMUNICATION_PROTOCOL_H__
#define __COMMUNICATION_PROTOCOL_H__

#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "sys.h"   //与STM32相关的一些定义
#include "stdlib.h"
#include <string.h>
#include <stdbool.h>
#include "cJSON.h"
#include "bsp_usart1.h"
#include "bsp_usart2.h"
#include "FIFO.h"



#define PROTOCOL_PACKET_CONSISTENT_LENGTH   8
#define PROTOCOL_PACKET_RESENT_COUNT_MAX    2
#define PROTOCOL_PACKET_RESENT_TIME_MAX     1

#define sendUartByteBuf   UART2_DMA_Send_Data
//#define sendUartByteBuf   sendUart1BytesBuf

typedef enum
{ 
    FunctionWord_Handshake      = 0xF0,
    FunctionWord_Acknowledgement, 
    FunctionWord_RegisterDevice,
    FunctionWord_Dormant,
    FunctionWord_StartUP,
    FunctionWord_Data,
    FunctionWord_Logout,
    FunctionWord_Reboot,
    FunctionWord_Shutdown
}FunctionWord_TypeDef;


typedef struct Uint8PacketNode
{
    uint8_t* packet;
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



Uint8PacketQueue* CreatUint8PacketQueue(void);
void Uint8PacketQueuePushData(Uint8PacketQueue* Uint8PacketQueueHandle,uint8_t* packet);
void Uint8PacketQueuePush(Uint8PacketQueue* Uint8PacketQueueHandle,Uint8PacketNode* Uint8PacketNodePointer);
Uint8PacketNode* Uint8PacketQueuePop(Uint8PacketQueue* Uint8PacketQueueHandle);

void SendUnsentPacketQueue(void);
void SendUnackedPacketQueue(void);
void IncreaseUnackedPacketQueueResendTime(void);
void DeleteAckedIndexPacket(uint16_t packetAckedIndex);
void AssembleCommunicationPacket(FunctionWord_TypeDef FunctionWord, uint16_t JSONMessageDataLength,uint8_t* JSONMessageData);



#endif















