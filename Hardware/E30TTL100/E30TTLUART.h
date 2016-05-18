#ifndef __E30TTLUART_H
#define __E30TTLUART_H
#include "sys.h"   
#include "bsp_usart3.h"

#define E30TTLUART_SendOneByte          SendUSART3OneByte
#define E30TTLUART_ChangeBaudRate       ChangeUSART3BaudRate
#define E30TTLUART_UART_SendBytesBuf    USART3_DMA_Send_Data
#define E30TTLUART_UART_SendBuf_Force_Block SendUSART3BytesBuf
#define E30TTLUART_UART_AppointSendBytesBuf SendUSART3BytesBuf     //指定发送时需要的发送函数不能是DMA方式


#define E30TTLUART_M0_Port GPIOB
#define E30TTLUART_M0_Pin  GPIO_Pin_8

#define E30TTLUART_M1_Port GPIOB
#define E30TTLUART_M1_Pin  GPIO_Pin_9

#define E30TTLUART_AUX_Port GPIOB
#define E30TTLUART_AUX_Pin  GPIO_Pin_7

////IO操作函数											   
#define	E30TTLUART_M0_OUT    PBout(8)
#define	E30TTLUART_M1_OUT    PBout(9)
#define	E30TTLUART_AUX_IN    PBin(7)  //数据端口	PA0 


#define E30TTLUART_MODE_0 0
#define E30TTLUART_MODE_1 1
#define E30TTLUART_MODE_2 2
#define E30TTLUART_MODE_3 3

void E30TTLUART_Init(void);//初始化E30TTL模块引脚
void E30TTLUART_Config(uint16_t E30TTLUART_Address,uint8_t E30TTLUART_Channel,FunctionalState isPoint2Point);//配置模块
void E30TTLUART_MultiCountConfig(uint16_t E30TTLUART_Address,uint8_t E30TTLUART_Channel,FunctionalState isPoint2Point,uint8_t reConfigTimes);//多次配置生效
void E30TTLUART_TransparentSendBytesData(uint8_t* bytesBuf, uint16_t bytesBufLength);
void E30TTLUART_Point2PointSendBytesData(uint16_t address,uint8_t channel,uint8_t* bytesBuf, uint16_t bytesBufLength);
void E30TTLUART_AppointSendBytesData(uint8_t *USART3_SendBuff, uint16_t DataSendLength);
extern void (*E30TTLUART_SendBytesData)(uint8_t *USART3_SendBuff, uint16_t DataSendLength);//对外API,提供发送接口，对内区分是否透传
#endif















