#include <stdarg.h>
#include "SPI.h"
#include "wifi_config.h"



/**
*@brief SPI Initialization
**/
void SPI1_Initialization(void)
{
    SPI_InitTypeDef  SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_SPI1 | RCC_APB2Periph_GPIOA, ENABLE);

    /* NSS---->GPIO(LED) */
    SPI_SSOutputCmd(SPI1, ENABLE);


    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);


    /* SPI1 Config -------------------------------------------------------------*/
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//全双工通信
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;//做主机
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;//8位数据帧
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;//空闲时为低电平
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;//数据在第1个跳变沿被采集
    SPI_InitStructure.SPI_NSS = SPI_NSS_Hard;//CS脚为软件模式
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;//64分频
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;//高字节在前
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1, &SPI_InitStructure);

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    NVIC_InitStructure.NVIC_IRQChannel = SPI1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 8;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    SPI_I2S_ITConfig(SPI1,SPI_I2S_IT_TXE,DISABLE);
    SPI_I2S_ITConfig(SPI1,SPI_I2S_IT_ERR,DISABLE);
    SPI_I2S_ITConfig(SPI1,SPI_I2S_IT_RXNE,DISABLE);


    /* Enable SPI1 */
    SPI_Cmd(SPI1, ENABLE);
    SPI_I2S_ClearITPendingBit(SPI1, SPI_I2S_IT_TXE);
    SPI_I2S_ClearITPendingBit(SPI1, SPI_I2S_IT_RXNE);

}


/**
*@brief SPI Initialization
**/
void SPI2_Initialization(void)
{
    SPI_InitTypeDef  SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
    
    /* NSS---->GPIO(LED) */
    SPI_SSOutputCmd(SPI2, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15 ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);


    /* SPI2 Config -------------------------------------------------------------*/
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//做从机只接收
    SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;//做主机
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;//8位数据帧
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;//空闲时为低电平
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;//数据在第1个跳变沿被采集
    SPI_InitStructure.SPI_NSS = SPI_NSS_Hard;//CS脚为软件模式
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;//64分频
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;//高字节在前
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI2, &SPI_InitStructure);

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    NVIC_InitStructure.NVIC_IRQChannel = SPI2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    SPI_I2S_ITConfig(SPI2,SPI_I2S_IT_TXE,DISABLE);
    SPI_I2S_ITConfig(SPI2,SPI_I2S_IT_RXNE,ENABLE);


    /* Enable SPI2 */
    SPI_Cmd(SPI2, ENABLE);
    SPI_I2S_ClearITPendingBit(SPI2, SPI_I2S_IT_TXE);
    SPI_I2S_ClearITPendingBit(SPI2, SPI_I2S_IT_RXNE);
}


void SPI1SendOneByte(uint8_t byteData)
{
    int count = 0;
    int COUNT_MAX = 50;
    
     /* 判断发送缓冲是否为空 */ 
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)
    {
        count++;
        if(count>COUNT_MAX)
        {
            count = 0;
            return;
        }
    }
    /* 发送字节 */
    SPI_I2S_SendData(SPI1, byteData);  
    /* 判断接收缓冲是否为空*/
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)
    {
        count++;
        if(count>COUNT_MAX)
        {
            count = 0;
            return;
        }
    }
    /* flush data read during the write */ 
    SPI_I2S_ReceiveData(SPI1); 
}

uint8_t SPI1ReceiveOneByte(void)
{
    uint8_t receiveByte=0;
  
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);/* Loop while DR register in not emplty */
    receiveByte = (uint8_t)SPI_I2S_ReceiveData(SPI1);
    return receiveByte;
}


void SPI2SendOneByte(uint8_t byteData)
{
    SPI_I2S_ClearFlag(SPI2, SPI_I2S_FLAG_TXE);
    SPI_I2S_SendData(SPI2, byteData);    /* Send byte through the SPI peripheral */
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);    /* Loop while DR register in not emplty */
}

uint8_t SPI2ReceiveOneByte(void)
{
    uint8_t receiveByte=0;
  
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);/* Loop while DR register in not emplty */
    receiveByte = (uint8_t)SPI_I2S_ReceiveData(SPI2);
    return receiveByte;
}

/**
* @brief This function handles SPI1 global interrupt.
*/
void SPI1_IRQHandler(void)
{
    uint8_t SPI_Value=0;
    if( SPI_I2S_GetITStatus(SPI1, SPI_I2S_IT_TXE) == SET )
    {
        SPI_I2S_SendData(SPI1,0xFF);      
    }
    if( SPI_I2S_GetITStatus(SPI1, SPI_I2S_IT_RXNE) == SET )   
    {     
       SPI_Value = SPI_I2S_ReceiveData(SPI1);    
    }
    SPI_I2S_ClearITPendingBit(SPI1, SPI_I2S_IT_TXE);
    SPI_I2S_ClearITPendingBit(SPI1, SPI_I2S_IT_RXNE);
}





void SPI2_IRQHandler(void)
{   
    uint8_t SPI_Value;

   int count = 0;
    int COUNT_MAX = 50;
    
     /* 判断发送缓冲是否为空 */ 
    while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET)
    {
        count++;
        if(count>COUNT_MAX)
        {
            count = 0;
            return;
        }
    }
    /* 发送字节 */
    SPI_I2S_SendData(SPI2, 0xFF);  
    /* 判断接收缓冲是否为空*/
    while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET)
    {
        count++;
        if(count>COUNT_MAX)
        {
            count = 0;
            return;
        }
    }
    /* flush data read during the write */ 
    SPI_Value = SPI_I2S_ReceiveData(SPI2); 
    
    switch(SPI_Value)
    {
        case 0x01:
            printf("主机STM32F103VCT6开发板连接成功!\r\n");
        break;
        
        case 0x02:
            printf("我是STM32F302-NUCLEO开发板!\r\n");
        break;
        
        case 0x03:
            printf("我在休息，随时可以干活!\r\n");
        break;
        
        case 0x04:
            printf("收到指令!开始工作中...\r\n");
        break;
    }
}



