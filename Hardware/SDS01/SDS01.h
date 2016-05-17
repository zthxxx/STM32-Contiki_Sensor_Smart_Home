#ifndef __SDS01_H__
#define __SDS01_H__
#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "bsp_uart4.h"
#include "FIFO.h"

#define SDS01_PROTOCOL_PACKET_LENGTH         10

typedef struct SDS01_PacketNode
{
    uint8_t packetHead;
    uint8_t signalType;
    uint8_t PM2_5_ByteLow;
    uint8_t PM2_5_ByteHigh;
    float PM2_5_Value;
    uint8_t PM10_ByteLow;
    uint8_t PM10_ByteHigh;
    float PM10_Value;
    uint8_t ID_ByteLow;
    uint8_t ID_ByteHigh;
    uint8_t checkSum;
    uint8_t packetEndding;
}SDS01_PacketNode;


void SDS01_Init(void);
void SDS01_PushReceiveByteDataIntoReceiveFIFO(uint8_t streamByteData);
void SDS01_LoadReceiveQueueByteToPacketBlock(void);
float SDS01_getPM2_5_Value(void);
float SDS01_getPM10_Value(void);


#endif













