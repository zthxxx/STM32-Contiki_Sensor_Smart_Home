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
    SysTick_Init();                                                                 //配置 SysTick 为 1ms 中断一次 

    PC_Usart("\r\nESP8266 WiFi模块测试\r\n");                            //打印测试例程提示信息


    ESP8266_STA_TCP_Client();

    while(1);
	
}


/*********************************************END OF FILE**********************/
