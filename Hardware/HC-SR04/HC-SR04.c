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


  

float UltrasonicWave_Distance;      //计算出的距离    


/*
 * 函数名：UltrasonicWave_Configuration
 * 描述  ：超声波模块的初始化
 * 输入  ：无
 * 输出  ：无	
 */
void UltrasonicWave_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;	
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    //GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);	//关闭jtag
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);

    GPIO_InitStructure.GPIO_Pin = HCSR04_TRIG_PIN;					 //PC8接TRIG
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		     //设为推挽输出模式
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	         
    GPIO_Init(HCSR04_TRIG_PORT, &GPIO_InitStructure);	                 //初始化外设GPIO 

    GPIO_InitStructure.GPIO_Pin = HCSR04_ECHO_PIN;				     //PC7接ECH0
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;		 //设为输入
    GPIO_Init(HCSR04_ECHO_PORT,&GPIO_InitStructure);						 //初始化GPIOA

    //  中断线以及中断初始化配置
    GPIO_EXTILineConfig(HCSR04_EXTI_PortSource,HCSR04_EXTI_PinSource);
    EXTI_InitStructure.EXTI_Line=HCSR04_EXTI_Line;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;//下降沿中断
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);		//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    NVIC_InitStructure.NVIC_IRQChannel = HCSR04_EXTI_IRQn;			//使能按键所在的外部中断通道
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//抢占优先级2 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;					//子优先级2 
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
    NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
   
}


void HCSR04_EXTI_IRQHandler(void)
{
    if(EXTI_GetITStatus(HCSR04_EXTI_Line) != RESET)
    {
        EXTI_ClearITPendingBit(HCSR04_EXTI_Line);  //清除EXTI7线路挂起位
        TIM_SetCounter(TIM2,0);
        TIM_Cmd(TIM2, ENABLE);                                             //开启时钟
        while(GPIO_ReadInputDataBit(HCSR04_ECHO_PORT,HCSR04_ECHO_PIN));	                 //等待低电平
        TIM_Cmd(TIM2, DISABLE);			                                 //定时器2使能
        UltrasonicWave_Distance=TIM_GetCounter(TIM2)*5*34/100.00;		//计算距离 单位cm
        if(UltrasonicWave_Distance>0 && UltrasonicWave_Distance<300)
        {
            printf("Distance: %f cm\r\n",UltrasonicWave_Distance);
        }
    }
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

/******************* (C) 1209 Lab *****END OF FILE************/
