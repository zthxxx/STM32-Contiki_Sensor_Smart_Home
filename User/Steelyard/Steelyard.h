#ifndef __STEELYARD_H__
#define __STEELYARD_H__
#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "stdbool.h"
#include "stdint.h"
#include "stdio.h"

#include "bsp_SysTick.h"
#include "FIFO.h"
#include "LIFO.h"
#include "oled.h"
#include "HX711.h"
#include "Keyboard_4x5.h"
#include "Filters.h"

#include "contiki-conf.h"
#include <stdint.h>
#include <stdio.h>
#include <debug-uart.h>
#include <process.h>
#include <procinit.h>
#include <etimer.h>
#include <autostart.h>
#include <clock.h>
#include "contiki_delay.h"
#include "ProcessTask.h"

typedef enum {
	Steelyard_Value_Key 	= 0x01,	//数值键
	Steelyard_Mode_Key 		= 0x02,	//控制键
    Steelyard_Control_Key 	= 0x03,	//控制键
} Steelyard_Key_Type;


//typedef enum {
//	Steelyard_Set_UnitPrice_Mode 		= 0x01,	//设置单价
//	Steelyard_Peeling_Mode 				= 0x02,	//去皮
//	Steelyard_Accumulation_Mode 		= 0x03, //累加
//	Steelyard_Adjust_Coefficient_Mode 	= 0x04 	//手动调整系数
//} Steelyard_Mode;

typedef enum {
    Steelyard_Decimal_Sign 		= 0x00,	//小数点，进位
    Steelyard_Set_UnitPrice_Sign= 0x01,	//设置单价
	Steelyard_Accumulation_Sign	= 0x02,	//累加
	Steelyard_Adjust_Coefficient_Sign 	= 0x03,	//手动调整系数
    Steelyard_Inputting_Sign 	= 0x04 	//正在输入
} Steelyard_Sign;

#define Steelyard_Weight_Row    0
#define Steelyard_Price_Row     1
#define Steelyard_UnitPrice_Row 2
#define Steelyard_Total_Row     3
#define Steelyard_Adjust_Weight_Row    3


//控制按键
#define VK_Steelyard_Adjust_Zero	0x0E
#define VK_Steelyard_Peeling		0x0F
#define VK_Steelyard_Convert_Unit	0x10

//模式按键
#define VK_Steelyard_UnitPrice		0x71
#define VK_Steelyard_Accumulate		0x72
#define VK_Steelyard_Adjust_Coefficient	0x73



typedef void(*Steelyard_Key_Process)(uint8_t key_index);

extern float Steelyard_CurrentlyWeight;
extern float Steelyard_UnitPrice;
extern float Steelyard_CurrentlyPrice;
extern float Steelyard_TotalPrice; 

extern bool Steelyard_Is_Decimal;
extern bool Steelyard_Is_Set_UnitPrice;
extern bool Steelyard_Is_Accumulation;
extern bool Steelyard_Is_Adjust_Coefficient;
extern bool Steelyard_Is_Inputting;

extern uint8_t Steelyard_Display_Row_Head_Length[];
extern uint8_t Steelyard_Display_Row_Endding_Length[];

void Steelyard_Display_Weight(void);
void Steelyard_Display_Price(void);
void Steelyard_Display_UnitPrice(void);
void Steelyard_Display_Total(void);

float Steelyard_Get_CurrentlyPrice(void);
void Steelyard_Dispose_ValueKey(uint8_t key_index);
void Steelyard_Dispose_Mode_Key(uint8_t key_index);
void Steelyard_Dispose_Control_Key(uint8_t key_index);

PROCESS_NAME(Steelyard_Load_Key_process);


#endif




