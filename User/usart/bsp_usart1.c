/**
  ******************************************************************************
  * @file    bsp_usart1.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   usart应用bsp
  ******************************************************************************

  ******************************************************************************
  */
  
#include "bsp_usart1.h"


uint8_t USART1_DMA_SendBuff[USART1_SEND_DMA_BUF_LENTH];


void USART1_NVIC_Configuration(FunctionalState isEnable)
{
    NVIC_IRQChannel_Configuration_Set(USART1_IRQn, 3, 0, isEnable);
}



 /**
  * @brief  USART1 GPIO 配置,工作模式配置。115200 8-N-1
  * @param  无
  * @retval 无
  */
void USART1_Config(uint32_t BaudRate)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	/* config USART1 clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);
	
	/* USART1 GPIO config */
	/* Configure USART1 Tx (PA.09) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);    
	
	/* Configure USART1 Rx (PA.10) as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* USART1 mode config */
	USART_InitStructure.USART_BaudRate = BaudRate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);
    /* 使能串口2接收中断 */
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);	
    USART_Cmd(USART1, ENABLE);
    USART1_NVIC_Configuration(ENABLE);
    MYDMA_Config(USART1_DMA_Channel,(u32)&USART1->DR,(u32)USART1_DMA_SendBuff,ENABLE,USART1_SEND_DMA_BUF_LENTH);

}


/// 重定向c库函数printf到USART1
int fputc(int ch, FILE *f)
{
		/* 发送一个字节数据到USART1 */
		USART_SendData(USART1, (uint8_t) ch);
		
		/* 等待发送完毕 */
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);		
	
		return (ch);
}

/// 重定向c库函数scanf到USART1
int fgetc(FILE *f)
{
		/* 等待串口1输入数据 */
		while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);

		return (int)USART_ReceiveData(USART1);
}



void sendUart1OneByte(uint8_t byteData)//串口发送信息,通过查询方式发送一个字符
{
    USART_ClearFlag(USART1,USART_FLAG_TC);//先清除一下发送中断标志位，会解决第一个字节丢失的问题。
	USART_SendData(USART1, byteData);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
}

void sendUart1BytesBuf(uint8_t* bytesBuf, uint16_t bytesBufLength)
{    
	while(bytesBufLength--)
	{
		sendUart1OneByte(*(bytesBuf++));
	}
}

void USART1_DMA_Send_Data(uint8_t *USART1_SendBuff, uint16_t DataSendLength)
{
    memcpy(USART1_DMA_SendBuff,USART1_SendBuff,DataSendLength);
    USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE); //串口向dma发出请求
    USART1_TXD_DMA_Enable(DataSendLength);
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
	u8 receiveByte = 0;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
        USART_ClearITPendingBit(USART1,USART_IT_RXNE); //清除中断标志
		receiveByte = USART_ReceiveData(USART1);//(USART1->DR);		//读取接收到的数据
        sendUart2OneByte(receiveByte);
	}
}


/*********************************************END OF FILE**********************/
