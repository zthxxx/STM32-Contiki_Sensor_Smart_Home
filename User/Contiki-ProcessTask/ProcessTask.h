#ifndef __PROCESS_TASK_H__
#define __PROCESS_TASK_H__
#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include <string.h>
#include "bsp_gpio.h"
#include "bsp_usart1.h"
#include "bsp_usart2.h"
#include "bsp_SysTick.h"
#include "dma.h"
#include "iwdg.h"
#include "SPI.h"
#include "delay.h"
#include "timers.h"
#include "timer4_cap.h"
#include "NVIC_CONFIG.H"

#include "wifi_config.h"
#include "wifi_function.h"
#include "oled.h"
#include "dht11.h"
#include "adc.h"
#include "MQ-2.h"
#include "HC-SR501.h"
#include "HC-SR04.h"
#include "BH1750.h"

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

#define __WIFI_MODULE_ON__          //WIFI模块
#define __OLED_MODULE_ON__          //OLED显示屏
#define __DHT11_MODULE_ON__         //温湿度传感器
#define __MQ02_MODULE_ON__          //烟雾传感器
#define __HCSR501_MODULE_ON__       //红外热释电人体传感器
#define __HCSR04_MODULE_ON__        //超声波测距模块
#define __BH1750_MODULE_ON__        //关照传感器


PROCESS_NAME(red_blink_process);
PROCESS_NAME(green_blink_process);
PROCESS_NAME(IWDG_Feed_process);
PROCESS_NAME(clock_test_process);

PROCESS_NAME(wifi_send_test_process);
PROCESS_NAME(OLED_Show_Increment_process);
PROCESS_NAME(DHT11_Read_Data_process);
PROCESS_NAME(MQ02_Read_Value_process);
PROCESS_NAME(HCSR501_Read_Status_process);
PROCESS_NAME(HCSR04_Measure_Distance_process);
PROCESS_NAME(BH1750_Measure_Lumen_process);






#endif

