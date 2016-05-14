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
#include "E30TTLUART.h"


#define PROTOCOL_PACKET_CONSISTENT_LENGTH   8
#define PROTOCOL_PACKET_RESENT_COUNT_MAX    2
#define PROTOCOL_PACKET_RESENT_TIME_MAX     1

//使用串口1或者2发送  
//#define sendUartByteBuf   USART1_DMA_Send_Data
//#define sendUartByteBuf   USART2_DMA_Send_Data
#define sendUartByteBuf   E30TTLUART_SendBytesData

typedef enum FunctionWord_TypeDef
{ 
    FunctionWord_Null           = 0x00,
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

typedef struct PacketBlock
{
    uint8_t PacketHead[2];
    uint16_t PacketIndex;
    FunctionWord_TypeDef PacketFunctionWord;
    uint16_t PacketJSONDataLength;
    uint8_t* PacketJSONData;
    uint8_t PacketCheckSum;
}PacketBlock;


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



Uint8PacketQueue* CreatUint8PacketQueue(void);

void SendUnsentPacketQueue(void);
void SendUnackedPacketQueue(void);
void IncreaseUnackedPacketQueueResendTime(void);

void AssembleProtocolPacketPushSendQueue(FunctionWord_TypeDef FunctionWord, uint16_t JSONMessageDataLength,uint8_t* JSONMessageData);
void PushReceiveByteDataIntoReceiveQueue(uint8_t streamByteData);
void LoadReceiveQueueByteToPacketBlock(void);
void DealWithReceivePacketQueue(void);

#endif















