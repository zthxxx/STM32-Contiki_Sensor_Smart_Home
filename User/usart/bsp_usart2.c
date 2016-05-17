/******************** (C) COPYRIGHT 2012 WildFire Team **************************

**********************************************************************************/
#include <stdarg.h>
#include "bsp_usart2.h"
#include "wifi_config.h"

USART2_Receive_Handler USART2ReceiveHandler;
uint8_t USART2_DMA_SendBuff[USART2_SEND_DMA_BUF_LENTH];


/*
 * 函数名：USART2_Config
 * 描述  ：USART2 GPIO 配置,工作模式配置
 * 输入  ：无
 * 输出  : 无
 * 调用  ：外部调用
 */
void USART2_Config(uint32_t BaudRate)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	/* config USART2 clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	/* USART2 GPIO config */
    /* Configure USART2 Tx (PA.02) as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
        
    /* Configure USART2 Rx (PA.03) as input floating */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
	  
	/* USART2 mode config */
	USART_InitStructure.USART_BaudRate = BaudRate;               
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2, &USART_InitStructure); 

	/* 使能串口2接收中断 */
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	/* 使能串口2总线空闲中断 */
	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);
	USART_ClearFlag(USART2,USART_FLAG_TC);//先清除一下发送中断标志位，会解决第一个字节丢失的问题。
	USART_Cmd(USART2, ENABLE);
    SetUSART2_NVIC_ISENABLE(ENABLE);
	USART2ReceiveHandler = ReceiveUSART2WifiCmdDelegate;
    MYDMA_Config(USART2_TX_DMA_Channel,(u32)&USART2->DR,(u32)USART2_DMA_SendBuff,ENABLE,USART2_SEND_DMA_BUF_LENTH);
}

void ChangeUSART2ReceiveMode()
{
    USART_ITConfig(USART2, USART_IT_IDLE, DISABLE);
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    SetUSART2_NVIC_ISENABLE(ENABLE);
    USART2ReceiveHandler = ReceiveUSART2PacketDelegate;
}
/*
 * 函数名：itoa
 * 描述  ：将整形数据转换成字符串
 * 输入  ：-radix =10 表示10进制，其他结果为0
 *         -value 要转换的整形数
 *         -buf 转换后的字符串
 *         -radix = 10
 * 输出  ：无
 * 返回  ：无
 * 调用  ：被USART2_printf()调用
 */
static char *itoa( int value, char *string, int radix )
{
    int     i, d;
    int     flag = 0;
    char    *ptr = string;

    /* This implementation only works for decimal numbers. */
    if (radix != 10)
    {
        *ptr = 0;
        return string;
    }

    if (!value)
    {
        *ptr++ = 0x30;
        *ptr = 0;
        return string;
    }

    /* if this is a negative value insert the minus sign. */
    if (value < 0)
    {
        *ptr++ = '-';

        /* Make the value positive. */
        value *= -1;
    }

    for (i = 10000; i > 0; i /= 10)
    {
        d = value / i;

        if (d || flag)
        {
            *ptr++ = (char)(d + 0x30);
            value -= (d * i);
            flag = 1;
        }
    }

    /* Null terminate the string. */
    *ptr = 0;

    return string;

} /* NCL_Itoa */


/*
 * 函数名：USART2_printf
 * 描述  ：格式化输出，类似于C库中的printf，但这里没有用到C库
 * 输入  ：-USARTx 串口通道，这里只用到了串口2，即USART2
 *		     -Data   要发送到串口的内容的指针
 *			   -...    其他参数
 * 输出  ：无
 * 返回  ：无 
 * 调用  ：外部调用
 *         典型应用USART2_printf( USART2, "\r\n this is a demo \r\n" );
 *            		 USART2_printf( USART2, "\r\n %d \r\n", i );
 *            		 USART2_printf( USART2, "\r\n %s \r\n", j );
 */
void USART2_printf( USART_TypeDef* USARTx, char *Data, ... )
{
	const char *s;
  int d;   
  char buf[16];

  va_list ap;
  va_start(ap, Data);

	while ( *Data != 0)     // 判断是否到达字符串结束符
	{				                          
		if ( *Data == 0x5c )  //'\'
		{									  
			switch ( *++Data )
			{
				case 'r':							          //回车符
					USART_SendData(USARTx, 0x0d);
					Data ++;
					break;

				case 'n':							          //换行符
					USART_SendData(USARTx, 0x0a);	
					Data ++;
					break;
				
				default:
					Data ++;
				    break;
			}			 
		}
		else if ( *Data == '%')
		{									  //
			switch ( *++Data )
			{				
				case 's':										  //字符串
					s = va_arg(ap, const char *);
          for ( ; *s; s++) 
					{
						USART_SendData(USARTx,*s);
						while( USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET );
          }
					Data++;
          break;

        case 'd':										//十进制
          d = va_arg(ap, int);
          itoa(d, buf, 10);
          for (s = buf; *s; s++) 
					{
						USART_SendData(USARTx,*s);
						while( USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET );
          }
					Data++;
          break;
				 default:
						Data++;
				    break;
			}		 
		} /* end of else if */
		else USART_SendData(USARTx, *Data++);
		while( USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET );
	}
}

void SetUSART2_NVIC_ISENABLE(FunctionalState isEnable)
{
    NVIC_IRQChannel_Configuration_Set(USART2_IRQn, 1, 0, isEnable);
}

void sendUart2OneByte(uint8_t byteData)
{
	
	USART_SendData(USART2, byteData);
	while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);//等待发送结束
}

void USART2_DMA_Send_Data(uint8_t *USART2_SendBuff, uint16_t DataSendLength)
{
    memcpy(USART2_DMA_SendBuff,USART2_SendBuff,DataSendLength);
    USART_DMACmd(USART2,USART_DMAReq_Tx,ENABLE); //串口向dma发出请求
    USART2_TXD_DMA_Enable(DataSendLength);
}


void ReceiveUSART2PacketDelegate(void)                	//串口中断服务程序
{
	u8 receiveByte = 0;

	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
        USART_ClearITPendingBit(USART2,USART_IT_RXNE); //清除中断标志
		receiveByte = USART_ReceiveData(USART2);//(USART1->DR);		//读取接收到的数据
        sendUart1OneByte(receiveByte);
	}
}

void ReceiveUSART2WifiCmdDelegate(void)
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

    if(USART_GetITStatus( USART2, USART_IT_IDLE ) == SET)    //数据帧接收完毕
    {
        strEsp8266_Fram_Record .InfBit .FramFinishFlag = 1;

        ch = USART_ReceiveData( USART2 ); //由软件序列清除中断标志位(先读USART_SR，然后读USART_DR)
    }	
}

/**
  * @brief  This function handles USART2 Handler.
  * @param  None
  * @retval None
  */
void USART2_IRQHandler( void )
{	
    if (USART_GetFlagStatus(USART2, USART_FLAG_ORE) != RESET)//放置ORE中断导致无数据也中断的假死
    {
        USART_ReceiveData(USART2);
    }
    USART2ReceiveHandler();   
}




/******************* (C) COPYRIGHT 2012 WildFire Team *****END OF FILE************/



