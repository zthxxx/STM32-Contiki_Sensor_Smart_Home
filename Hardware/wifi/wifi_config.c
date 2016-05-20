/**
  ******************************************************************************
  * @file    wifi_config.c
  * @author  fire
  * @version V1.0
  * @date    2014-xx-xx
  * @brief   WiFi模块接口配置驱动
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 ISO-STM32 开发板
  * 论坛    :http://www.chuxue123.com
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
	*/

#include "wifi_config.h"
#include "bsp_SysTick.h"
#include "bsp_gpio.h"
#include "bsp_usart1.h"
#include "bsp_usart2.h"
#include "SPI.h"
#include "oled.h"

struct  STRUCT_USARTx_Fram strEsp8266_Fram_Record = { 0 };
struct  STRUCT_USART1_1_Fram str1_1esp8266 = {0} ;

bool IS_WIFI_LOG_DEBUG = true;

char WIFI_SSID_NAME[] = "Signal_Process_Download";
//char WIFI_SSID_NAME[] = "Signal_Process_PandoraBox2.4";
//char WIFI_SSID_NAME[] = "Suishenbox_F002";
char WIFI_PASSWORD[] = "zhanglaoshidianhuahaoma";//此处可使用主机名和ip
//char TCP_SERVER_IPADDRESS[] = "TIAN";
//char TCP_SERVER_IPADDRESS[] = "192.168.1.105";
char TCP_SERVER_IPADDRESS[] = "115.159.203.38";
char TCP_SERVER_PORT[] = "31511";
char TCP_CILENT_IPADDRESS[] = "ESP_113AFD";
char TCP_CILENT_PORT[] = "50000";


/**
  * @brief  WiFi_Config wifi 初始化
  * @param  无
  * @retval 无
  */
void WiFi_Config( void )
{
    USART2_Config(115200);
	WiFi_RST_INIT();
}

void ESP8266_WIFI_GPIO_Config( void )
{		
    /*定义一个GPIO_InitTypeDef类型的结构体*/
    GPIO_InitTypeDef GPIO_InitStructure;

    /*开启GPIOA的外设时钟*/
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE ); 

    /* 配置WiFi模块的片选（CH）引脚	复位重启（RST）引脚*/
    /*选择要控制的GPIOA0（CH）引脚和GPIOA1（RST）引脚*/															   
    GPIO_InitStructure.GPIO_Pin = WIFI_RST_GPIO_Pin;	

    /*设置引脚模式为通用推挽输出*/
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   

    /*设置引脚速率为50MHz */   
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

    /*调用库函数，初始化GPIOA*/
    GPIO_Init( WIFI_RST_GPIO_Port, &GPIO_InitStructure );	 

//    /*拉高WiFi模块的复位重启引脚	*/
    GPIO_SetBits( WIFI_RST_GPIO_Port, WIFI_RST_GPIO_Pin );	
}

/*********************************************************end of file**************************************************/
