/**

  ******************************************************************************
  */ 
 
#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include <string.h>
#include "bsp_gpio.h"
#include "bsp_usart1.h"
#include "bsp_usart2.h"
#include "bsp_usart3.h"
#include "bsp_uart4.h"
#include "bsp_SysTick.h"
#include "dma.h"
#include "iwdg.h"
#include "SPI.h"
#include "delay.h"
#include "timers.h"
#include "timer4_cap.h"
#include "NVIC_CONFIG.H"
#include "cJSON.h"
#include "CommunicationConfig.h"
#include "CommunicationProtocol.h"

#include "wifi_config.h"
#include "wifi_function.h"
#include "oled.h"
#include "dht11.h"
#include "adc.h"
#include "MQ-2.h"
#include "HC-SR501.h"
#include "HC-SR04.h"
#include "BH1750.h"
#include "RC522.h"
#include "E30TTLUART.h"
#include "SDS01.h"
#include "SHT15.h"

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
#include "ProcessTask.h"


void BSP_Config(void)
{
    /* 初始化 */
    delay_init();
    clock_init();

    NVIC_Configuration_Init();
    LED_GPIO_Config();
    USART1_Config(115200);
    USART2_Config(115200);
    printf("Start Contiki OS\r\n");

#ifdef __OLED_MODULE_ON__
    OLED_Init(); //初始化OLED模块使用的接口和外设
    OLED_ShowString(0,0,"SPI OLED");
    OLED_ShowString(0,16,"Start OK!");
    OLED_Refresh_Gram();//更新显示
#endif         
    
#ifdef __DHT11_MODULE_ON__
    DHT11_Init(); 
#endif

#ifdef __MQ02_MODULE_ON__
    MQ02_Init();
#endif
    
#ifdef __HCSR501_MODULE_ON__
    HCSR501_Init();
#endif

#ifdef __HCSR04_MODULE_ON__
	UltrasonicWave_Configuration();               //对超声波模块初始化
#endif

#ifdef __BH1750_MODULE_ON__
	BH1750_Init();
	BH1750_Start();
#endif

#ifdef __RC522_MODULE_ON__
	RC522_Init();
#endif

#ifdef __SHT15_MODULE_ON__
	SHT15_Init();
#endif

#ifdef __SDS01_MODULE_ON__
	SDS01_Init();
#endif

#ifdef __WIFI_MODULE_ON__
    WiFi_Config(); 
    ESP8266_STA_TCP_Client();
    ChangeUSART2ReceiveMode();// 关闭串口2空闲中断 使能串口2接收中断 
#endif    

#ifdef __E30TTLUART_MODULE_ON__
	E30TTLUART_Init();
    E30TTLUART_MultiCountConfig(0x0000,0x50,DISABLE,3);
    printf("E30-TTL-100 OK.\r\n");
#endif  
}


int main(void)
{
    BSP_Config();    
    
    IWDG_Start(2);  //wifi模块透传之后开启看门狗
    
    process_init();
    autostart_start(autostart_processes);
    process_start(&red_blink_process,NULL);
    process_start(&green_blink_process,NULL);

#ifdef __CJSON_LIB_TEST__
    process_start(&cJSON_test_process,NULL);
#endif
    
#ifdef __COMMUNICAT_PROTOCOL__
    process_start(&Communication_Protocol_Send_process,NULL);
#endif  
        
#ifdef __CLOCK_TICK_TEST__
    process_start(&clock_test_process,NULL);
#endif    

#ifdef __OLED_MODULE_ON__
    process_start(&OLED_Show_Increment_process,NULL);
#endif
    
#ifdef __DHT11_MODULE_ON__
    process_start(&DHT11_Read_Data_process,NULL);
#endif   
    
#ifdef __WIFI_MODULE_TEST__     
    process_start(&wifi_send_test_process,NULL);
#endif
    
#ifdef __MQ02_MODULE_ON__     
    process_start(&MQ02_Read_Value_process,NULL);
#endif

#ifdef __HCSR501_MODULE_ON__     
    process_start(&HCSR501_Read_Status_process,NULL);
#endif

#ifdef __HCSR04_MODULE_ON__     
    process_start(&HCSR04_Measure_Distance_process,NULL);
#endif

#ifdef __BH1750_MODULE_ON__     
    process_start(&BH1750_Measure_Lumen_process,NULL);
#endif

#ifdef __RC522_MODULE_ON__     
    process_start(&RC522_Read_Card_process,NULL);
#endif

#ifdef __SHT15_MODULE_ON__
	process_start(&SHT15_Read_DATA_Value_process,NULL);
#endif

#ifdef __SDS01_MODULE_ON__
	process_start(&SDS01_Read_PM_Value_process,NULL);
#endif

#ifdef __COMMUNICAT_PROTOCOL_SENSOR_DATA__
    process_start(&CommunicatProtocol_Send_Sensor_Data,NULL);
#endif

    while (1)
    {
        do
        {
        }while (process_run()>0);
    }
}





