#ifndef __COMMUNICATION_CONFIG_H__
#define __COMMUNICATION_CONFIG_H__


#include "CommunicationProtocol.h"
#include "ProtocolQueueManger.h"



#define PROTOCOL_PACKET_RESENT_COUNT_MAX    2
#define PROTOCOL_PACKET_RESENT_TIME_MAX     1

//使用串口1或者2发送  
//#define sendUartByteBuf   USART1_DMA_Send_Data
#define sendUartByteBuf   USART2_DMA_Send_Data
//#define sendUartByteBuf   E30TTLUART_SendBytesData



extern const uint16_t Protocol_LoopbackAddress;
extern const uint16_t Protocol_LocalhostAddress;
extern const uint16_t Protocol_BroadcastAddress;
extern const uint16_t Protocol_DefaultGatewayAddress;




#endif






