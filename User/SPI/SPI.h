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

void SPI1_Init(void);
uint8_t SPI1_WriteReadOneByte(uint8_t byteData);

void SPI2_Init(void);
uint8_t SPI2_WriteReadOneByte(uint8_t byteData);

void SPI3_Init(void);
uint8_t SPI3_WriteReadOneByte(uint8_t byteData);
void SPI3_Send_Byte(uint8_t dat);
uint16_t SPI3_Receive_Data(void);
#endif
