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
  
#include "bsp_uart4.h"


uint8_t UART4_DMA_SendBuff[UART4_SEND_DMA_BUF_LENTH];


void UART4_NVIC_Configuration(FunctionalState isEnable)
{
    NVIC_IRQChannel_Configuration_Set(UART4_IRQn, 3, 3, isEnable);
}



 /**
  * @brief  UART4 GPIO 配置,工作模式配置。115200 8-N-1
  * @param  无
  * @retval 无
  */
void UART4_Config(uint32_t BaudRate)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	/* config UART4 clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
    
	/* UART4 GPIO config */
	/* Configure UART4 Tx (PB.10) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);    
	
	/* Configure UART4 Rx (PB.11) as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	/* UART4 mode config */
	USART_InitStructure.USART_BaudRate = BaudRate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(UART4, &USART_InitStructure);
    /* 使能串口4接收中断 */
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);	
    USART_Cmd(UART4, ENABLE);
    UART4_NVIC_Configuration(ENABLE);
    MYDMA_Config(UART4_TX_DMA_Channel,(u32)&UART4->DR,(u32)UART4_DMA_SendBuff,ENABLE,UART4_SEND_DMA_BUF_LENTH);

}


void ChangeUART4BaudRate(uint32_t BaudRate, FunctionalState ReceiveITState)
{
    USART_InitTypeDef USART_InitStructure;
    /* UART4 mode config */
	USART_InitStructure.USART_BaudRate = BaudRate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(UART4, &USART_InitStructure);
    USART_ITConfig(UART4, USART_IT_RXNE, ReceiveITState);	
    USART_Cmd(UART4, ENABLE);
}


void SendUART4OneByte(uint8_t byteData)//串口发送信息,通过查询方式发送一个字符
{
    USART_ClearFlag(UART4,USART_FLAG_TC);//先清除一下发送中断标志位，会解决第一个字节丢失的问题。
	USART_SendData(UART4, byteData);
	while(USART_GetFlagStatus(UART4,USART_FLAG_TC)!=SET);//等待发送结束
}

void SendUART4BytesBuf(uint8_t* bytesBuf, uint16_t bytesBufLength)
{    
	while(bytesBufLength--)
	{
		SendUART4OneByte(*(bytesBuf++));
	}
}

void UART4_DMA_Send_Data(uint8_t *UART4_SendBuff, uint16_t DataSendLength)
{
    memcpy(UART4_DMA_SendBuff,UART4_SendBuff,DataSendLength);
    USART_DMACmd(UART4,USART_DMAReq_Tx,ENABLE); //串口向dma发出请求
    UART4_TXD_DMA_Enable(DataSendLength);
}

  /*******************************************************************************
* Function Name  : UART4_IRQHandler
* Description    : This function handles UART4 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UART4_IRQHandler(void)
{
	u8 receiveByte = 0;
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
        USART_ClearITPendingBit(UART4,USART_IT_RXNE); //清除中断标志
		receiveByte = USART_ReceiveData(UART4);//(UART4->DR);		//读取接收到的数据
//        sendUart1OneByte(receiveByte);
//        SendUART4OneByte(receiveByte);
//        PushReceiveByteDataIntoReceiveFIFO(receiveByte);
	}
}


/*********************************************END OF FILE**********************/
