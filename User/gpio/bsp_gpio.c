/**
********************************************************

  ******************************************************************************
  */
  
#include "bsp_gpio.h"   

 /**
  * @brief  初始化GPIO
  * @param  无
  * @retval 无
  */
  
void LED_GPIO_Config(void)
{		
    GPIO_InitTypeDef GPIO_InitStructure;/*定义一个GPIO_InitTypeDef类型的结构体*/

    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE ); /*开启GPIOA的外设时钟*/
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOD, ENABLE ); /*开启GPIOD的外设时钟*/
													   
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;/*选择要控制的GPIO引脚*/	
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;/*设置引脚模式为通用推挽输出*/   
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; /*设置引脚速率为50MHz */  
    GPIO_Init( GPIOA, &GPIO_InitStructure );/*调用库函数，初始化GPIO*/

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    GPIO_ResetBits(GPIOA, GPIO_Pin_8);
    GPIO_ResetBits(GPIOD, GPIO_Pin_2);
}







/*********************************************END OF FILE**********************/
