#ifndef __USART2_H
#define	__USART2_H


#include "stm32f10x.h"
#include "bsp_usart1.h"
#include <stdio.h>


void USART2_Config( void );
extern void USART2_printf(USART_TypeDef* USARTx, char *Data, ...);
void sendUart2OneByte(uint8_t byteData);
void SetUART2_NVIC_ISENABLE(uint8_t isEnable);

typedef void (*USART2_Receive_Handler)(void);
extern USART2_Receive_Handler USART2ReceiveHandler;
void ReceiveUSART2PacketDelegate(void);                	//串口中断服务程序
void ReceiveUSART2WifiCmdDelegate(void);                //wifi串口中断服务程序
//extern void bsp_USART2_IRQHandler(void);
//extern char *get_rebuff(uint8_t *len);
//extern void clean_rebuff(void);


#endif /* __USART2_H */
