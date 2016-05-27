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



typedef enum {
	Steelyard_Set_UnitPrice_Mode 		= 0x01,	//设置单价
	Steelyard_Peeling_Mode 				= 0x02,	//去皮
	Steelyard_Accumulation_Mode 		= 0x03, 	//累加
	Steelyard_Adjust_Coefficient_Mode 	= 0x04 	//手动调整系数
} Steelyard_Mode;

typedef enum {
	Steelyard_Decimal_Sign 		= 0x01,	//小数点，进位
	Steelyard_Peeling_Sign 		= 0x02,	//去皮
	Steelyard_Accumulation_Sign	= 0x03,	//累加
	Steelyard_Adjust_Coefficient_Sign 	= 0x04 	//手动调整系数
} Steelyard_Sign;


#define VK_Steelyard_UnitPrice		0x70
#define VK_Steelyard_Peeling		0x71
#define VK_Steelyard_Adjust_Zero	0x72
#define VK_Steelyard_Accumulate		0x73
#define VK_Steelyard_Convert_Unit	0x74
#define VK_Steelyard_Adjust_Coefficient	0x75




#endif




