/**
  ******************************************************************************
  * @file    bsp_usart3.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   usart应用bsp
  ******************************************************************************

  ******************************************************************************
  */
  
#include "bsp_uart5.h"




void UART5_NVIC_Configuration(FunctionalState isEnable)
{
    NVIC_IRQChannel_Configuration_Set(UART5_IRQn, 3, 3, isEnable);
}



 /**
  * @brief  UART5 GPIO 配置,工作模式配置。115200 8-N-1
  * @param  无
  * @retval 无
  */
void UART5_Config(uint32_t BaudRate)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	/* config UART5 clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);
    
	/* UART5 GPIO config */
	/* Configure UART5 Tx (PC.12) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);    
	
	/* Configure UART5 Rx (PD.2) as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	/* UART5 mode config */
	USART_InitStructure.USART_BaudRate = BaudRate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(UART5, &USART_InitStructure);
    /* 使能串口5接收中断 */
	USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);	
    USART_Cmd(UART5, ENABLE);
    UART5_NVIC_Configuration(ENABLE);
    USART_DMACmd(UART5,USART_DMAReq_Tx,ENABLE);
}


void ChangeUART5BaudRate(uint32_t BaudRate, FunctionalState ReceiveITState)
{
    USART_InitTypeDef USART_InitStructure;
    /* UART5 mode config */
	USART_InitStructure.USART_BaudRate = BaudRate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(UART5, &USART_InitStructure);
    USART_ITConfig(UART5, USART_IT_RXNE, ReceiveITState);	
    USART_Cmd(UART5, ENABLE);
}


void SendUART5OneByte(uint8_t byteData)//串口发送信息,通过查询方式发送一个字符
{
    USART_ClearFlag(UART5,USART_FLAG_TC);//先清除一下发送中断标志位，会解决第一个字节丢失的问题。
	USART_SendData(UART5, byteData);
	while(USART_GetFlagStatus(UART5,USART_FLAG_TC)!=SET);//等待发送结束
}

void SendUART5BytesBuf(uint8_t* bytesBuf, uint16_t bytesBufLength)
{    
	while(bytesBufLength--)
	{
		SendUART5OneByte(*(bytesBuf++));
	}
}


  /*******************************************************************************
* Function Name  : UART5_IRQHandler
* Description    : This function handles UART5 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UART5_IRQHandler(void)
{
	u8 receiveByte = 0;
	if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
        USART_ClearITPendingBit(UART5,USART_IT_RXNE); //清除中断标志
		receiveByte = USART_ReceiveData(UART5);//(UART5->DR);		//读取接收到的数据

	}
}


/*********************************************END OF FILE**********************/
