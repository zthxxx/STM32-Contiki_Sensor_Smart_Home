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





//统一虚拟按键码 仿winuser.h 暂时够用  需要的时候自行添加
//定义数据按键0~9 
#define   VK_0	0x30 
#define   VK_1	0x31 
#define   VK_2	0x32 
#define   VK_3	0x33 
#define   VK_4	0x34 
#define   VK_5	0x35 
#define   VK_6	0x36 
#define   VK_7	0x37 
#define   VK_8	0x38 
#define   VK_9	0x39

//定义数据按键A~Z   注:只是按键，大小写是由shift Capital状态决定
#define   VK_A	0x41 
#define   VK_B	0x42 
#define   VK_C	0x43 
#define   VK_D	0x44 
#define   VK_E	0x45 
#define   VK_F	0x46 
#define   VK_G	0x47 
#define   VK_H	0x48 
#define   VK_I	0x49 
#define   VK_J	0x4A 
#define   VK_K	0x4B 
#define   VK_L	0x4C 
#define   VK_M	0x4D 
#define   VK_N	0x4E 
#define   VK_O	0x4F 
#define   VK_P	0x50 
#define   VK_Q	0x51 
#define   VK_R	0x52 
#define   VK_S	0x53 
#define   VK_T	0x54 
#define   VK_U	0x55 
#define   VK_V	0x56 
#define   VK_W	0x57 
#define   VK_X	0x58 
#define   VK_Y	0x59 
#define   VK_Z	0x5A 

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




