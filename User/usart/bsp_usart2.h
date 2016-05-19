#ifndef __USART2_H
#define	__USART2_H

#include <stdio.h>
#include "stdlib.h"
#include <string.h>
#include "stm32f10x.h"
#include "NVIC_CONFIG.H"
#include "bsp_usart1.h"
#include "dma.h"


typedef void (*USART2_Receive_Handler)(void);
extern USART2_Receive_Handler USART2ReceiveHandler;


void USART2_Config(uint32_t BaudRate);
void ChangeUSART2ReceiveMode(void);
void USART2_printf(USART_TypeDef* USARTx, char *Data, ...);
void SendUSART2OneByte(uint8_t byteData);
void SendUSART2BytesBuf(uint8_t* bytesBuf, uint16_t bytesBufLength);
void SetUSART2_NVIC_ISENABLE(FunctionalState isEnable);
void ReceiveUSART2PacketDelegate(void);                	//串口中断服务程序
void ReceiveUSART2WifiCmdDelegate(void);                //wifi串口中断服务程序
void USART2_DMA_Send_Data(uint8_t *USART_SendBuff, uint16_t DataSendLength);//USART2 DMA发送串


#endif /* __USART2_H */
