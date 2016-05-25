#ifndef __HX711_H
#define __HX711_H
#include "stm32f10x.h"
#include "stdbool.h"
#include "stdint.h"
#include "stdio.h"
#include "delay.h"

#define HX711_DAT_Pin  		GPIO_Pin_8
#define HX711_DAT_Port 		GPIOC
#define HX711_SCK_Pin  		GPIO_Pin_9
#define HX711_SCK_Port		GPIOC
#define HX711_RCC_Periph	RCC_APB2Periph_GPIOC

#define HX711_Delay_us		Delay_NOP_us

#define HX711_Default_Shift	24
#define HX711_CHANNEL_A_128	25
#define HX711_CHANNEL_B_64	26
#define HX711_CHANNEL_A_64	27


void HX711_Init(void);//≥ı ºªØ
uint32_t HX711_Read_Value(void);
double HX711_Read_Weight(void);
void HX711_Zero_Offset_Adjust(void);


#endif
