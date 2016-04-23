#ifndef __USART2_H
#define	__USART2_H

#include <stdio.h>
#include "stdlib.h"
#include <string.h>
#include "stm32f10x.h"
#include "bsp_usart1.h"
#include "dma.h"

#define UART_SEND_DMA_BUF_LENTH        500


typedef void (*USART2_Receive_Handler)(void);
extern USART2_Receive_Handler USART2ReceiveHandler;
extern uint8_t UART2_DMA_SendBuff[UART_SEND_DMA_BUF_LENTH];
extern uint8_t *UART2_SendBuff;

void USART2_Config(uint32_t BaudRate);
void USART2_printf(USART_TypeDef* USARTx, char *Data, ...);
void sendUart2OneByte(uint8_t byteData);
void SetUART2_NVIC_ISENABLE(uint8_t isEnable);
void ReceiveUSART2PacketDelegate(void);                	//串口中断服务程序
void ReceiveUSART2WifiCmdDelegate(void);                //wifi串口中断服务程序
void UART2_DMA_Send_Data(uint8_t *UART2_SendBuff, uint16_t DataSendLength);//UART2 DMA发送串


#endif /* __USART2_H */
