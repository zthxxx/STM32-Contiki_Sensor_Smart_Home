#ifndef __HCSR04_H
#define	__HCSR04_H
#include "stm32f10x.h"
#include <stdio.h>
#include <stdbool.h>
#include "timer4_cap.h"

#define	HCSR04_TRIG_PORT        GPIOA		    //TRIG       
#define	HCSR04_TRIG_PIN         GPIO_Pin_11     //TRIG  

#define	HCSR04_ECHO_PORT        GPIOB		    //ECHO 
#define	HCSR04_ECHO_PIN         GPIO_Pin_6	    //ECHO 



void UltrasonicWave_Configuration(void);               //对超声波模块初始化
void UltrasonicWave_StartMeasure(void);                //开始测距，发送一个>10us的脉冲，然后测量返回的高电平时间
uint16_t UltrasonicWave_GetDistance(void);

#endif /* __UltrasonicWave_H */

