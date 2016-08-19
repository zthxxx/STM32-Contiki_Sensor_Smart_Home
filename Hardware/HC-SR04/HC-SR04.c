/******************** (C) 1209 Lab **************************
 * 文件名  : HC-SR04.c
 * 描述    ：超声波测距模块，UltrasonicWave_Configuration（）函数
             初始化超声模块，UltrasonicWave_StartMeasure（）函数
			 启动测距，并将测得的数据通过串口1打印出来         
 * 实验平台：Mini STM32开发板  STM32F103RBT6
 * 硬件连接：------------------
 *          | PC8  - TRIG      |
 *          | PC7  - ECHO      |
 *           ------------------
 * 库版本  ：ST3.5.0
 *
 * 作者    ：Lee 
*********************************************************************************/

#include "HC-SR04.h"
#include "timers.h"
#include "delay.h"

  
#define UltrasonicWave_DISTANCE_MAX  350

/*
 * 函数名：UltrasonicWave_Configuration
 * 描述  ：超声波模块的初始化
 * 输入  ：无
 * 输出  ：无	
 */
void UltrasonicWave_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;	
    
    RCC_APB2PeriphClockCmd(HCSR04_TRIG_RCC_Periph|HCSR04_ECHO_RCC_Periph|RCC_APB2Periph_AFIO, ENABLE);

    GPIO_InitStructure.GPIO_Pin = HCSR04_TRIG_PIN;			//PC8接TRIG
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		//设为推挽输出模式
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	         
    GPIO_Init(HCSR04_TRIG_PORT, &GPIO_InitStructure);	     //初始化外设GPIO 

    GPIO_InitStructure.GPIO_Pin = HCSR04_ECHO_PIN;				     
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;		 //设为输入
    GPIO_Init(HCSR04_ECHO_PORT,&GPIO_InitStructure);	//初始化GPIOA
    
    TIM4_Cap_Channel_1_Init(0xFFFF,720-1);
}



/*
 * 函数名：UltrasonicWave_StartMeasure
 * 描述  ：开始测距，发送一个>10us的脉冲，然后测量返回的高电平时间
 * 输入  ：无
 * 输出  ：无	
 */
void UltrasonicWave_StartMeasure(void)
{
  GPIO_SetBits(HCSR04_TRIG_PORT,HCSR04_TRIG_PIN); 		  //送>10US的高电平
  Delay_NOP_us(20);		                      //延时20US
  GPIO_ResetBits(HCSR04_TRIG_PORT,HCSR04_TRIG_PIN); 
}


uint16_t UltrasonicWave_GetDistance(void)
{
    uint16_t distance;
    distance = Channel_1_UpTime * 17/100;
    
    return (distance < UltrasonicWave_DISTANCE_MAX) ? distance : 0;
}


/******************* (C) 1209 Lab *****END OF FILE************/
