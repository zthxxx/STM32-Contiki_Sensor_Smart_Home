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

#define __WIFI_MODULE_ON__
#define __OLED_MODULE_ON__

PROCESS(red_blink_process, "Red Blink");
PROCESS(green_blink_process, "Green Blink");
PROCESS(wifi_send_test_process, "Wifi module send data test");
PROCESS(IWDG_Feed_process, "Timing to feed dog");
AUTOSTART_PROCESSES(&etimer_process,&IWDG_Feed_process);

void BSP_Config(void)
{
    /* 初始化 */
    clock_init();
    USART1_Config(115200);
    
#ifdef __WIFI_MODULE_ON__
    WiFi_Config(); //初始化WiFi模块使用的接口和外设
#endif
    
#ifdef __OLED_MODULE_ON__
    OLED_Init(); //初始化OLED模块使用的接口和外设
#endif                
    
}


int main(void)
{	
    BSP_Config();    
    
#ifdef __OLED_MODULE_ON__
    {
        OLED_ShowString(0,0,"SPI OLED");
        OLED_ShowString(0,32,"Start OK!");
        OLED_Refresh_Gram();//更新显示
    }
#endif 

#ifdef __WIFI_MODULE_ON__    
    ESP8266_STA_TCP_Client();
#endif
    
    IWDG_Start(2);  //wifi模块透传之后开启看门狗
    
    process_init();
    autostart_start(autostart_processes);
    process_start(&red_blink_process,NULL);
    process_start(&green_blink_process,NULL);
    
#ifdef __WIFI_MODULE_ON__     
    process_start(&wifi_send_test_process,NULL);
#endif

    while (1)
    {
        do
        {
        }while (process_run()>0);
    }
}


/*******************PROCESS************************/


PROCESS_THREAD(red_blink_process, ev, data)
{
    static struct etimer et;
    PROCESS_BEGIN();
    while(1)
    {
        Contiki_etimer_DelayMS(500);
        GPIO_SetBits(GPIOA, GPIO_Pin_8);
        Contiki_etimer_DelayMS(500);
        GPIO_ResetBits(GPIOA, GPIO_Pin_8);
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

PROCESS_THREAD(wifi_send_test_process, ev, data)
{
    static struct etimer et;
    PROCESS_BEGIN();
    while(1)
    {
        
        Contiki_etimer_DelayMS(500);
        UART2_SendBuff = " asdfafasdf";
        UART2_DMA_Send_Data(UART2_SendBuff, 11);
        
        Contiki_etimer_DelayMS(500);        
        UART2_SendBuff = " 21398416hy";
        UART2_DMA_Send_Data(UART2_SendBuff, 11);
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

