#ifndef __USART1_H
#define	__USART1_H

#include "stm32f10x.h"
#include <stdio.h>
#include "NVIC_CONFIG.H"
#include "bsp_usart2.h"



void USART1_Config(uint32_t BaudRate);
int fputc(int ch, FILE *f);
int fgetc(FILE *f);
void sendUart1OneByte(uint8_t byteData);
void sendUart1BytesBuf(uint16_t bytesBufLength,uint8_t* bytesBuf);
#endif /* __USART1_H */
