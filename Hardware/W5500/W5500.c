/**********************************************************************************
 * 文件名  ：W5500.c
 * 描述    ：W5500 驱动函数库         
 * 库版本  ：ST_v3.5
**********************************************************************************/
#include "W5500.h"	



bool W5500_Socket0_Sendding = false;
Uint8PacketQueue W5500_Socket0_SendPacketQueue = {NULL, NULL, CreatUint8PacketNode, Free_W5500_PacketNoedItem};


/***************----- 网络参数变量定义 -----***************/
uint8_t Gateway_IP[4]={192,168,169,1};//网关IP地址 
uint8_t Sub_Mask[4]={255,255,255,0};	//子网掩码 
uint8_t Phy_Addr[6]={0x0C,0x29,0xAB,0x7C,0x00,0x01};	//物理地址(MAC) 
uint8_t IP_Addr[4]={192,168,169,104};	//本机IP地址 

uint8_t S0_Port[2]={0x13,0x88};	//端口0的端口号(0x1388 5000) 
//uint8_t S0_Target_IP[4]={192,168,1,105};	//端口0目的IP地址
uint8_t S0_Target_IP[4]={115,159,203,38};	//端口0目的IP地址
uint8_t S0_Target_Port[2]={0x7B,0x17};	//端口0目的端口号(0x7B17 31511) 

uint8_t UDP_Target_IPR[4]={192,168,1,105};	//UDP(广播)模式,目的主机IP地址
uint8_t UDP_Target_Port[2]={0x7B,0x17};	//UDP(广播)模式,目的主机端口号

Socket socket_0 = {
    0,
    {192,168,169,104},
    5000,
    {115,159,203,38},
    31511,
    TCP_CLIENT,
    S_BREAK,
    S_TRANSMITOK,
    &W5500_Socket0_SendPacketQueue,
    false
};

/***************----- 端口数据缓冲区 -----***************/
uint8_t Rx_Buffer[S_RX_SIZE];	//端口接收数据缓冲区

uint8_t W5500_Interrupt;	//W5500中断标志(0:无中断,1:有中断)



/*******************************************************************************
* 函数名  : W5500_GPIO_Configuration
* 描述    : W5500 GPIO初始化配置
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : 无
*******************************************************************************/
void W5500_GPIO_Configuration(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	EXTI_InitTypeDef  EXTI_InitStructure;	

   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB
                | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD
                | RCC_APB2Periph_AFIO, ENABLE); 
	/* W5500_RST引脚初始化配置(PC5) */
	GPIO_InitStructure.GPIO_Pin  = W5500_RST_PIN;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(W5500_RST_PORT, &GPIO_InitStructure);
	GPIO_ResetBits(W5500_RST_PORT, W5500_RST_PIN);
	
	/* W5500_INT引脚初始化配置(PC4) */	
	GPIO_InitStructure.GPIO_Pin = W5500_INT_PIN;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(W5500_INT_PORT, &GPIO_InitStructure);
		
	/* Connect EXTI Line4 to PC4 */
	GPIO_EXTILineConfig(W5500_INT_SOURCE_PORT, W5500_INT_SOURCE_PIN);

	/* PC4 as W5500 interrupt input */
	EXTI_InitStructure.EXTI_Line = W5500_INT_EXTI_LINE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
}

/*******************************************************************************
* 函数名  : W5500_NVIC_Configuration
* 描述    : W5500 接收引脚中断优先级设置
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : 无
*******************************************************************************/
void W5500_NVIC_Configuration(void)
{
    NVIC_IRQChannel_Configuration_Set(W5500_INT_EXTI_IRQn,1,2,ENABLE);
}

/*******************************************************************************
* 函数名  : W5500_SPI_Send_Short
* 描述    : SPI发送2个字节数据(16位)
* 输入    : dat:待发送的16位数据
* 输出    : 无
* 返回值  : 无
* 说明    : 无
*******************************************************************************/
void W5500_SPI_Send_Short(uint16_t dat)
{
	W5500_SPI_Send_Byte(dat>>8);//写数据高位
	W5500_SPI_Send_Byte(dat);	//写数据低位
}
void W5500_SCS_High(void)
{
    GPIO_SetBits(W5500_SCS_PORT,W5500_SCS_PIN); //置W5500的SCS为高电平
}
void W5500_SCS_Low(void)
{
    GPIO_ResetBits(W5500_SCS_PORT,W5500_SCS_PIN); //置W5500的SCS为高电平
}
/*******************************************************************************
* 函数名  : Write_W5500_1Byte
* 描述    : 通过SPI向指定地址寄存器写1个字节数据
* 输入    : reg:16位寄存器地址,dat:待写入的数据
* 输出    : 无
* 返回值  : 无
* 说明    : 无
*******************************************************************************/
void Write_W5500_1Byte(uint16_t reg, uint8_t dat)
{
	W5500_SCS_Low();//置W5500的SCS为低电平

	W5500_SPI_Send_Short(reg);//通过SPI写16位寄存器地址
	W5500_SPI_Send_Byte(FDM1|RWB_WRITE|COMMON_R);//通过SPI写控制字节,1个字节数据长度,写数据,选择通用寄存器
	W5500_SPI_Send_Byte(dat);//写1个字节数据

	W5500_SCS_High(); //置W5500的SCS为高电平
}

/*******************************************************************************
* 函数名  : Write_W5500_2Byte
* 描述    : 通过SPI向指定地址寄存器写2个字节数据
* 输入    : reg:16位寄存器地址,dat:16位待写入的数据(2个字节)
* 输出    : 无
* 返回值  : 无
* 说明    : 无
*******************************************************************************/
void Write_W5500_2Byte(uint16_t reg, uint16_t dat)
{
	W5500_SCS_Low();//置W5500的SCS为低电平
		
	W5500_SPI_Send_Short(reg);//通过SPI写16位寄存器地址
	W5500_SPI_Send_Byte(FDM2|RWB_WRITE|COMMON_R);//通过SPI写控制字节,2个字节数据长度,写数据,选择通用寄存器
	W5500_SPI_Send_Short(dat);//写16位数据

	W5500_SCS_High(); //置W5500的SCS为高电平
}

/*******************************************************************************
* 函数名  : Write_W5500_nByte
* 描述    : 通过SPI向指定地址寄存器写n个字节数据
* 输入    : reg:16位寄存器地址,*dat_ptr:待写入数据缓冲区指针,size:待写入的数据长度
* 输出    : 无
* 返回值  : 无
* 说明    : 无
*******************************************************************************/
void Write_W5500_nByte(uint16_t reg, uint8_t *dat_ptr, uint16_t size)
{
	uint16_t i;

	W5500_SCS_Low();//置W5500的SCS为低电平	
		
	W5500_SPI_Send_Short(reg);//通过SPI写16位寄存器地址
	W5500_SPI_Send_Byte(VDM|RWB_WRITE|COMMON_R);//通过SPI写控制字节,N个字节数据长度,写数据,选择通用寄存器

	for(i=0;i<size;i++)//循环将缓冲区的size个字节数据写入W5500
	{
		W5500_SPI_Send_Byte(*dat_ptr++);//写一个字节数据
	}

	W5500_SCS_High(); //置W5500的SCS为高电平
}

/*******************************************************************************
* 函数名  : Write_W5500_SOCK_1Byte
* 描述    : 通过SPI向指定端口寄存器写1个字节数据
* 输入    : s:端口号,reg:16位寄存器地址,dat:待写入的数据
* 输出    : 无
* 返回值  : 无
* 说明    : 无
*******************************************************************************/
void Write_W5500_SOCK_1Byte(uint8_t socket_index, uint16_t reg, uint8_t dat)
{
	W5500_SCS_Low();//置W5500的SCS为低电平	
		
	W5500_SPI_Send_Short(reg);//通过SPI写16位寄存器地址
	W5500_SPI_Send_Byte(FDM1|RWB_WRITE|(socket_index*0x20+0x08));//通过SPI写控制字节,1个字节数据长度,写数据,选择端口s的寄存器
	W5500_SPI_Send_Byte(dat);//写1个字节数据

	W5500_SCS_High(); //置W5500的SCS为高电平
}

/*******************************************************************************
* 函数名  : Write_W5500_SOCK_2Byte
* 描述    : 通过SPI向指定端口寄存器写2个字节数据
* 输入    : s:端口号,reg:16位寄存器地址,dat:16位待写入的数据(2个字节)
* 输出    : 无
* 返回值  : 无
* 说明    : 无
*******************************************************************************/
void Write_W5500_SOCK_2Byte(uint8_t socket_index, uint16_t reg, uint16_t dat)
{
	W5500_SCS_Low();//置W5500的SCS为低电平
			
	W5500_SPI_Send_Short(reg);//通过SPI写16位寄存器地址
	W5500_SPI_Send_Byte(FDM2|RWB_WRITE|(socket_index*0x20+0x08));//通过SPI写控制字节,2个字节数据长度,写数据,选择端口s的寄存器
	W5500_SPI_Send_Short(dat);//写16位数据

	W5500_SCS_High(); //置W5500的SCS为高电平
}

/*******************************************************************************
* 函数名  : Write_W5500_SOCK_4Byte
* 描述    : 通过SPI向指定端口寄存器写4个字节数据
* 输入    : s:端口号,reg:16位寄存器地址,*dat_ptr:待写入的4个字节缓冲区指针
* 输出    : 无
* 返回值  : 无
* 说明    : 无
*******************************************************************************/
void Write_W5500_SOCK_4Byte(uint8_t socket_index, uint16_t reg, uint8_t *dat_ptr)
{
	W5500_SCS_Low();//置W5500的SCS为低电平
			
	W5500_SPI_Send_Short(reg);//通过SPI写16位寄存器地址
	W5500_SPI_Send_Byte(FDM4|RWB_WRITE|(socket_index*0x20+0x08));//通过SPI写控制字节,4个字节数据长度,写数据,选择端口s的寄存器

	W5500_SPI_Send_Byte(*dat_ptr++);//写第1个字节数据
	W5500_SPI_Send_Byte(*dat_ptr++);//写第2个字节数据
	W5500_SPI_Send_Byte(*dat_ptr++);//写第3个字节数据
	W5500_SPI_Send_Byte(*dat_ptr++);//写第4个字节数据

	W5500_SCS_High(); //置W5500的SCS为高电平
}

/*******************************************************************************
* 函数名  : Read_W5500_1Byte
* 描述    : 读W5500指定地址寄存器的1个字节数据
* 输入    : reg:16位寄存器地址
* 输出    : 无
* 返回值  : 读取到寄存器的1个字节数据
* 说明    : 无
*******************************************************************************/
uint8_t Read_W5500_1Byte(uint16_t reg)
{
	uint8_t i;

	W5500_SCS_Low();//置W5500的SCS为低电平
			
	W5500_SPI_Send_Short(reg);//通过SPI写16位寄存器地址
	W5500_SPI_Send_Byte(FDM1|RWB_READ|COMMON_R);//通过SPI写控制字节,1个字节数据长度,读数据,选择通用寄存器

	i=W5500_SPI_Receive_Data();
	W5500_SPI_Send_Byte(0x00);//发送一个哑数据
	i=W5500_SPI_Receive_Data();//读取1个字节数据

	W5500_SCS_High();//置W5500的SCS为高电平
	return i;//返回读取到的寄存器数据
}

/*******************************************************************************
* 函数名  : Read_W5500_SOCK_1Byte
* 描述    : 读W5500指定端口寄存器的1个字节数据
* 输入    : s:端口号,reg:16位寄存器地址
* 输出    : 无
* 返回值  : 读取到寄存器的1个字节数据
* 说明    : 无
*******************************************************************************/
uint8_t Read_W5500_SOCK_1Byte(uint8_t socket_index, uint16_t reg)
{
	uint8_t i;

	W5500_SCS_Low();//置W5500的SCS为低电平
			
	W5500_SPI_Send_Short(reg);//通过SPI写16位寄存器地址
	W5500_SPI_Send_Byte(FDM1|RWB_READ|(socket_index*0x20+0x08));//通过SPI写控制字节,1个字节数据长度,读数据,选择端口s的寄存器

	i=W5500_SPI_Receive_Data();
	W5500_SPI_Send_Byte(0x00);//发送一个哑数据
	i=W5500_SPI_Receive_Data();//读取1个字节数据

	W5500_SCS_High();//置W5500的SCS为高电平
	return i;//返回读取到的寄存器数据
}

/*******************************************************************************
* 函数名  : Read_W5500_SOCK_2Byte
* 描述    : 读W5500指定端口寄存器的2个字节数据
* 输入    : s:端口号,reg:16位寄存器地址
* 输出    : 无
* 返回值  : 读取到寄存器的2个字节数据(16位)
* 说明    : 无
*******************************************************************************/
uint16_t Read_W5500_SOCK_2Byte(uint8_t socket_index, uint16_t reg)
{
	uint16_t i;

	W5500_SCS_Low();//置W5500的SCS为低电平
			
	W5500_SPI_Send_Short(reg);//通过SPI写16位寄存器地址
	W5500_SPI_Send_Byte(FDM2|RWB_READ|(socket_index*0x20+0x08));//通过SPI写控制字节,2个字节数据长度,读数据,选择端口s的寄存器

	i=W5500_SPI_Receive_Data();
	W5500_SPI_Send_Byte(0x00);//发送一个哑数据
	i=W5500_SPI_Receive_Data();//读取高位数据
	W5500_SPI_Send_Byte(0x00);//发送一个哑数据
	i = i<<8;
	i+=W5500_SPI_Receive_Data();//读取低位数据

	W5500_SCS_High();//置W5500的SCS为高电平
	return i;//返回读取到的寄存器数据
}

/*******************************************************************************
* 函数名  : Read_SOCK_Data_Buffer
* 描述    : 从W5500接收数据缓冲区中读取数据
* 输入    : s:端口号,*dat_ptr:数据保存缓冲区指针
* 输出    : 无
* 返回值  : 读取到的数据长度,rx_size个字节
* 说明    : 无
*******************************************************************************/
uint16_t Read_SOCK_Data_Buffer(Socket* socket, uint8_t *dat_ptr)
{
	uint16_t rx_size;
	uint16_t offset, offset1;
	uint16_t i;
	uint8_t j;

	rx_size=Read_W5500_SOCK_2Byte(socket->socket_index,Sn_RX_RSR);
	if(rx_size==0) return 0;//没接收到数据则返回
	if(rx_size>1460) rx_size=1460;

	offset=Read_W5500_SOCK_2Byte(socket->socket_index,Sn_RX_RD);
	offset1=offset;
	offset&=(S_RX_SIZE-1);//计算实际的物理地址

	W5500_SCS_Low();//置W5500的SCS为低电平

	W5500_SPI_Send_Short(offset);//写16位地址
	W5500_SPI_Send_Byte(VDM|RWB_READ|(socket->socket_index*0x20+0x18));//写控制字节,N个字节数据长度,读数据,选择端口s的寄存器
	j=W5500_SPI_Receive_Data();
	
	if((offset+rx_size)<S_RX_SIZE)//如果最大地址未超过W5500接收缓冲区寄存器的最大地址
	{
		for(i=0;i<rx_size;i++)//循环读取rx_size个字节数据
		{
			W5500_SPI_Send_Byte(0x00);//发送一个哑数据
			j=W5500_SPI_Receive_Data();//读取1个字节数据
			*dat_ptr=j;//将读取到的数据保存到数据保存缓冲区
			dat_ptr++;//数据保存缓冲区指针地址自增1
		}
	}
	else//如果最大地址超过W5500接收缓冲区寄存器的最大地址
	{
		offset=S_RX_SIZE-offset;
		for(i=0;i<offset;i++)//循环读取出前offset个字节数据
		{
			W5500_SPI_Send_Byte(0x00);//发送一个哑数据
			j=W5500_SPI_Receive_Data();//读取1个字节数据
			*dat_ptr=j;//将读取到的数据保存到数据保存缓冲区
			dat_ptr++;//数据保存缓冲区指针地址自增1
		}
		W5500_SCS_High(); //置W5500的SCS为高电平

		W5500_SCS_Low();//置W5500的SCS为低电平

		W5500_SPI_Send_Short(0x00);//写16位地址
		W5500_SPI_Send_Byte(VDM|RWB_READ|(socket->socket_index*0x20+0x18));//写控制字节,N个字节数据长度,读数据,选择端口s的寄存器
		j=W5500_SPI_Receive_Data();

		for(;i<rx_size;i++)//循环读取后rx_size-offset个字节数据
		{
			W5500_SPI_Send_Byte(0x00);//发送一个哑数据
			j=W5500_SPI_Receive_Data();//读取1个字节数据
			*dat_ptr=j;//将读取到的数据保存到数据保存缓冲区
			dat_ptr++;//数据保存缓冲区指针地址自增1
		}
	}
	W5500_SCS_High(); //置W5500的SCS为高电平

	offset1+=rx_size;//更新实际物理地址,即下次读取接收到的数据的起始地址
	Write_W5500_SOCK_2Byte(socket->socket_index, Sn_RX_RD, offset1);
	Write_W5500_SOCK_1Byte(socket->socket_index, Sn_CR, RECV);//发送启动接收命令
	return rx_size;//返回接收到数据的长度
}

/*******************************************************************************
* 函数名  : W5500_Send_Socket_Data
* 描述    : 将数据写入W5500的数据发送缓冲区
* 输入    : s:端口号,*dat_ptr:数据保存缓冲区指针,size:待写入数据的长度
* 输出    : 无
* 返回值  : 无
* 说明    : 无
*******************************************************************************/
void W5500_Send_Socket_Data(Socket* socket)
{
    if(socket->send_Packet_Queue_Handle->head)
    {
        uint16_t offset,offset1;
        uint16_t i;
        uint8_t* packet;
        uint16_t packetLength;
        Uint8PacketNode* uint8PacketNodePointer;
        uint8PacketNodePointer = Uint8PacketQueuePop(socket->send_Packet_Queue_Handle);
        packet = uint8PacketNodePointer->packet;
        packetLength = uint8PacketNodePointer->packetLength;
        socket->packet_Sendding = true;
        //如果是UDP模式,可以在此设置目的主机的IP和端口号
        if((Read_W5500_SOCK_1Byte(socket->socket_index,Sn_MR)&0x0f) != SOCK_UDP)//如果Socket打开失败
        {		
            Write_W5500_SOCK_4Byte(socket->socket_index, Sn_DIPR, UDP_Target_IPR);//设置目的主机IP  		
            Write_W5500_SOCK_2Byte(socket->socket_index, Sn_DPORTR, ((uint16_t)UDP_Target_Port[0]<<8)+UDP_Target_Port[1]);//设置目的主机端口号				
        }

        offset=Read_W5500_SOCK_2Byte(socket->socket_index,Sn_TX_WR);
        offset1=offset;
        offset&=(S_TX_SIZE-1);//计算实际的物理地址

        W5500_SCS_Low();//置W5500的SCS为低电平

        W5500_SPI_Send_Short(offset);//写16位地址
        W5500_SPI_Send_Byte(VDM|RWB_WRITE|(socket->socket_index*0x20+0x10));//写控制字节,N个字节数据长度,写数据,选择端口s的寄存器

        if((offset+packetLength)<S_TX_SIZE)//如果最大地址未超过W5500发送缓冲区寄存器的最大地址
        {
            for(i=0;i<packetLength;i++)//循环写入size个字节数据
            {
                W5500_SPI_Send_Byte(*packet++);//写入一个字节的数据		
            }
        }
        else//如果最大地址超过W5500发送缓冲区寄存器的最大地址
        {
            offset=S_TX_SIZE-offset;
            for(i=0;i<offset;i++)//循环写入前offset个字节数据
            {
                W5500_SPI_Send_Byte(*packet++);//写入一个字节的数据
            }
            W5500_SCS_High(); //置W5500的SCS为高电平

            W5500_SCS_Low();//置W5500的SCS为低电平

            W5500_SPI_Send_Short(0x00);//写16位地址
            W5500_SPI_Send_Byte(VDM|RWB_WRITE|(socket->socket_index*0x20+0x10));//写控制字节,N个字节数据长度,写数据,选择端口s的寄存器

            for(;i<packetLength;i++)//循环写入size-offset个字节数据
            {
                W5500_SPI_Send_Byte(*packet++);//写入一个字节的数据
            }
        }
        W5500_SCS_High(); //置W5500的SCS为高电平

        offset1+=packetLength;//更新实际物理地址,即下次写待发送数据到发送数据缓冲区的起始地址
        Write_W5500_SOCK_2Byte(socket->socket_index, Sn_TX_WR, offset1);
        Write_W5500_SOCK_1Byte(socket->socket_index, Sn_CR, SEND);//发送启动发送命令
        socket->send_Packet_Queue_Handle->FreePacketNoedItem(uint8PacketNodePointer);
    }
    else
    {
        socket->packet_Sendding = false;
    }
}


/*******************************************************************************
* 函数名  : W5500_Hardware_Reset
* 描述    : 硬件复位W5500
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : W5500的复位引脚保持低电平至少500us以上,才能重启W5500
*******************************************************************************/
void W5500_Hardware_Reset(void)
{
	GPIO_ResetBits(W5500_RST_PORT, W5500_RST_PIN);//复位引脚拉低
	W5500_Delay_ms(50);
	GPIO_SetBits(W5500_RST_PORT, W5500_RST_PIN);//复位引脚拉高
	W5500_Delay_ms(200);
	while((Read_W5500_1Byte(PHYCFGR)&LINK)==0);//等待以太网连接完成
}

/*******************************************************************************
* 函数名  : W5500_Init
* 描述    : 初始化W5500寄存器函数
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : 在使用W5500之前，先对W5500初始化
*******************************************************************************/
void W5500_Init(void)
{
    uint8_t socket_index;
    Socket* socket = &socket_0;
    printf("W5500 reset.\r\n");
	W5500_GPIO_Configuration();	//W5500 GPIO初始化配置
    W5500_NVIC_Configuration(); //W5500 中断配置
    W5500_SPI_Configuration();  //W5500 SPI初始化配置
    W5500_Hardware_Reset();		//硬件复位W5500
    printf("W5500 Ethernet link\r\n");
	Write_W5500_1Byte(MR, RST);//软件复位W5500,置1有效,复位后自动清0
	W5500_Delay_ms(10);//延时10ms,自己定义该函数
	//设置网关(Gateway)的IP地址,Gateway_IP为4字节uint8_t数组,自己定义 
	//使用网关可以使通信突破子网的局限，通过网关可以访问到其它子网或进入Internet
	Write_W5500_nByte(GAR, Gateway_IP, 4);
	//设置子网掩码(MASK)值,SUB_MASK为4字节uint8_t数组,自己定义
	//子网掩码用于子网运算
	Write_W5500_nByte(SUBR,Sub_Mask,4);		
	
	//设置物理地址,PHY_ADDR为6字节uint8_t数组,自己定义,用于唯一标识网络设备的物理地址值
	//该地址值需要到IEEE申请，按照OUI的规定，前3个字节为厂商代码，后三个字节为产品序号
	//如果自己定义物理地址，注意第一个字节必须为偶数
	Write_W5500_nByte(SHAR,Phy_Addr,6);		

	//设置本机的IP地址,IP_ADDR为4字节uint8_t数组,自己定义
	//注意，网关IP必须与本机IP属于同一个子网，否则本机将无法找到网关
	Write_W5500_nByte(SIPR,IP_Addr,4);		
	
	//设置发送缓冲区和接收缓冲区的大小，参考W5500数据手册
	for(socket_index=0;socket_index<8;socket_index++)
	{
		Write_W5500_SOCK_1Byte(socket_index,Sn_RXBUF_SIZE, 0x02);//Socket Rx memory size=2k
		Write_W5500_SOCK_1Byte(socket_index,Sn_TXBUF_SIZE, 0x02);//Socket Tx mempry size=2k
	}

	//设置重试时间，默认为2000(200ms) 
	//每一单位数值为100微秒,初始化时值设为2000(0x07D0),等于200毫秒
	Write_W5500_2Byte(RTR, 0x07d0);

	//设置重试次数，默认为8次 
	//如果重发的次数超过设定值,则产生超时中断(相关的端口中断寄存器中的Sn_IR 超时位(TIMEOUT)置“1”)
	Write_W5500_1Byte(RCR,8);

	//启动中断，参考W5500数据手册确定自己需要的中断类型
	//IMR_CONFLICT是IP地址冲突异常中断,IMR_UNREACH是UDP通信时，地址无法到达的异常中断
	//其它是Socket事件中断，根据需要添加
    printf("W5500 IRQn init.\r\n");
	Write_W5500_1Byte(IMR,IM_IR7 | IM_IR6);
	Write_W5500_1Byte(SIMR,S0_IMR);
	Write_W5500_SOCK_1Byte(socket->socket_index, Sn_IMR, IMR_SENDOK | IMR_TIMEOUT | IMR_RECV | IMR_DISCON | IMR_CON);
    printf("W5500 detect Gateway.\r\n");
    Detect_Gateway(socket);	//检查网关服务器
    printf("W5500 socket init.\r\n");
	Socket_Init(socket);		//指定Socket(0~7)初始化,初始化端口0
    printf("W5500 Inited.\r\n");
    if(socket->socket_Mode==TCP_CLIENT)//TCP客户端模式 
    {
        while(Socket_Connect(socket)!=TRUE)W5500_Delay_ms(5);;
        printf("TCP socket connected.\r\n");
    }
}

/*******************************************************************************
* 函数名  : Detect_Gateway
* 描述    : 检查网关服务器
* 输入    : 无
* 输出    : 无
* 返回值  : 成功返回TRUE(0xFF),失败返回FALSE(0x00)
* 说明    : 无
*******************************************************************************/
uint8_t Detect_Gateway(Socket* socket)
{
	uint8_t ip_adde[4];
	ip_adde[0]=socket->local_IP[0]+1;
	ip_adde[1]=socket->local_IP[1]+1;
	ip_adde[2]=socket->local_IP[2]+1;
	ip_adde[3]=socket->local_IP[3]+1;

	//检查网关及获取网关的物理地址
	Write_W5500_SOCK_4Byte(socket->socket_index,Sn_DIPR,ip_adde);//向目的地址寄存器写入与本机IP不同的IP值
	Write_W5500_SOCK_1Byte(socket->socket_index,Sn_MR,MR_TCP);//设置socket为TCP模式
	Write_W5500_SOCK_1Byte(socket->socket_index,Sn_CR,OPEN);//打开Socket	
	W5500_Delay_ms(5);//延时5ms 	
	
	if(Read_W5500_SOCK_1Byte(socket->socket_index,Sn_SR) != SOCK_INIT)//如果socket打开失败
	{
		Write_W5500_SOCK_1Byte(socket->socket_index,Sn_CR,CLOSE);//打开不成功,关闭Socket
		return FALSE;//返回FALSE(0x00)
	}

	Write_W5500_SOCK_1Byte(socket->socket_index,Sn_CR,CONNECT);//设置Socket为Connect模式						

	do
	{
		u8 j=0;
		j=Read_W5500_SOCK_1Byte(socket->socket_index,Sn_IR);//读取Socket0中断标志寄存器
		if(j!=0)
		Write_W5500_SOCK_1Byte(socket->socket_index,Sn_IR,j);
		W5500_Delay_ms(5);//延时5ms 
		if((j&IR_TIMEOUT) == IR_TIMEOUT)
		{
			return FALSE;	
		}
		else if(Read_W5500_SOCK_1Byte(socket->socket_index,Sn_DHAR) != 0xff)
		{
			Write_W5500_SOCK_1Byte(socket->socket_index,Sn_CR,CLOSE);//关闭Socket
			return TRUE;							
		}
	}while(1);
}

/*******************************************************************************
* 函数名  : Socket_Init
* 描述    : 指定Socket(0~7)初始化
* 输入    : s:待初始化的端口
* 输出    : 无
* 返回值  : 无
* 说明    : 无
*******************************************************************************/
void Socket_Init(Socket* socket)
{
	//设置分片长度，参考W5500数据手册，该值可以不修改	
	Write_W5500_SOCK_2Byte(socket->socket_index, Sn_MSSR, 1460);//最大分片字节数=1460(0x5b4)
	//设置指定端口
	switch(socket->socket_index)
	{
		case 0:
			//设置端口0的端口号
            socket->local_Port = ((uint16_t)rand()%65535);
            printf("W5500 Port: %d\r\n",socket->local_Port);
			Write_W5500_SOCK_2Byte(socket->socket_index, Sn_PORT, socket->local_Port);
			//设置端口0目的(远程)端口号
			Write_W5500_SOCK_2Byte(socket->socket_index, Sn_DPORTR,socket->target_Port);
			//设置端口0目的(远程)IP地址
			Write_W5500_SOCK_4Byte(socket->socket_index, Sn_DIPR, socket->target_IP);			
			
			break;

		case 1:
			break;

		case 2:
			break;

		case 3:
			break;

		case 4:
			break;

		case 5:
			break;

		case 6:
			break;

		case 7:
			break;

		default:
			break;
	}
}

/*******************************************************************************
* 函数名  : W5500_Socket_Set
* 描述    : W5500端口初始化配置
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : 分别设置4个端口,根据端口工作模式,将端口置于TCP服务器、TCP客户端或UDP模式.
*			从端口状态字节Socket_State可以判断端口的工作情况
*******************************************************************************/
bool W5500_Socket_Set_Default(Socket* socket)
{
	if(socket->port_State==0)//端口0初始化配置
	{
		if(socket->socket_Mode==TCP_SERVER)//TCP服务器模式 
		{
			if(Socket_Listen(socket)==TRUE)
            {
				socket->port_State=S_INIT;
                return true;
            }
		}
		else if(socket->socket_Mode==TCP_CLIENT)//TCP客户端模式 
		{
			if(Socket_Connect(socket)==TRUE)
            {
				socket->port_State=S_INIT;
                return true;
            }
		}
		else//UDP模式 
		{
			if(Socket_UDP(socket)==TRUE)
            {
				socket->port_State=S_INIT|S_CONN;
                return true;
            }
		}
        socket->port_State=0;
        return false;
	}
    return true;
}

/*******************************************************************************
* 函数名  : Socket_Connect
* 描述    : 设置指定Socket(0~7)为客户端与远程服务器连接
* 输入    : s:待设定的端口
* 输出    : 无
* 返回值  : 成功返回TRUE(0xFF),失败返回FALSE(0x00)
* 说明    : 当本机Socket工作在客户端模式时,引用该程序,与远程服务器建立连接
*			如果启动连接后出现超时中断，则与服务器连接失败,需要重新调用该程序连接
*			该程序每调用一次,就与服务器产生一次连接
*******************************************************************************/
uint8_t Socket_Connect(Socket* socket)
{
	Write_W5500_SOCK_1Byte(socket->socket_index,Sn_MR,MR_TCP);//设置socket为TCP模式
    Write_W5500_SOCK_1Byte(socket->socket_index,Sn_CR,CLOSE);//打开不成功,关闭Socket
	Write_W5500_SOCK_1Byte(socket->socket_index,Sn_CR,OPEN);//打开Socket
	W5500_Delay_ms(5);//延时10ms
	if(Read_W5500_SOCK_1Byte(socket->socket_index,Sn_SR)!=SOCK_INIT)//如果socket打开失败
	{
		Write_W5500_SOCK_1Byte(socket->socket_index,Sn_CR,CLOSE);//打开不成功,关闭Socket
		return FALSE;//返回FALSE(0x00)
	}
	Write_W5500_SOCK_1Byte(socket->socket_index,Sn_CR,CONNECT);//设置Socket为Connect模式
    socket->port_State=S_INIT;
	return TRUE;//返回TRUE,设置成功
}

/*******************************************************************************
* 函数名  : Socket_Listen
* 描述    : 设置指定Socket(0~7)作为服务器等待远程主机的连接
* 输入    : s:待设定的端口
* 输出    : 无
* 返回值  : 成功返回TRUE(0xFF),失败返回FALSE(0x00)
* 说明    : 当本机Socket工作在服务器模式时,引用该程序,等等远程主机的连接
*			该程序只调用一次,就使W5500设置为服务器模式
*******************************************************************************/
uint8_t Socket_Listen(Socket* socket)
{
	Write_W5500_SOCK_1Byte(socket->socket_index,Sn_MR,MR_TCP);//设置socket为TCP模式 
	Write_W5500_SOCK_1Byte(socket->socket_index,Sn_CR,OPEN);//打开Socket	
	W5500_Delay_ms(5);//延时5ms
	if(Read_W5500_SOCK_1Byte(socket->socket_index,Sn_SR)!=SOCK_INIT)//如果socket打开失败
	{
		Write_W5500_SOCK_1Byte(socket->socket_index,Sn_CR,CLOSE);//打开不成功,关闭Socket
		return FALSE;//返回FALSE(0x00)
	}	
	Write_W5500_SOCK_1Byte(socket->socket_index,Sn_CR,LISTEN);//设置Socket为侦听模式	
	W5500_Delay_ms(5);//延时5ms
	if(Read_W5500_SOCK_1Byte(socket->socket_index,Sn_SR)!=SOCK_LISTEN)//如果socket设置失败
	{
		Write_W5500_SOCK_1Byte(socket->socket_index,Sn_CR,CLOSE);//设置不成功,关闭Socket
		return FALSE;//返回FALSE(0x00)
	}

	return TRUE;

	//至此完成了Socket的打开和设置侦听工作,至于远程客户端是否与它建立连接,则需要等待Socket中断，
	//以判断Socket的连接是否成功。参考W5500数据手册的Socket中断状态
	//在服务器侦听模式不需要设置目的IP和目的端口号
}

/*******************************************************************************
* 函数名  : Socket_UDP
* 描述    : 设置指定Socket(0~7)为UDP模式
* 输入    : s:待设定的端口
* 输出    : 无
* 返回值  : 成功返回TRUE(0xFF),失败返回FALSE(0x00)
* 说明    : 如果Socket工作在UDP模式,引用该程序,在UDP模式下,Socket通信不需要建立连接
*			该程序只调用一次，就使W5500设置为UDP模式
*******************************************************************************/
uint8_t Socket_UDP(Socket* socket)
{
	Write_W5500_SOCK_1Byte(socket->socket_index,Sn_MR,MR_UDP);//设置Socket为UDP模式*/
	Write_W5500_SOCK_1Byte(socket->socket_index,Sn_CR,OPEN);//打开Socket*/
	W5500_Delay_ms(5);//延时5ms
	if(Read_W5500_SOCK_1Byte(socket->socket_index,Sn_SR)!=SOCK_UDP)//如果Socket打开失败
	{
		Write_W5500_SOCK_1Byte(socket->socket_index,Sn_CR,CLOSE);//打开不成功,关闭Socket
		return FALSE;//返回FALSE(0x00)
	}
	else
		return TRUE;

	//至此完成了Socket的打开和UDP模式设置,在这种模式下它不需要与远程主机建立连接
	//因为Socket不需要建立连接,所以在发送数据前都可以设置目的主机IP和目的Socket的端口号
	//如果目的主机IP和目的Socket的端口号是固定的,在运行过程中没有改变,那么也可以在这里设置
}

/*******************************************************************************
* 函数名  : W5500_INT_EXTI_IRQHandler
* 描述    : W5500中断线中断服务函数(W5500 INT引脚中断服务函数)
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : 无
*******************************************************************************/
void W5500_INT_EXTI_IRQHandler(void)
{
	if(EXTI_GetITStatus(W5500_INT_EXTI_LINE) != RESET)
	{
		EXTI_ClearITPendingBit(W5500_INT_EXTI_LINE);
		W5500_Interrupt=1;
	}
}

/*对内封装，提供对外push进接收FIFO的接口
*
*/
void W5500_Push_Socket0_SendDataIntoFIFO(uint8_t *Socket_SendBuff, uint16_t DataSendLength)//对内封装，提供对外push进FIFO的接口
{
    Socket* socket = &socket_0;
    Uint8PacketQueuePushStreamData(socket->send_Packet_Queue_Handle, Socket_SendBuff, DataSendLength);
    if(socket->packet_Sendding == false)
    {
        W5500_Send_Socket_Data(socket);
    }
}

void Free_W5500_PacketNoedItem(Uint8PacketNode* uint8PacketNodePointer)
{
    if(!uint8PacketNodePointer)return;
    if(uint8PacketNodePointer->packetBlock)
    {
        free(uint8PacketNodePointer->packetBlock);
        uint8PacketNodePointer->packetBlock = NULL;
    }
    free(uint8PacketNodePointer);
}


/*******************************************************************************
* 函数名  : W5500_Interrupt_Process
* 描述    : W5500中断处理程序框架
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : 无
*******************************************************************************/
void W5500_Interrupt_Process(void)
{
	uint8_t IRQ_sign;
    Socket* socket;
    while(1)
    {
        W5500_Interrupt=0;//清零中断标志
        IRQ_sign = Read_W5500_1Byte(IR);//读取中断标志寄存器
        Write_W5500_1Byte(IR, (IRQ_sign&0xf0));//回写清除中断标志

        if((IRQ_sign & CONFLICT) == CONFLICT)//IP地址冲突异常处理
        {
             //自己添加代码
        }

        if((IRQ_sign & UNREACH) == UNREACH)//UDP模式下地址无法到达异常处理
        {
            //自己添加代码
        }

        IRQ_sign=Read_W5500_1Byte(SIR);//读取端口中断标志寄存器	
        if((IRQ_sign & S0_INT) == S0_INT)//Socket0事件处理 
        {
            socket = &socket_0;
            IRQ_sign=Read_W5500_SOCK_1Byte(socket->socket_index,Sn_IR);//读取Socket0中断标志寄存器
            Write_W5500_SOCK_1Byte(socket->socket_index,Sn_IR,IRQ_sign);
            if(IRQ_sign&IR_CON)//在TCP模式下,Socket0成功连接 
            {
                socket->port_State|=S_CONN;//网络连接状态0x02,端口完成连接，可以正常传输数据
            }
            if(IRQ_sign&IR_DISCON)//在TCP模式下Socket断开连接处理
            {
                Write_W5500_SOCK_1Byte(socket->socket_index,Sn_CR,CLOSE);//关闭端口,等待重新打开连接 
                Socket_Init(socket);		//指定Socket(0~7)初始化,初始化端口0
                socket->port_State=S_BREAK;//网络连接状态0x00,端口连接失败
            }
            if(IRQ_sign&IR_SEND_OK)//Socket0数据发送完成,可以再次启动S_tx_process()函数发送数据 
            {
                socket->send_receive_State|=S_TRANSMITOK;//端口发送一个数据包完成
            }
            if(IRQ_sign&IR_RECV)//Socket接收到数据,可以启动S_rx_process()函数 
            {
                socket->send_receive_State|=S_RECEIVE;//端口接收到一个数据包
            }
            if(IRQ_sign&IR_TIMEOUT)//Socket连接或数据传输超时处理 
            {
                Write_W5500_SOCK_1Byte(socket->socket_index,Sn_CR,CLOSE);// 关闭端口,等待重新打开连接 
                socket->port_State=S_BREAK;//网络连接状态0x00,端口连接失败
            }
        }
        if(Read_W5500_1Byte(SIR) == 0) 
            break;
    }
}

bool W5500_Daemon_Process(void)
{
    Socket* socket = &socket_0;
    if(W5500_Socket_Set_Default(socket) == false)//W5500端口初始化配置
    {
        return false;
    }
    if(W5500_Interrupt)//处理W5500中断		
    {
        W5500_Interrupt_Process();//W5500中断处理程序框架
    }
    if(socket->port_State == S_BREAK)
    {
        if(socket->socket_Mode==TCP_CLIENT)//TCP客户端模式 
        {
            while(Socket_Connect(socket)!=TRUE)W5500_Delay_ms(10);
        }
    }
    if((socket->send_receive_State & S_RECEIVE) == S_RECEIVE)//如果Socket0接收到数据
    {
        socket->send_receive_State&=~S_RECEIVE;
        W5500_Socket_Receive_Process(socket);//W5500接收并发送接收到的数据
    }
    if(socket->port_State == (S_INIT|S_CONN))
    {
        if((socket->send_receive_State & S_TRANSMITOK) == S_TRANSMITOK)
        {
            socket->send_receive_State&=~S_TRANSMITOK;
            if(socket->send_Packet_Queue_Handle->head)
            {
                W5500_Send_Socket_Data(socket);
            }
            else
            {
                socket->packet_Sendding = false;
            }
        }
    }
 
    return true;
}

/*******************************************************************************
* 函数名  : W5500_Socket_Receive_Process
* 描述    : W5500接收并处理接收到的数据
* 输入    : s:端口号
* 输出    : 无
* 返回值  : 无
* 说明    : 本过程先调用S_rx_process()从W5500的端口接收数据缓冲区读取数据,
*			然后将读取的数据从Rx_Buffer拷贝到Temp_Buffer缓冲区进行处理。
*			处理完毕，将数据从Temp_Buffer拷贝到Tx_Buffer缓冲区。调用S_tx_process()
*			发送数据。
*******************************************************************************/
void W5500_Socket_Receive_Process(Socket* socket)
{
	uint16_t size;
    uint8_t* Tx_Buffer;
	size=Read_SOCK_Data_Buffer(socket, Rx_Buffer);
    Tx_Buffer = (uint8_t*)malloc(sizeof(uint8_t)*size);
	memcpy(Tx_Buffer, Rx_Buffer, size);
    Uint8PacketQueuePushStreamData(socket->send_Packet_Queue_Handle, Tx_Buffer, size);
	W5500_Send_Socket_Data(socket);
    free(Tx_Buffer);
}

