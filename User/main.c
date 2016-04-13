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
  char cCh='1';	

	/* 初始化 */
	WiFi_Config();                                                                  //初始化WiFi模块使用的接口和外设
	SysTick_Init();                                                                 //配置 SysTick 为 1ms 中断一次 

	PC_Usart("\r\n-----SDingBa----------------WiFi模块测试\r\n");                            //打印测试例程提示信息

  PC_Usart ( "\r\n请输入模块的测试模式的编号，编号如下：\
              \r\n1.STA模式： TCP Client\
              \r\n2.AP模式： AP Server\
              \r\n3.STA+AP模式： (STA)TCP Client + (AP)TCP Server\
              \r\n输入格式为：编号+空格，点击发送\r\n" );
	
// scanf ( "%c",  & cCh );

  switch ( cCh )
  {
  		GPIO_SetBits( GPIOA, GPIO_Pin_0 );
		case '1':
			pNet_Test = ESP8266_STA_TCP_Client;
		  PC_Usart("\r\n即将进行的测试是：1.STA模式： TCP Client\r\n");
		  break;
		
		case '2':
			pNet_Test = ESP8266_AP_TCP_Server;
		  PC_Usart("\r\n即将进行的测试是：2.AP模式： TCP Server\r\n");
		  break;

		case '3':
			pNet_Test = ESP8266_StaTcpClient_ApTcpServer;
		  PC_Usart("\r\n即将进行的测试是：3.STA+AP模式： (STA)TCP Client + (AP)TCP Server\r\n");
		  break;

		default:
			pNet_Test = NULL;
		  PC_Usart("\r\n输入错误，不进行测试！\r\n");
		  break;		
			
	}

	if ( pNet_Test )
		  pNet_Test ();

  while(1);
	
}


/*********************************************END OF FILE**********************/
