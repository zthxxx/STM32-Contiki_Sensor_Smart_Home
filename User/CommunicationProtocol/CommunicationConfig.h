#ifndef __COMMUNICATION_CONFIG_H__
#define __COMMUNICATION_CONFIG_H__


#include "CommunicationProtocol.h"
#include "ProtocolQueueManger.h"

//配置是节点模式还是终端模式，__TERMINAL_ON__表示终端模式，__TERMINAL_OFF__表示节点模式
#define __TERMINAL_ON__



//使用配置发送端口  
//#define sendUartByteBuf   USART1_DMA_Send_Data
#ifdef __TERMINAL_ON__
//    #define sendUartByteBuf   USART2_DMA_Send_Data
    #define sendUartByteBuf   SendUSART2BytesBuf
#else
    #ifdef __TERMINAL_OFF__
    #define sendUartByteBuf   E30TTLUART_SendBytesData
    #endif
#endif



//配置重发次数与超时时间
#define PROTOCOL_PACKET_RESENT_COUNT_MAX    2
#define PROTOCOL_PACKET_RESENT_TIME_MAX     1

extern const uint16_t Protocol_LoopbackAddress;
extern const uint16_t Protocol_LocalhostAddress;
extern const uint16_t Protocol_BroadcastAddress;
extern const uint16_t Protocol_GatewayAddress;




#endif






