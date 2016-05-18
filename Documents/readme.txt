STM32 移植使用Contiki系统

1) 不仅是ESP8266驱动，整合了wifi模块、OLED显示器、烟雾传感器、关照传感器、温湿度传感器、超声波传感器，红外热释电传感器。
2) 目录结构：
	Documents  ---  说明文档
	Hardware   ---  外围连接设备
	Libraries  ---  STM32库依赖项
	Listing    ---  编译链接生成文件
	Output     ---  编译输出文件
	Project    ---  工程项目文件
	User       ---  单片机内部及板级设备
3) 现将各模块驱动全部调通并能将所有数据打印显示。
4) 现系统中各任务均为单元测试例程，并未做数据处理及控制。
5) 使用串口1作为打印调节输出端口。
6) 终端和节点不同的地方：
		a.) CommunicationConfig.h 中 #define __TERMINAL_ON__ 位置的宏定义 __TERMINAL_ON__ __TERMINAL_OFF__
		b.) bsp_usart3.C 中 USART3_IRQHandler 中是否开启接收无线串口的数据;
		c.) ProcessTask.C CommunicatProtocol_Send_Sensor_Data中传感器类型与应用层json的地址;
		d.) ProcessTask.C CommunicatProtocol_Send_Sensor_Data AssembleProtocolPacketPushSendQueue 中设置传输层协议目的地址;
		e.) CommunicationConfig.c 中 Protocol_LocalhostAddress 设置传输层协议本机地址;
		f.) main.c 中 E30TTLUART_MultiCountConfig设置链路层本机地址和信道;
		f.) E30TTLUART.c 中 E30TTLUART_Appoint_Target_Address 中设置链路层目的地址和信道;
		g.) wifi_config.c 中设置wifi与socket相关数据;
		
7) 以下为各模块接口（具体接口文本在各模块文件夹内）。



/*************************************************
STM32串口驱动ESP8266 wifi模块STA TCP Client模式

1) 依赖串口2
2)引脚连接：
ESP8266_UTXD  --- STM32_A3(USART2_RXD)
ESP8266_URXD  --- STM32_A2(USART2_TXD)
ESP8266_VCC   --- STM32_3V3
ESP8266_GND   --- STM32_GND
ESP8266_RST   --- STM32_A1

配置路由器wifi SSID KEY、TCP Server后，开机自动连接  并发数据测试。
PC连接USART1，通信链路如下：
USART1 <--> USART2 <--> ESP8266 <--> 路由器 <--> TCP server
通信数据收发及显示均正常。
*************************************************/


/*************************************************
STM32 SPI1驱动微雪OLED模块 4线SPI模式

1) 依赖SPI1模块
2) 引脚连接：
OLED_VCC  --- STM32_3V3
OLED_DIN  --- STM32_A7(SPI1_MOSI)
OLED_CLK  --- STM32_A5(SPI1_SCK)
OLED_CS   --- STM32_A4(SPI1_NSS)
OLED_DC   --- STM32_B0
OLED_RES  --- STM32_C0

SPI_DataSize_8b
SPI_CPOL_High
SPI_CPHA_2Edge
SPI_FirstBit_MSB
*************************************************/

/*************************************************
STM32 驱动DHT11模块

1) 引脚连接:
DHT11_OUT <---> STM32_C13
DHT11_+   <---> STM32_3V3
DHT11_-   <---> STM32_GNG
*************************************************/

/*************************************************
STM32 ADC驱动MQ-02模块

1) 依赖ADC模块
2) 引脚连接:
MQ02_AO  <---> STM32_A0
MQ02_VCC <---> STM32_3V3
MQ02_GNG <---> STM32_GNG

可以模拟与数字双路输出 模拟量输出范围为：0-5V。污染气体的浓度越大其输出电压越高。
*************************************************/

/*************************************************
STM32 驱动HC-SR501模块

1) 由于模块数据不够准确，因此未用中断，仅用查询。
2) 引脚连接:
HC-SR501_OUT  <---> STM32_B5
HC-SR501_VCC  <---> STM32_5V
HC-SR501_GND  <---> STM32_GND

有人靠近时高电平   没人时为低电平
传感器背面，电位器朝上时，
左电位器为延时调节，顺时针增加感应延时时间；
右电位器为距离调节，顺时针增加感应距离；
*************************************************/


/*************************************************
STM32 计时器中断驱动HC-SR04模块

1) 依赖TIMER4模块捕获电平
2) 引脚连接:
HC-SR04_Targ <---> STM32_A8
HC-SR04_Echo <---> STM32_B6
HC-SR04_VCC  <---> STM32_5V
HC-SR04_GND  <---> STM32_GND

*************************************************/

/*************************************************
STM32 模拟IIC驱动BH1750模块

1) 使用独立引脚模拟IIC
2) 引脚连接:
BH1750_SCL <---> STM32_PC5
BH1750_SDA <---> STM32_PC4
BH1750_VCC <---> STM32_3V3
BH1750_GND <---> STM32_GND

*************************************************/


/*************************************************
STM32 硬件SPI2驱动RC522模块

引脚连接:
1--RC522_SDA <--->STM32_PB1
2--RC522_SCK <--->STM32_PB13
3--RC522_MOSI<--->STM32_PB15
4--RC522_MISO<--->STM32_PB14
5--RC522_IRQ <--->NC
6--RC522_GND <--->STM32_GND
7--RC522_RST <--->STM32_PB2
8--RC522_3.3V<--->STM32_3V3
*************************************************/

/*************************************************
STM32 USART3驱动E30TTLUART模块

E30TTLUART_AUX  <---> STM32_B7
E30TTLUART_M0   <---> STM32_B8
E30TTLUART_M1   <---> STM32_B9
E30TTLUART_RXD  <---> STM32_B10(USART3_TX)
E30TTLUART_TXD  <---> STM32_B11(USART3_RX)
E30TTLUART_VCC  <---> STM32_3V3
E30TTLUART_GND  <---> STM32_GNG
*************************************************/

/*************************************************
STM32 串口4驱动SDS01模块

SDS01_TXD <---> STM32_PC11
SDS01_RXD <---> STM32_PC12
SDS01_VCC <---> STM32_5V
SDS01_GND <---> STM32_GND
*************************************************/













