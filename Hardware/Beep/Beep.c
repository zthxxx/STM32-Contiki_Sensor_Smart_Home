/**
********************************************************

  ******************************************************************************
  */
  
#include "Beep.h"   

uint16_t beep_flag = 0;


 /**
  * @brief  初始化GPIO
  * @param  无
  * @retval 无
  */

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

void Beep_On(void)
{
    GPIO_ResetBits(Beep_Port, Beep_Pin);
}

void Beep_Off(void)
{
    GPIO_SetBits(Beep_Port, Beep_Pin);
}

void SetBeepAlertSource(uint8_t alertSource)
{
    beep_flag |= 0x0001 << alertSource;
}

void ResetBeepAlertSource(uint8_t alertSource)
{
    beep_flag &= ~(uint16_t)(0x0001 << alertSource);
}

/*********************************************END OF FILE**********************/
