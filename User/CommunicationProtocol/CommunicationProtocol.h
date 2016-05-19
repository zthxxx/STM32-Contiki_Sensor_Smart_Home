#ifndef __COMMUNICATION_PROTOCOL_H__
#define __COMMUNICATION_PROTOCOL_H__

#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "sys.h"   //与STM32相关的一些定义
#include "stdlib.h"
#include <string.h>
#include <stdbool.h>
#include "CommunicationConfig.h"
#include "Uint8PacketQueueManger.h"
#include "CommunicationDealPacket.h"
#include "cJSON.h"
#include "bsp_usart1.h"
#include "bsp_usart2.h"
#include "FIFO.h"
#include "E30TTLUART.h"

#define PROTOCOL_PACKET_HEAD_LENGTH         4
#define PROTOCOL_PACKET_CONSISTENT_LENGTH   (11+PROTOCOL_PACKET_HEAD_LENGTH)



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
    uint8_t head[PROTOCOL_PACKET_HEAD_LENGTH];
    uint16_t targetAddress;
    uint16_t sourceAddress;
    uint16_t index;
    FunctionWord_TypeDef functionWord;
    uint8_t headCheckSum;
    uint16_t messageDataLength;
    uint8_t* messageData;
    uint8_t messageDataCheckSum;
}PacketBlock;



extern  Uint8PacketQueue* UnsentPacketQueueHandle;
extern  Uint8PacketQueue* UnackedPacketQueueHandle;
extern  Uint8PacketQueue* ReceivedPacketBlockQueueHandle;
extern  Uint8FIFOQueue* TianMaoProtocolReceiveBytesFIFOQueueHandle;

extern uint16_t Protocol_PacketSendIndex;


void SendUnsentPacketQueue(void);
void SendUnackedPacketQueue(void);
void IncreaseUnackedPacketQueueResendTime(void);
Uint8PacketNode* CreatTianProtocolUint8PacketNode(uint8_t* packet, void* packetBlock);
void AssembleProtocolPacketPushSendQueue(uint16_t targetAddress, FunctionWord_TypeDef functionWord, uint16_t messageDataLength,uint8_t* messageData);
uint8_t* ResolvePacketStructIntoBytes(PacketBlock* packetBlock);
void LoadReceiveQueueByteToPacketBlock(void);
void DealWithReceivePacketQueue(void);
void PushTianProtocolReceiveByteIntoFIFO(uint8_t streamByteData);
#endif















