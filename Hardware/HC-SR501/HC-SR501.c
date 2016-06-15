#include "HC-SR501.h"


//初始化PB5和PE5为输出口.并使能这两个口的时钟		    
//LED IO初始化
void HCSR501_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //使能PB,PE端口时钟

    GPIO_InitStructure.GPIO_Pin = HCSR501_GPIO_Pin;				 //LED0-->PB.5 端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD ; 		 //
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
    GPIO_Init(HCSR501_GPIO_Port, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB.5
}
 

bool HCSR501_Read_Data()
{
    return HCSR501_GPIO_Read();
}



