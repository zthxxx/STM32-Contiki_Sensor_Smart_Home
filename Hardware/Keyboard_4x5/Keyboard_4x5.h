#ifndef __KEYBOARD_4x5_H__
#define __KEYBOARD_4x5_H__
#include "stm32f10x.h"
#include "stdbool.h"
#include <math.h>
#include "stdint.h"
#include "stdio.h"
#include "delay.h"
#include "contiki_delay.h"

typedef uint16_t GPIO_Pin;

#define KEY_ROW_1_Pin  		GPIO_Pin_6
#define KEY_ROW_1_Port 		GPIOB
#define KEY_ROW_2_Pin  		GPIO_Pin_7
#define KEY_ROW_2_Port 		GPIOB
#define KEY_ROW_3_Pin  		GPIO_Pin_8
#define KEY_ROW_3_Port 		GPIOB
#define KEY_ROW_4_Pin  		GPIO_Pin_9
#define KEY_ROW_4_Port 		GPIOB

#define KEY_COL_1_Pin  		GPIO_Pin_10
#define KEY_COL_1_Port 		GPIOB
#define KEY_COL_2_Pin  		GPIO_Pin_11
#define KEY_COL_2_Port 		GPIOB
#define KEY_COL_3_Pin  		GPIO_Pin_12
#define KEY_COL_3_Port 		GPIOB
#define KEY_COL_4_Pin  		GPIO_Pin_13
#define KEY_COL_4_Port 		GPIOB
#define KEY_COL_5_Pin  		GPIO_Pin_14
#define KEY_COL_5_Port 		GPIOB

#define KEYBOARD_RCC_Periph	RCC_APB2Periph_GPIOB

#define KEYBOARD_Delay_ms	Contiki_etimer_DelayMS

void KEYBOARD_Init(void);
uint16_t KEYBOARD_Get_Button_Queue_Size(void);
uint8_t KEYBOARD_Get_Top_Button(void);
void KEYBOARD_Scan_Catch(void);

#endif




