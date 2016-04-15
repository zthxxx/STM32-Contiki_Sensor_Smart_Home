/**

  ******************************************************************************
  */ 
 
#include "stm32f10x.h"
#include "bsp_gpio.h"
#include "bsp_usart1.h"
#include "bsp_usart2.h"
#include "wifi_config.h"
#include "wifi_function.h"
#include "bsp_SysTick.h"
#include "dma.h"
#include <string.h>
 
 
void ( * pNet_Test ) ( void );


/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{	
    /* 初始化 */
    WiFi_Config();                                                                  //初始化WiFi模块使用的接口和外设
    SysTick_Init();                   /* 延时函数及时钟初始化 */
    
    ESP8266_STA_TCP_Client();

    while(1)
    {
        Delay_ms(500);
        GPIO_SetBits(GPIOA, GPIO_Pin_8);
        UART2_SendBuff = " asdfafasdf";
        UART2_DMA_Send_Data(UART2_SendBuff, 11);
        Delay_ms(500);
        GPIO_ResetBits(GPIOA, GPIO_Pin_8);
        UART2_SendBuff = " 21398416";
        UART2_DMA_Send_Data(UART2_SendBuff, 11);
    }
}


/*********************************************END OF FILE**********************/
