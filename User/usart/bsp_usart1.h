#ifndef __USART1_H
#define	__USART1_H

#include "stm32f10x.h"
#include <stdio.h>
#include "bsp_usart2.h"

void USART1_Config(void);
int fputc(int ch, FILE *f);
int fgetc(FILE *f);
void sendUart1OneByte(uint8_t byteData);
#endif /* __USART1_H */
