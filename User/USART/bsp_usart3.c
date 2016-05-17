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
  
#include "bsp_usart3.h"


uint8_t USART3_DMA_SendBuff[USART3_SEND_DMA_BUF_LENTH];


void USART3_NVIC_Configuration(FunctionalState isEnable)
{
    NVIC_IRQChannel_Configuration_Set(USART3_IRQn, 3, 1, isEnable);
}



 /**
  * @brief  USART3 GPIO 配置,工作模式配置。115200 8-N-1
  * @param  无
  * @retval 无
  */
void USART3_Config(uint32_t BaudRate)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	/* config USART3 clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    
	/* USART3 GPIO config */
	/* Configure USART3 Tx (PB.10) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);    
	
	/* Configure USART3 Rx (PB.11) as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	/* USART3 mode config */
	USART_InitStructure.USART_BaudRate = BaudRate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART3, &USART_InitStructure);
    /* 使能串口3接收中断 */
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);	
    USART_Cmd(USART3, ENABLE);
    USART3_NVIC_Configuration(ENABLE);
    MYDMA_Config(USART3_TX_DMA_Channel,(u32)&USART3->DR,(u32)USART3_DMA_SendBuff,ENABLE,USART3_SEND_DMA_BUF_LENTH);

}


void ChangeUSART3BaudRate(uint32_t BaudRate, FunctionalState ReceiveITState)
{
    USART_InitTypeDef USART_InitStructure;
    /* USART3 mode config */
	USART_InitStructure.USART_BaudRate = BaudRate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART3, &USART_InitStructure);
    USART_ITConfig(USART3, USART_IT_RXNE, ReceiveITState);	
    USART_Cmd(USART3, ENABLE);
}


void SendUSART3OneByte(uint8_t byteData)//串口发送信息,通过查询方式发送一个字符
{
    USART_ClearFlag(USART3,USART_FLAG_TC);//先清除一下发送中断标志位，会解决第一个字节丢失的问题。
	USART_SendData(USART3, byteData);
	while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//等待发送结束
}

void SendUSART3BytesBuf(uint8_t* bytesBuf, uint16_t bytesBufLength)
{    
	while(bytesBufLength--)
	{
		SendUSART3OneByte(*(bytesBuf++));
	}
}

void USART3_DMA_Send_Data(uint8_t *USART3_SendBuff, uint16_t DataSendLength)
{
    memcpy(USART3_DMA_SendBuff,USART3_SendBuff,DataSendLength);
    USART_DMACmd(USART3,USART_DMAReq_Tx,ENABLE); //串口向dma发出请求
    USART3_TXD_DMA_Enable(DataSendLength);
}

  /*******************************************************************************
* Function Name  : USART3_IRQHandler
* Description    : This function handles USART3 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART3_IRQHandler(void)
{
	u8 receiveByte = 0;
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
        USART_ClearITPendingBit(USART3,USART_IT_RXNE); //清除中断标志
		receiveByte = USART_ReceiveData(USART3);//(USART3->DR);		//读取接收到的数据
//        sendUart1OneByte(receiveByte);
//        sendUart2OneByte(receiveByte);
#ifdef __TERMINAL_ON__
        PushReceiveByteDataIntoReceiveFIFO(receiveByte);
#endif
	}
}


/*********************************************END OF FILE**********************/
