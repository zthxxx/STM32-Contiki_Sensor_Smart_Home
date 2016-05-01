#ifndef __SR501_H
#define __SR501_H	 
#include "stdbool.h"
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//LED驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/2
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////
#define HCSR501_GPIO_Pin  GPIO_Pin_5
#define HCSR501_GPIO_Port GPIOB

#define HCSR501_GPIO_Read() PBin(5)// PB5


void HCSR501_Init(void);//初始化
bool HCSR501_Read_Data(void);

		 				    
#endif
