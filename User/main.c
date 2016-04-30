/**

  ******************************************************************************
  */ 
 
#include "stm32f10x.h"
#include <string.h>
#include "bsp_gpio.h"
#include "bsp_usart1.h"
#include "bsp_usart2.h"
#include "wifi_config.h"
#include "wifi_function.h"
#include "bsp_SysTick.h"
#include "dma.h"
#include "iwdg.h"
#include "SPI.h"
#include "oled.h"

#include "contiki-conf.h"
#include <stdint.h>
#include <stdio.h>
#include <debug-uart.h>
#include <process.h>
#include <procinit.h>
#include <etimer.h>
#include <autostart.h>
#include <clock.h>
#include "contiki_delay.h"


PROCESS(red_blink_process, "Red Blink");
PROCESS(green_blink_process, "Green Blink");
PROCESS(IWDG_Feed_process, "Timing to feed dog");
AUTOSTART_PROCESSES(&IWDG_Feed_process);

void BSP_Config(void)
{
    /* 初始化 */
    clock_init();
    USART1_Config(115200);
    WiFi_Config();                       //初始化WiFi模块使用的接口和外设
    OLED_Init();
}


int main(void)
{	
    BSP_Config();    
    
    OLED_ShowString(0,0,"SPI OLED");
	OLED_ShowString(0,32,"Start OK!");
	OLED_Refresh_Gram();//更新显示
    ESP8266_STA_TCP_Client();
    
    IWDG_Start(2);  //wifi模块透传之后开启看门狗
    
    process_init();
    process_start(&etimer_process,NULL);
    autostart_start(autostart_processes);
    process_start(&red_blink_process,NULL);
    process_start(&green_blink_process,NULL);
    
    while (1)
    {
        do
        {
        }while (process_run()>0);
    }
}





PROCESS_THREAD(red_blink_process, ev, data)
{
    static struct etimer et;
    PROCESS_BEGIN();
    while(1)
    {
        
        Contiki_etimer_DelayMS(500);
    
        GPIO_SetBits(GPIOA, GPIO_Pin_8);
        UART2_SendBuff = " asdfafasdf";
        UART2_DMA_Send_Data(UART2_SendBuff, 11);
        
        Contiki_etimer_DelayMS(500);
        
        GPIO_ResetBits(GPIOA, GPIO_Pin_8);
        UART2_SendBuff = " 21398416";
        UART2_DMA_Send_Data(UART2_SendBuff, 11);
    }
    PROCESS_END();
}

PROCESS_THREAD(green_blink_process, ev, data)
{
    static struct etimer et;
    PROCESS_BEGIN();
    while(1)
    {
        Contiki_etimer_DelayMS(200);
        GPIO_SetBits(GPIOD, GPIO_Pin_2);
        Contiki_etimer_DelayMS(200);
        GPIO_ResetBits(GPIOD, GPIO_Pin_2);
    }
    PROCESS_END();
}

PROCESS_THREAD(IWDG_Feed_process, ev, data)
{
    static struct etimer et;
    PROCESS_BEGIN();
    while(1)
    {
        IWDG_Feed();
        Contiki_etimer_DelayMS(1000);
    }
    PROCESS_END();
}

