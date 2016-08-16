#include <stdarg.h>
#include "SPI.h"
#include "ESP8266_config.h"


/**
*@brief SPI Initialization
**/
void SPI1_Init(void)
{
    SPI_InitTypeDef  SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    
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
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;//空闲时为低电平
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;//数据在第1个跳变沿被采集
    SPI_InitStructure.SPI_NSS = SPI_NSS_Hard;//CS脚为软件模式
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;//64分频
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;//高字节在前
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1, &SPI_InitStructure);


    NVIC_IRQChannel_Configuration_Set(SPI1_IRQn, 3, 1, ENABLE);
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
void SPI2_Init(void)
{
    SPI_InitTypeDef  SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_15 ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    SPI_SSOutputCmd(SPI2, ENABLE);
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//全双工通信
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;//做主机
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;//8位数据帧
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;//空闲时为低电平
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;//数据在第1个跳变沿被采集
    SPI_InitStructure.SPI_NSS = SPI_NSS_Hard;//CS脚为软件模式
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;//16分频
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;//高字节在前
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI2, &SPI_InitStructure);

//    NVIC_IRQChannel_Configuration_Set(SPI2_IRQn, 2, 0, ENABLE);
//    SPI_I2S_ITConfig(SPI2,SPI_I2S_IT_TXE,DISABLE);
//    SPI_I2S_ITConfig(SPI2,SPI_I2S_IT_RXNE,ENABLE);


    /* Enable SPI2 */
    SPI_Cmd(SPI2, ENABLE);
    SPI_I2S_ClearITPendingBit(SPI2, SPI_I2S_IT_TXE);
    SPI_I2S_ClearITPendingBit(SPI2, SPI_I2S_IT_RXNE);
}


/*******************************************************************************
* Function Name  : SPI_FLASH_Init
* Description    : Initializes the peripherals used by the SPI FLASH driver.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void SPI3_Init(void)
{
	GPIO_InitTypeDef 	GPIO_InitStructure;
	SPI_InitTypeDef   	SPI_InitStructure;	   

  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);	
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);
    
	/* 初始化SCK、MISO、MOSI引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB,GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5);

	/* 初始化CS引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_15);

	/* 初始化配置STM32 SPI */
	SPI_InitStructure.SPI_Direction=SPI_Direction_2Lines_FullDuplex;	//SPI设置为双线双向全双工
	SPI_InitStructure.SPI_Mode=SPI_Mode_Master;							//设置为主SPI
	SPI_InitStructure.SPI_DataSize=SPI_DataSize_8b;						//SPI发送接收8位帧结构
	SPI_InitStructure.SPI_CPOL=SPI_CPOL_Low;							//时钟悬空低
	SPI_InitStructure.SPI_CPHA=SPI_CPHA_1Edge;							//数据捕获于第1个时钟沿
	SPI_InitStructure.SPI_NSS=SPI_NSS_Soft;								//NSS由外部管脚管理
	SPI_InitStructure.SPI_BaudRatePrescaler=SPI_BaudRatePrescaler_2;	//波特率预分频值为2
	SPI_InitStructure.SPI_FirstBit=SPI_FirstBit_MSB;					//数据传输从MSB位开始
	SPI_InitStructure.SPI_CRCPolynomial=7;								//CRC多项式为7
	SPI_Init(SPI3,&SPI_InitStructure);									//根据SPI_InitStruct中指定的参数初始化外设SPI寄存器

	SPI_Cmd(SPI3,ENABLE);	//STM32使能SPI
}

void SPI3_Send_Byte(uint8_t dat)
{
	SPI_I2S_SendData(SPI3,dat);//写1个字节数据
	while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == RESET);//等待数据寄存器空
}

uint16_t SPI3_Receive_Data()
{
    return SPI_I2S_ReceiveData(SPI3);
}




uint8_t SPI1_WriteReadOneByte(uint8_t byteData)
{
    int count = 0;
    int COUNT_MAX = 500;
    
     /* 判断发送缓冲是否为空 */ 
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)
    {
        count++;
        if(count>COUNT_MAX)
        {
            count = 0;
            return 0;
        }
    }
    /* 发送字节 */
    SPI_I2S_SendData(SPI1, byteData);  
    /* 判断接收缓冲是否为空*/
    count = 0;
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)
    {
        count++;
        if(count>COUNT_MAX)
        {
            count = 0;
            return 0;
        }
    }
    /* flush data read during the write */ 
    return SPI_I2S_ReceiveData(SPI1); 
}


uint8_t SPI2_WriteReadOneByte(uint8_t byteData)
{
    int count = 0;
    int COUNT_MAX = 500;
    
     /* 判断发送缓冲是否为空 */ 
    while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET)
    {
        count++;
        if(count>COUNT_MAX)
        {
            count = 0;
            return 0;
        }
    }
    /* 发送字节 */
    SPI_I2S_SendData(SPI2, byteData);  
    /* 判断接收缓冲是否为空*/
    count = 0;
    while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET)
    {
        count++;
        if(count>COUNT_MAX)
        {
            count = 0;
            return 0;
        }
    }
    /* flush data read during the write */ 
    return SPI_I2S_ReceiveData(SPI2); 
}

uint8_t SPI3_WriteReadOneByte(uint8_t byteData)
{
    int count = 0;
    int COUNT_MAX = 500;
    
     /* 判断发送缓冲是否为空 */ 
    while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == RESET)
    {
        count++;
        if(count>COUNT_MAX)
        {
            count = 0;
            return 0;
        }
    }
    /* 发送字节 */
    SPI_I2S_SendData(SPI3, byteData);
    /* 判断接收缓冲是否为空*/
    count = 0;
    while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_RXNE) == RESET)
    {
        count++;
        if(count>COUNT_MAX)
        {
            count = 0;
            return 0;
        }
    }
    /* flush data read during the write */ 
    return SPI_I2S_ReceiveData(SPI3); 
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
    uint8_t SPI_Value=0;
    if( SPI_I2S_GetITStatus(SPI2, SPI_I2S_IT_TXE) == SET )
    {
        SPI_I2S_SendData(SPI2,0xFF);      
    }
    if( SPI_I2S_GetITStatus(SPI2, SPI_I2S_IT_RXNE) == SET )   
    {     
       SPI_Value = SPI_I2S_ReceiveData(SPI2);    
    }
    SPI_I2S_ClearITPendingBit(SPI2, SPI_I2S_IT_TXE);
    SPI_I2S_ClearITPendingBit(SPI2, SPI_I2S_IT_RXNE);
}



