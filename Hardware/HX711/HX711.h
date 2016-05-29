#ifndef __HX711_H
#define __HX711_H
#include "stm32f10x.h"
#include "stdbool.h"
#include <math.h>
#include "stdint.h"
#include "stdio.h"
#include "stm_flash.h"
#include "delay.h"
#include "Filters.h"

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

#define STM32_FLASH_HX711_Coefficient_Page (STM32_FLASH_BASE + ((STM32_FLASH_SIZE - 2)<<10))

extern double HX711_fitting_coefficient[2];

void HX711_Init(void);//³õÊ¼»¯
uint32_t HX711_Read_Value(void);
uint32_t HX711_Read_Average_Value(void);
uint8_t HX711_Get_DAT_Pin_State(void);
double HX711_Read_Weight(void);
void HX711_Save_Adjust_Coefficient(void);
void HX711_Load_Adjust_Conefficient(void);
void HX711_Zero_Offset_Adjust(void);
double HX711_Window_Filter(void);
double HX711_Window_Weighting_Filter(void);
#endif
