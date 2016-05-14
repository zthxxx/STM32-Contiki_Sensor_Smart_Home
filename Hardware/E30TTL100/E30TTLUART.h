#ifndef __E30TTLUART_H
#define __E30TTLUART_H
#include "sys.h"   
#include "bsp_usart3.h"

#define E30TTLUART_SendOneByte SendUSART3OneByte
#define E30TTLUART_SendBytesBuf USART3_DMA_Send_Data
#define E30TTLUART_ChangeBaudRate ChangeUSART3BaudRate

#define E30TTLUART_MODE_0 0
#define E30TTLUART_MODE_1 1
#define E30TTLUART_MODE_2 2
#define E30TTLUART_MODE_3 3

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


void E30TTLUART_Init(void);//初始化E30TTL模块引脚
void E30TTLUART_Config(uint16_t E30TTLUART_Address,uint8_t E30TTLUART_Channel,FunctionalState isPoint2Point);//配置模块
void E30TTLUART_SendBytesData(uint8_t *USART3_SendBuff, uint16_t DataSendLength);
void E30TTLUART_Point2PointSendBytesData(uint16_t address,uint8_t channel,uint8_t* bytesBuf, uint16_t bytesBufLength);
#endif















