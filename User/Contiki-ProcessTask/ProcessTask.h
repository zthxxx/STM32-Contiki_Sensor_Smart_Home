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
#include "cJSON.h"
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




PROCESS_NAME(red_blink_process);
PROCESS_NAME(green_blink_process);
PROCESS_NAME(IWDG_Feed_process);
PROCESS_NAME(Communication_Protocol_Send_process);
PROCESS_NAME(clock_test_process);
PROCESS_NAME(cJSON_test_process);

PROCESS_NAME(wifi_send_test_process);
PROCESS_NAME(OLED_Show_Increment_process);
PROCESS_NAME(DHT11_Read_Data_process);
PROCESS_NAME(MQ02_Read_Value_process);
PROCESS_NAME(HCSR501_Read_Status_process);
PROCESS_NAME(HCSR04_Measure_Distance_process);
PROCESS_NAME(BH1750_Measure_Lumen_process);
PROCESS_NAME(RC522_Read_Card_process);





#endif

