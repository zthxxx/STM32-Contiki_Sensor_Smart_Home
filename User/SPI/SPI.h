#ifndef __SPI_H
#define	__SPI_H

#include <stdio.h>
#include "stdlib.h"
#include <string.h>
#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "dma.h"
#include "bsp_usart1.h"
#include "bsp_usart2.h"
#include "NVIC_CONFIG.H"

void SPI1_Initialization(void);
void SPI1SendOneByte(uint8_t byteData);
uint8_t SPI1ReceiveOneByte(void);

void SPI2_Initialization(void);
void SPI2SendOneByte(uint8_t byteData);
uint8_t SPI2ReceiveOneByte(void);
uint8_t SPI2_WriteReadOneByte(uint8_t byteData);


#endif
