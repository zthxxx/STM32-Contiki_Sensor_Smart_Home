#ifndef __WIFI_CONFIG_H
#define	__WIFI_CONFIG_H


#include "stm32f10x.h"
#include <stdbool.h>

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif


//* 数据类型 */
typedef enum{
	STA,
  AP,
  STA_AP  
} ENUM_Net_ModeTypeDef;


typedef enum{
	 enumTCP,
	 enumUDP,
} ENUM_NetPro_TypeDef;
	

typedef enum{
	Multiple_ID_0 = 0,
	Multiple_ID_1 = 1,
	Multiple_ID_2 = 2,
	Multiple_ID_3 = 3,
	Multiple_ID_4 = 4,
	Single_ID     = 5,
} ENUM_ID_NO_TypeDef;
	

typedef enum{
	OPEN_PWD = 0,
	WEP = 1,
	WPA_PSK = 2,
	WPA2_PSK = 3,
	WPA_WPA2_PSK = 4,
} ENUM_AP_PsdMode_TypeDef;


#define RX_BUF_MAX_LEN     1024                                     //最大接收缓存字节数

extern struct  STRUCT_USARTx_Fram                                  //串口数据帧的处理结构体
{
	char  Data_RX_BUF[ RX_BUF_MAX_LEN ];
	
  union {
    __IO u16 InfAll;
    struct {
		  __IO u16 FramLength       :15;                               // 14:0 
		  __IO u16 FramFinishFlag   :1;                                // 15 
	  } InfBit;
  }; 
	
} strPc_Fram_Record, strEsp8266_Fram_Record;

extern struct  STRUCT_USART1_1_Fram                                  //串口数据帧的处理结构体
{
	char  Data_RX_BUF[ RX_BUF_MAX_LEN ];
	
  union {
    __IO u16 InfAll;
    struct {
		  __IO u16 FramLength       :15;                               // 14:0 
		  __IO u16 FramFinishFlag   :1;                                // 15 
	  } InfBit;
  }; 
	
} strPc1_1_Fram_Record, str1_1esp8266;

#define WIFI_RST_GPIO_Pin GPIO_Pin_1
#define WIFI_RST_GPIO_Port GPIOA
//* 函数 */
#define WiFi_RST_INIT                  ESP8266_WIFI_GPIO_Config                //WiFi使用的ES引脚初始化函数，推挽输出 



void WiFi_Config( void );
void NVIC_Configuration( void );
void ESP8266_WIFI_GPIO_Config( void );

extern bool IS_WIFI_LOG_DEBUG;
extern char WIFI_SSID_NAME[];
extern char WIFI_PASSWORD[];
extern char TCP_SERVER_IPADDRESS[];
extern char TCP_SERVER_PORT[];
extern char TCP_CILENT_IPADDRESS[];
extern char TCP_CILENT_PORT[];

#endif    /* __WIFI_CONFIG_H */
