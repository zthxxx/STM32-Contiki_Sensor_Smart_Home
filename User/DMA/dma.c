#include "dma.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK miniSTM32开发板
//DMA 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/8
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////


uint8_t USART1_DMA_SendBuff[USART1_SEND_DMA_BUF_LENTH];
bool USART1_DMA_Sendding = false;
Uint8PacketQueue USART1_DMASendPacketQueue = {NULL, NULL, CreatUint8PacketNode, FreePacketNoedItem};
Uint8PacketQueue* USART1_DMASendPacketQueueHandle = &USART1_DMASendPacketQueue;

uint8_t USART2_DMA_SendBuff[USART2_SEND_DMA_BUF_LENTH];
bool USART2_DMA_Sendding = false;
Uint8PacketQueue USART2_DMASendPacketQueue = {NULL, NULL, CreatUint8PacketNode, FreePacketNoedItem};
Uint8PacketQueue* USART2_DMASendPacketQueueHandle = &USART2_DMASendPacketQueue;

uint8_t USART3_DMA_SendBuff[USART3_SEND_DMA_BUF_LENTH];
bool USART3_DMA_Sendding = false;
Uint8PacketQueue USART3_DMASendPacketQueue = {NULL, NULL, CreatUint8PacketNode, FreePacketNoedItem};
Uint8PacketQueue* USART3_DMASendPacketQueueHandle = &USART3_DMASendPacketQueue;

uint8_t UART4_DMA_SendBuff[UART4_SEND_DMA_BUF_LENTH];
bool UART4_DMA_Sendding = false;
Uint8PacketQueue UART4_DMASendPacketQueue = {NULL, NULL, CreatUint8PacketNode, FreePacketNoedItem};
Uint8PacketQueue* UART4_DMASendPacketQueueHandle = &UART4_DMASendPacketQueue;
   
//DMA的各通道配置
//这里的传输形式是固定的,这点要根据不同的情况来修改
//从存储器->外设模式/8位数据宽度/存储器增量模式
//Channel_x:DMA通道CHx
//PeripheralBaseAddr:外设地址
//MemoryBaseAddr:存储器地址
//BufferSize:数据传输量 
void MYDMA_Config(DMA_Channel_TypeDef* Channel_x,u32 PeripheralBaseAddr,u32 MemoryBaseAddr,FunctionalState isMemoryToPeripheral,u16 BufferSize)
{
    DMA_InitTypeDef DMA_InitStructure;
 	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	                //使能DMA传输
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);	                //使能DMA传输
    DMA_DeInit(Channel_x);                                          //将DMA的通道1寄存器重设为缺省值
	DMA_InitStructure.DMA_PeripheralBaseAddr = PeripheralBaseAddr;  //DMA外设基地址
	DMA_InitStructure.DMA_MemoryBaseAddr = MemoryBaseAddr;  		//DMA内存基地址
    DMA_InitStructure.DMA_BufferSize = BufferSize;  			    //DMA通道的DMA缓存的大小
    
    if(isMemoryToPeripheral == ENABLE)
    {
        DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;                  //数据传输方向，从内存读取发送到外设
        DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;    //外设地址寄存器不变
        DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;             //内存地址寄存器递增
    }
    else
    {
        DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                  //数据传输方向，从外设读取发送到内存
        DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Enable;    //外设地址寄存器递增
        DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;             //内存地址寄存器不变
    }
    
    
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  //外设数据宽度为8位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;     //内存数据宽度为8位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                       //工作在正常缓存模式
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;               //DMA通道 x拥有中优先级 
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                        //DMA通道x没有设置为内存到内存传输
	DMA_Init(Channel_x, &DMA_InitStructure);                        //根据DMA_InitStruct中指定的参数初始化DMA的通道所标识的寄存器
	DMA_Cmd(Channel_x, ENABLE);                                         //使能所指示的通道 在每次发送请求时使能
}
//开启一次DMA传输
void MYDMA_Enable(DMA_Channel_TypeDef* DMA_CHx, u16 bufferSize)
{ 
	DMA_Cmd(DMA_CHx, DISABLE );  //关闭USART1 TX DMA1 所指示的通道      
 	DMA_SetCurrDataCounter(DMA_CHx,bufferSize);//DMA通道的DMA缓存的大小
 	DMA_Cmd(DMA_CHx, ENABLE);  //使能USART1 TX DMA1 所指示的通道 
}	  


void USART1_TXD_DMA_Enable(u16 bufferSize)
{
 	MYDMA_Enable(USART1_TX_DMA_Channel,bufferSize);//DMA通道的DMA缓存的大小
}

void USART2_TXD_DMA_Enable(u16 bufferSize)
{
 	MYDMA_Enable(USART2_TX_DMA_Channel,bufferSize);//DMA通道的DMA缓存的大小
}

void USART3_TXD_DMA_Enable(u16 bufferSize)
{
 	MYDMA_Enable(USART3_TX_DMA_Channel,bufferSize);//DMA通道的DMA缓存的大小
}

void UART4_TXD_DMA_Enable(u16 bufferSize)
{
 	MYDMA_Enable(UART4_TX_DMA_Channel,bufferSize);//DMA通道的DMA缓存的大小
}



void USART_DMA_SendPacketNode(Uint8PacketQueue* USART_DMASendPacketQueueHandle,uint8_t* USART_DMA_SendBuff,bool* USART_DMA_Sendding,void (*USART_TXD_DMA_Enable)(u16 bufferSize))
{
    if(USART_DMASendPacketQueueHandle->head)
    {
        uint8_t* packet;
        uint16_t packetLength;
        Uint8PacketNode* uint8PacketNodePointer;
        uint8PacketNodePointer = Uint8PacketQueuePop(USART_DMASendPacketQueueHandle);
        packet = uint8PacketNodePointer->packet;
        packetLength = uint8PacketNodePointer->packetLength;
        memcpy(USART_DMA_SendBuff,packet,packetLength);
        free(uint8PacketNodePointer);
        *USART_DMA_Sendding = true;
        USART_TXD_DMA_Enable(packetLength);
    }
}


/*对内封装，提供对外push进接收FIFO的接口
*
*/
void PushUSRAT1_DMA_SendDataIntoFIFO(uint8_t *USART_SendBuff, uint16_t DataSendLength)//对内封装，提供对外push进FIFO的接口
{
    Uint8PacketQueuePushStreamData(USART1_DMASendPacketQueueHandle, USART_SendBuff, DataSendLength);
    if(USART1_DMA_Sendding == false)
    {
        USART_DMA_SendPacketNode(USART1_DMASendPacketQueueHandle, USART1_DMA_SendBuff, &USART1_DMA_Sendding, USART1_TXD_DMA_Enable);
    }
}

/*对内封装，提供对外push进接收FIFO的接口
*
*/
void PushUSRAT2_DMA_SendDataIntoFIFO(uint8_t *USART_SendBuff, uint16_t DataSendLength)//对内封装，提供对外push进FIFO的接口
{
    Uint8PacketQueuePushStreamData(USART2_DMASendPacketQueueHandle, USART_SendBuff, DataSendLength);
    if(USART2_DMA_Sendding == false)
    {
        USART_DMA_SendPacketNode(USART2_DMASendPacketQueueHandle, USART2_DMA_SendBuff, &USART2_DMA_Sendding, USART2_TXD_DMA_Enable);
    }
}


/*对内封装，提供对外push进接收FIFO的接口
*
*/
void PushUSRAT3_DMA_SendDataIntoFIFO(uint8_t *USART_SendBuff, uint16_t DataSendLength)//对内封装，提供对外push进FIFO的接口
{
    Uint8PacketQueuePushStreamData(USART3_DMASendPacketQueueHandle, USART_SendBuff, DataSendLength);
    if(USART3_DMA_Sendding == false)
    {
        USART_DMA_SendPacketNode(USART3_DMASendPacketQueueHandle, USART3_DMA_SendBuff, &USART3_DMA_Sendding, USART3_TXD_DMA_Enable);
    }
}


/*对内封装，提供对外push进接收FIFO的接口
*
*/
void PushURAT4_DMA_SendDataIntoFIFO(uint8_t *USART_SendBuff, uint16_t DataSendLength)//对内封装，提供对外push进FIFO的接口
{
    Uint8PacketQueuePushStreamData(UART4_DMASendPacketQueueHandle, USART_SendBuff, DataSendLength);
    if(UART4_DMA_Sendding == false)
    {
        USART_DMA_SendPacketNode(UART4_DMASendPacketQueueHandle, UART4_DMA_SendBuff, &UART4_DMA_Sendding, UART4_TXD_DMA_Enable);
    }
}

void USART1_DMA_IRQHandler(void)
{
    if(DMA_GetFlagStatus(USART1_DMA_TX_FLAG) == SET)
    {
        USART1_DMA_Sendding = false;
        DMA_ClearFlag(USART1_DMA_TX_FLAG);
        if(USART1_DMASendPacketQueueHandle->head)
        {
            USART_DMA_SendPacketNode(USART1_DMASendPacketQueueHandle, USART1_DMA_SendBuff, &USART1_DMA_Sendding, USART1_TXD_DMA_Enable);
        }
    }
}

void USART2_DMA_IRQHandler(void)
{
    if(DMA_GetFlagStatus(USART2_DMA_TX_FLAG) == SET)
    {
        USART2_DMA_Sendding = false;
        DMA_ClearFlag(USART2_DMA_TX_FLAG);
        if(USART2_DMASendPacketQueueHandle->head)
        {
            USART_DMA_SendPacketNode(USART2_DMASendPacketQueueHandle, USART2_DMA_SendBuff, &USART2_DMA_Sendding, USART2_TXD_DMA_Enable);
        }
    }
}

void USART3_DMA_IRQHandler(void)
{
    if(DMA_GetFlagStatus(USART3_DMA_TX_FLAG) == SET)
    {
        USART3_DMA_Sendding = false;
        DMA_ClearFlag(USART3_DMA_TX_FLAG);
        if(USART3_DMASendPacketQueueHandle->head)
        {
            USART_DMA_SendPacketNode(USART3_DMASendPacketQueueHandle, USART3_DMA_SendBuff, &USART3_DMA_Sendding, USART3_TXD_DMA_Enable);
        }
    }
}
void UART4_DMA_IRQHandler(void)
{
    if(DMA_GetFlagStatus(UART4_DMA_TX_FLAG) == SET)
    {
        UART4_DMA_Sendding = false;
        DMA_ClearFlag(UART4_DMA_TX_FLAG);
        if(UART4_DMASendPacketQueueHandle->head)
        {
            USART_DMA_SendPacketNode(UART4_DMASendPacketQueueHandle, UART4_DMA_SendBuff, &UART4_DMA_Sendding, UART4_TXD_DMA_Enable);
        }
    }
}










