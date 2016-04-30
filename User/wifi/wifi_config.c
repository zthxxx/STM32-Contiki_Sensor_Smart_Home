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

char WIFI_SSID_NAME[] = "Signal_Process_PandoraBox2.4";
char WIFI_PASSWORD[] = "zhanglaoshidianhuahaoma";//此处可使用主机名和ip
char TCP_SERVER_IPADDRESS[] = "TIAN";
char TCP_SERVER_PORT[] = "31500";
char TCP_CILENT_IPADDRESS[] = "ESP_113AFD";
char TCP_CILENT_PORT[] = "50000";


/**
  * @brief  WiFi_Config wifi 初始化
  * @param  无
  * @retval 无
  */
void WiFi_Config( void )
{
    uint16_t delaytime = 00;
	WiFi_RST_INIT();
    WiFi_led_INIT();
 	WiFi_USART1_INIT(115200); 
	WiFi_USART2_INIT(115200); 
	WiFi_NVIC_INIT();
    SPI1_Initialization();
    SPI2_Initialization();
    

    SPI1SendOneByte(0xcd);
    SPI1SendOneByte(0xff);


    sendUart1OneByte(0x01);
    sendUart1OneByte(0x02);
    sendUart1OneByte(0x03);
    
    while(1)
    {
        SPI1SendOneByte(0x01);
        delay_us(delaytime);
        SPI1SendOneByte(0x02);
        delay_us(delaytime);
        SPI1SendOneByte(0x03);
        delay_us(delaytime);
        SPI1SendOneByte(0x04);
        delay_us(delaytime);
        SPI1SendOneByte(0x04);
        delay_us(delaytime);
        GPIO_WriteBit(GPIOD, GPIO_Pin_2, (BitAction)!GPIO_ReadOutputDataBit(GPIOD, GPIO_Pin_2));
    }
    
    OLED_Init();
	OLED_ShowString(0,0,"SPI OLED");
	OLED_ShowString(0,32,"Start OK!");
	OLED_Refresh_Gram();//更新显示
}


/// 配置NVIC中的UART2中断
void NVIC_Configuration( void )
{
	NVIC_InitTypeDef NVIC_InitStructure; 
	
	/* Configure the NVIC Preemption Priority Bits */  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	/* Enable the USART2 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;	 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);


   /////////////////////////////////
   /*
   配置 串口中断  1 的初始化函数
   
   */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	/* Enable the USART2 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;	 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

}


/*********************************************************end of file**************************************************/
