/**
********************************************************

  ******************************************************************************
  */
  
#include "bsp_gpio.h"   


LightLamp lamps[3] = {
    {
        0, 
        "Red", 
        0, 
        LED_Red_On,
        LED_Red_Off
    }, 
    {
        1, 
        "Green", 
        0, 
        LED_Green_On,
        LED_Green_Off
    },
    {
        2, 
        "Gate", 
        0, 
        Gate_On,
        Gate_Off
    }
};
 /**
  * @brief  初始化GPIO
  * @param  无
  * @retval 无
  */
  
void LED_GPIO_Config(void)
{		
    GPIO_InitTypeDef GPIO_InitStructure;/*定义一个GPIO_InitTypeDef类型的结构体*/

    RCC_APB2PeriphClockCmd( LED_Red_RCC_Periph | LED_Green_RCC_Periph | Gate_RCC_Periph, ENABLE); /*开启GPIOA的外设时钟 开启GPIOD的外设时钟*/

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;/*设置引脚模式为通用推挽输出*/   
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; /*设置引脚速率为50MHz */  	
    
    GPIO_InitStructure.GPIO_Pin = LED_Red_Pin;/*选择要控制的GPIO引脚*/	
    GPIO_Init(LED_Red_Port, &GPIO_InitStructure );/*调用库函数，初始化GPIO*/

    GPIO_InitStructure.GPIO_Pin = LED_Green_Pin;
    GPIO_Init(LED_Green_Port, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = Gate_Pin;
    GPIO_Init(Gate_Port, &GPIO_InitStructure);
    
    LED_Red_Off();
    LED_Green_Off();
    Gate_Off();
}

void LED_Red_On(void)
{
    GPIO_SetBits(LED_Red_Port, LED_Red_Pin);
}

void LED_Red_Off(void)
{
    GPIO_ResetBits(LED_Red_Port, LED_Red_Pin);
}

void LED_Green_On(void)
{
    GPIO_SetBits(LED_Green_Port, LED_Green_Pin);
}

void LED_Green_Off(void)
{
    GPIO_ResetBits(LED_Green_Port, LED_Green_Pin);
}

void Gate_On(void)
{
    GPIO_ResetBits(Gate_Port, Gate_Pin);
}

void Gate_Off(void)
{
    GPIO_SetBits(Gate_Port, Gate_Pin);
}

/*********************************************END OF FILE**********************/
