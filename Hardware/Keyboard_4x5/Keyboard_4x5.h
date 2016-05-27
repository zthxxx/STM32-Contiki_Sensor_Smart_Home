#ifndef __KEYBOARD_4x5_H__
#define __KEYBOARD_4x5_H__
#include "stm32f10x.h"
#include "stdbool.h"
#include <math.h>
#include "stdint.h"
#include "stdio.h"
#include "bsp_SysTick.h"
#include "contiki_delay.h"
#include "FIFO.h"

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
#define KEY_COL_3_Pin  		GPIO_Pin_1
#define KEY_COL_3_Port 		GPIOB
#define KEY_COL_4_Pin  		GPIO_Pin_14
#define KEY_COL_4_Port 		GPIOB
#define KEY_COL_5_Pin  		GPIO_Pin_15
#define KEY_COL_5_Port 		GPIOB

#define KEYBOARD_RCC_Periph	RCC_APB2Periph_GPIOB
#define KEYBOARD_Delay_ms	Delay_ms





//统一虚拟按键码 仿winuser.h
#define VK_NUMPAD0        0x60
#define VK_NUMPAD1        0x61
#define VK_NUMPAD2        0x62
#define VK_NUMPAD3        0x63
#define VK_NUMPAD4        0x64
#define VK_NUMPAD5        0x65
#define VK_NUMPAD6        0x66
#define VK_NUMPAD7        0x67
#define VK_NUMPAD8        0x68
#define VK_NUMPAD9        0x69
#define VK_DECIMAL        0x6E

#define VK_CLEAR          0x0C
#define VK_RETURN         0x0D




void KEYBOARD_Init(void);
uint16_t KEYBOARD_Get_Button_Queue_Size(void);
uint8_t KEYBOARD_Read_Button(void);
void KEYBOARD_Push_Button_IntoQueue(uint8_t Keyboard_Button_Index);
uint8_t KEYBOARD_Get_Top_Button(void);
void KEYBOARD_Scan_Catch(void);

#endif




