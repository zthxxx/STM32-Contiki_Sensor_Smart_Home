#ifndef __T6603_H__
#define __T6603_H__
#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "bsp_uart5.h"
#include "FIFO.h"

#define T6603_USART_SendBytesBuf            SendUART5BytesBuf
#define T6603_RECEIVE_PACKET_HEAD_LENGTH	3
#define T6603_PROTOCOL_PACKET_LENGTH        (T6603_RECEIVE_PACKET_HEAD_LENGTH + 2)

typedef struct T6603_PacketNode
{
    uint8_t packetHead[T6603_RECEIVE_PACKET_HEAD_LENGTH];
    uint8_t CO2_PPM_ByteLow;
    uint8_t CO2_PPM_ByteHigh;
	uint16_t CO2_PPM_Value;
}T6603_PacketNode;


void T6603_Init(void);
void T6603_SendReadCMD(void);
void T6603_PushReceiveByteIntoFIFO(uint8_t streamByteData);
void T6603_LoadReceiveQueueByteToPacketBlock(void);
uint16_t T6603_getCO2_PPM(void);


#endif













