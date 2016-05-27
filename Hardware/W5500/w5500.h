#ifndef	_W5500_H_
#define	_W5500_H_
#include "stm32f10x.h"
#include "NVIC_CONFIG.H"
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "bsp_SysTick.h"
#include "SPI.h"
#include "Uint8PacketQueueManger.h"

/***************** Common Register *****************/
#define MR		0x0000
	#define RST		0x80
	#define WOL		0x20
	#define PB		0x10
	#define PPP		0x08
	#define FARP	0x02

#define GAR		0x0001
#define SUBR	0x0005
#define SHAR	0x0009
#define SIPR	0x000f

#define INTLEVEL	0x0013
#define IR		0x0015
	#define CONFLICT	0x80
	#define UNREACH		0x40
	#define PPPOE		0x20
	#define MP			0x10

#define IMR		0x0016
	#define IM_IR7		0x80
	#define IM_IR6		0x40
	#define IM_IR5		0x20
	#define IM_IR4		0x10

#define SIR		0x0017
	#define S7_INT		0x80
	#define S6_INT		0x40
	#define S5_INT		0x20
	#define S4_INT		0x10
	#define S3_INT		0x08
	#define S2_INT		0x04
	#define S1_INT		0x02
	#define S0_INT		0x01

#define SIMR	0x0018
	#define S7_IMR		0x80
	#define S6_IMR		0x40
	#define S5_IMR		0x20
	#define S4_IMR		0x10
	#define S3_IMR		0x08
	#define S2_IMR		0x04
	#define S1_IMR		0x02
	#define S0_IMR		0x01

#define RTR		0x0019
#define RCR		0x001b

#define PTIMER	0x001c
#define PMAGIC	0x001d
#define PHA		0x001e
#define PSID	0x0024
#define PMRU	0x0026

#define UIPR	0x0028
#define UPORT	0x002c

#define PHYCFGR	0x002e
	#define RST_PHY		0x80
	#define OPMODE		0x40
	#define DPX			0x04
	#define SPD			0x02
	#define LINK		0x01

#define VERR	0x0039

/********************* Socket Register *******************/
#define Sn_MR		0x0000
	#define MULTI_MFEN		0x80
	#define BCASTB			0x40
	#define	ND_MC_MMB		0x20
	#define UCASTB_MIP6B	0x10
	#define MR_CLOSE		0x00
	#define MR_TCP		0x01
	#define MR_UDP		0x02
	#define MR_MACRAW		0x04

#define Sn_CR		0x0001
	#define OPEN		0x01
	#define LISTEN		0x02
	#define CONNECT		0x04
	#define DISCON		0x08
	#define CLOSE		0x10
	#define SEND		0x20
	#define SEND_MAC	0x21
	#define SEND_KEEP	0x22
	#define RECV		0x40

#define Sn_IR		0x0002
	#define IR_SEND_OK		0x10
	#define IR_TIMEOUT		0x08
	#define IR_RECV			0x04
	#define IR_DISCON		0x02
	#define IR_CON			0x01

#define Sn_SR		0x0003
	#define SOCK_CLOSED		0x00
	#define SOCK_INIT		0x13
	#define SOCK_LISTEN		0x14
	#define SOCK_ESTABLISHED	0x17
	#define SOCK_CLOSE_WAIT		0x1c
	#define SOCK_UDP		0x22
	#define SOCK_MACRAW		0x02

	#define SOCK_SYNSEND	0x15
	#define SOCK_SYNRECV	0x16
	#define SOCK_FIN_WAI	0x18
	#define SOCK_CLOSING	0x1a
	#define SOCK_TIME_WAIT	0x1b
	#define SOCK_LAST_ACK	0x1d

#define Sn_PORT		0x0004
#define Sn_DHAR	   	0x0006
#define Sn_DIPR		0x000c
#define Sn_DPORTR	0x0010

#define Sn_MSSR		0x0012
#define Sn_TOS		0x0015
#define Sn_TTL		0x0016

#define Sn_RXBUF_SIZE	0x001e
#define Sn_TXBUF_SIZE	0x001f
#define Sn_TX_FSR	0x0020
#define Sn_TX_RD	0x0022
#define Sn_TX_WR	0x0024
#define Sn_RX_RSR	0x0026
#define Sn_RX_RD	0x0028
#define Sn_RX_WR	0x002a

#define Sn_IMR		0x002c
	#define IMR_SENDOK	0x10
	#define IMR_TIMEOUT	0x08
	#define IMR_RECV	0x04
	#define IMR_DISCON	0x02
	#define IMR_CON		0x01

#define Sn_FRAG		0x002d
#define Sn_KPALVTR	0x002f

/*******************************************************************/
/************************ SPI Control Byte *************************/
/*******************************************************************/
/* Operation mode bits */
#define VDM		0x00
#define FDM1	0x01
#define	FDM2	0x02
#define FDM4	0x03

/* Read_Write control bit */
#define RWB_READ	0x00
#define RWB_WRITE	0x04

/* Block select bits */
#define COMMON_R	0x00

/* Socket 0 */
#define S0_REG		0x08
#define S0_TX_BUF	0x10
#define S0_RX_BUF	0x18

/* Socket 1 */
#define S1_REG		0x28
#define S1_TX_BUF	0x30
#define S1_RX_BUF	0x38

/* Socket 2 */
#define S2_REG		0x48
#define S2_TX_BUF	0x50
#define S2_RX_BUF	0x58

/* Socket 3 */
#define S3_REG		0x68
#define S3_TX_BUF	0x70
#define S3_RX_BUF	0x78

/* Socket 4 */
#define S4_REG		0x88
#define S4_TX_BUF	0x90
#define S4_RX_BUF	0x98

/* Socket 5 */
#define S5_REG		0xa8
#define S5_TX_BUF	0xb0
#define S5_RX_BUF	0xb8

/* Socket 6 */
#define S6_REG		0xc8
#define S6_TX_BUF	0xd0
#define S6_RX_BUF	0xd8

/* Socket 7 */
#define S7_REG		0xe8
#define S7_TX_BUF	0xf0
#define S7_RX_BUF	0xf8

#define TRUE	0xff
#define FALSE	0x00

#define S_RX_SIZE	2048	/*定义Socket接收缓冲区的大小，可以根据W5500_RMSR的设置修改 */
#define S_TX_SIZE	2048  	/*定义Socket发送缓冲区的大小，可以根据W5500_TMSR的设置修改 */

extern void Delay_ms(uint32_t d);//延时函数(ms)

/***************----- W5500 GPIO定义 -----***************/

#define W5500_SCS_PIN		        GPIO_Pin_15	//定义W5500的CS引脚	 
#define W5500_SCS_PORT	            GPIOA
	
#define W5500_RST_PIN		        GPIO_Pin_2	//定义W5500的RST引脚
#define W5500_RST_PORT	            GPIOC

#define W5500_INT_PIN		        GPIO_Pin_3	//定义W5500的INT引脚
#define W5500_INT_PORT	            GPIOC
#define W5500_INT_SOURCE_PIN        GPIO_PinSource3
#define W5500_INT_SOURCE_PORT       GPIO_PortSourceGPIOC
#define W5500_INT_EXTI_LINE         EXTI_Line3
#define W5500_INT_EXTI_IRQn         EXTI3_IRQn
#define W5500_INT_EXTI_IRQHandler   EXTI3_IRQHandler

#define W5500_SPI_Configuration     SPI3_Init
#define W5500_SPI_Send_Byte         SPI3_Send_Byte
#define W5500_SPI_Receive_Data      SPI3_Receive_Data
#define W5500_Delay_ms              Delay_ms


/***************----- 端口的运行模式 -----***************/
//端口0的运行模式,0:TCP服务器模式,1:TCP客户端模式,2:UDP(广播)模式
#define TCP_SERVER		0x00	//TCP服务器模式
#define TCP_CLIENT		0x01	//TCP客户端模式 
#define UDP_MODE		0x02	//UDP(广播)模式 

/***************----- 端口的运行状态 -----***************/
//端口0状态记录,1:端口完成初始化,2端口完成连接(可以正常传输数据)
#define S_BREAK			0x00	//端口完成初始化 
#define S_INIT			0x01	//端口完成初始化 
#define S_CONN			0x02	//端口完成连接,可以正常传输数据 

/***************----- 端口收发数据的状态 -----***************/
//端口0接收和发送数据的状态,1:端口接收到数据,2:端口发送数据完成 
#define S_RECEIVE		0x01		//端口接收到一个数据包 
#define S_TRANSMITOK	0x02		//端口发送一个数据包完成 


typedef struct Socket
{
    uint8_t socket_index;//Socket端口号
    uint8_t local_IP[4];//本机IP地址，这里写指针是方便链接，本质是长度为4的数组
    uint16_t local_Port;//本机端口号
    uint8_t target_IP[4];//目标IP地址
    uint16_t target_Port;//目标端口号
    uint8_t socket_Mode;//端口的运行模, 00H:TCP服务器模式, 01H:TCP客户端模式, 02H:UDP(广播)模式
    uint8_t port_State;//端口状态记录, 01H:端口完成初始化, 02H端口完成连接(可以正常传输数据) 
    uint8_t send_receive_State;//端口接收和发送数据的状态, 01H:端口接收到数据, 02H:端口发送数据完成
    Uint8PacketQueue* send_Packet_Queue_Handle;//发送缓存队列
    bool packet_Sendding;//队列是否正在发送
} Socket;


void W5500_GPIO_Configuration(void);//W5500 GPIO初始化配置
void W5500_NVIC_Configuration(void);//W5500 接收引脚中断优先级设置
void W5500_SPI_Configuration(void);//W5500 SPI初始化配置(STM32 SPI1)
void W5500_Hardware_Reset(void);//硬件复位W5500
void Socket_Init(Socket* socket);//指定Socket(0~7)初始化
bool W5500_Socket_Set_Default(Socket* socket);//Socket0设置为默认状态
uint8_t Detect_Gateway(Socket* socket);//检查网关服务器

uint8_t Socket_Connect(Socket* socket);//设置指定Socket(0~7)为客户端与远程服务器连接
uint8_t Socket_Listen(Socket* socket);//设置指定Socket(0~7)作为服务器等待远程主机的连接
uint8_t Socket_UDP(Socket* socket);//设置指定Socket(0~7)为UDP模式
uint16_t Read_SOCK_Data_Buffer(Socket* socket, uint8_t *dat_ptr);//指定Socket(0~7)接收数据处理
void W5500_Send_Socket_Data(Socket* socket); //指定Socket(0~7)发送数据处理
void W5500_Socket_Receive_Process(Socket* socket);//W5500接收并处理接收到的数据

void W5500_Init(void);//初始化W5500寄存器函数
bool W5500_Daemon_Process(void);//W5500的守护进程，提供监测端口状态，端口初始化，中断处理接、收处理的调用
void W5500_Interrupt_Process(void);//W5500中断处理程序框架
void Free_W5500_PacketNoedItem(Uint8PacketNode* uint8PacketNodePointer);
void W5500_Push_Socket0_SendDataIntoFIFO(uint8_t *Socket_SendBuff, uint16_t DataSendLength);//对内封装，提供对外push进FIFO的接口
#endif


