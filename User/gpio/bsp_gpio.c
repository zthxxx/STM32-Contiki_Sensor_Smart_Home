/**
********************************************************

  ******************************************************************************
  */
  
#include "bsp_gpio.h"   

uint16_t beep_flag = 0;

LightLamp lamps[2] = {
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

    RCC_APB2PeriphClockCmd( LED_Red_RCC_Periph | LED_Green_RCC_Periph, ENABLE ); /*开启GPIOA的外设时钟 开启GPIOD的外设时钟*/

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;/*设置引脚模式为通用推挽输出*/   
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; /*设置引脚速率为50MHz */  	
    
    GPIO_InitStructure.GPIO_Pin = LED_Red_Pin;/*选择要控制的GPIO引脚*/	
    GPIO_Init(LED_Red_Port, &GPIO_InitStructure );/*调用库函数，初始化GPIO*/

    GPIO_InitStructure.GPIO_Pin = LED_Green_Pin;
    GPIO_Init(LED_Green_Port, &GPIO_InitStructure);
    
    LED_Red_On();
    LED_Green_Off();
}

void Beep_GPIO_Config(void)
{	
    GPIO_InitTypeDef GPIO_InitStructure;/*定义一个GPIO_InitTypeDef类型的结构体*/

    RCC_APB2PeriphClockCmd(Beep_RCC_Periph, ENABLE ); /*开启GPIOA的外设时钟 开启GPIOD的外设时钟*/

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;/*设置引脚模式为通用推挽输出*/   
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; /*设置引脚速率为50MHz */  	
    
    GPIO_InitStructure.GPIO_Pin = Beep_Pin;/*选择要控制的GPIO引脚*/	
    GPIO_Init(Beep_Port, &GPIO_InitStructure );/*调用库函数，初始化GPIO*/

    
    Beep_Off();
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

void Beep_On(void)
{
    GPIO_ResetBits(Beep_Port, Beep_Pin);
}

void Beep_Off(void)
{
    GPIO_SetBits(Beep_Port, Beep_Pin);
}
/*********************************************END OF FILE**********************/
