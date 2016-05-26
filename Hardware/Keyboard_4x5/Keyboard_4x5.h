#ifndef __KEYBOARD_4x5_H__
#define __KEYBOARD_4x5_H__
#include "stm32f10x.h"
#include "stdbool.h"
#include <math.h>
#include "stdint.h"
#include "stdio.h"
#include "delay.h"

#define KEY_ROW_1_Pin  		GPIO_Pin_6
#define KEY_ROW_1_Port 		GPIOB
#define KEY_ROW_2_Pin  		GPIO_Pin_7
#define KEY_ROW_2_Port 		GPIOB
#define KEY_ROW_3_Pin  		GPIO_Pin_8
#define KEY_ROW_3_Port 		GPIOB
#define KEY_ROW_4_Pin  		GPIO_Pin_9
#define KEY_ROW_4_Port 		GPIOB

#define KEY_COL_1_Pin  		GPIO_Pin_10
#define KEY_ROW_1_Port 		GPIOB
#define KEY_COL_2_Pin  		GPIO_Pin_11
#define KEY_ROW_2_Port 		GPIOB
#define KEY_COL_3_Pin  		GPIO_Pin_12
#define KEY_ROW_3_Port 		GPIOB
#define KEY_COL_4_Pin  		GPIO_Pin_13
#define KEY_ROW_4_Port 		GPIOB
#define KEY_COL_5_Pin  		GPIO_Pin_14
#define KEY_ROW_5_Port 		GPIOB

#define KEYBOARD_RCC_Periph	RCC_APB2Periph_GPIOB

#define KEYBOARD_Delay_ms	Delay_NOP_us


void KEYBOARD_Init(void);//≥ı ºªØ
uint32_t HX711_Read_Value(void);
uint8_t HX711_Get_DAT_Pin_State(void);
double HX711_Read_Weight(void);
void HX711_Zero_Offset_Adjust(void);
double HX711_Window_Filter(void);

#endif
