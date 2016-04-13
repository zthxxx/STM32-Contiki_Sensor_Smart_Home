/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTI
  
  AL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include <stdio.h>
#include "bsp_SysTick.h"
#include "bsp_usart2.h"
#include "wifi_config.h"
#include "wifi_function.h"
  #include <string.h>

//extern void TimingDelay_Decrement(void);
//extern void USART2_printf(USART_TypeDef* USARTx, char *Data,...);


/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
	TimingDelay_Decrement();	
}


  /*******************************************************************************
* Function Name  : USART1_IRQHandler
* Description    : This function handles USART1 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART1_IRQHandler(void)
{
/*			char ch;
	//pStr = ESP8266_ReceiveString ( DISABLE );
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		
		
		ch  = USART_ReceiveData( USART1 );
		
		if( str1_1esp8266 .InfBit .FramLength < ( RX_BUF_MAX_LEN - 1 ) )   //预留1个字节写结束符
		{
				str1_1esp8266 .Data_RX_BUF [ strEsp8266_Fram_Record .InfBit .FramLength ++ ]  = ch;

		} 

	}
	 	 
	if ( USART_GetITStatus( USART1, USART_IT_IDLE ) == SET )    //数据帧接收完毕
	{
     	 str1_1esp8266 .InfBit .FramFinishFlag = 1;
		
		ch = USART_ReceiveData( USART1 ); //由软件序列清除中断标志位(先读USART_SR，然后读USART_DR)
	
  }	
	*/

//	  //确认是否接收到数据
//       	//	uint8_t ch;
//			u32 ul = 0;
//		//	int stringvalue=0;
//			char cstring;
//			char cStrInput [100] = { 0 };
//	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
//	{ 	
//	       //ch = USART1->DR;
//			cstring = USART_ReceiveData(USART1);
//	  	//	printf( "%c", ch );    //将接受到的数据直接返回打印
//			
//			if(){
//				
//			}		
//
//			ul = strlen ( cStrInput );
//			ESP8266_SendString ( DISABLE, cStrInput, ul, ( ENUM_ID_NO_TypeDef ) 0 );
//			PC_Usart ( "%s", strEsp8266_Fram_Record .Data_RX_BUF );
//	} 
//	  
//       //接收到数据
//       if(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == SET)
//       {
//              //将数据回送至超级终端
//          //    USART_SendData(USART1, USART_ReceiveData(USART1));
////				ca  =	 USART_ReceiveData(USART1);	              
////					 if(ca=="1"){
////					  		GPIO_SetBits( GPIOA, GPIO_Pin_6 );	
////					 }
//			//	  GPIO_SetBits( GPIOA, ~GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6) );
//			 	if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6))  {//检测PB口的第10个管脚的电平
//					GPIO_ResetBits(GPIOA,GPIO_Pin_6); //使第2个管脚为 0 
//				}else{
//					GPIO_SetBits(GPIOA,GPIO_Pin_6);	 //使第2个管脚为 1
//			  }
//			  //等待数据发送完毕
//           //   while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
//             
//       }

}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/
/**
  * @brief  This function handles USART2 Handler.
  * @param  None
  * @retval None
  */
void USART2_IRQHandler( void )
{	
	char ch;
	//pStr = ESP8266_ReceiveString ( DISABLE );
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		
		
		ch  = USART_ReceiveData( USART2 );
		
		if( strEsp8266_Fram_Record .InfBit .FramLength < ( RX_BUF_MAX_LEN - 1 ) )   //预留1个字节写结束符
		{
				strEsp8266_Fram_Record .Data_RX_BUF [ strEsp8266_Fram_Record .InfBit .FramLength ++ ]  = ch;

		} 

	}
	 	 
	if ( USART_GetITStatus( USART2, USART_IT_IDLE ) == SET )    //数据帧接收完毕
	{
     	 strEsp8266_Fram_Record .InfBit .FramFinishFlag = 1;
		
		ch = USART_ReceiveData( USART2 ); //由软件序列清除中断标志位(先读USART_SR，然后读USART_DR)
	
  }	

}


/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
