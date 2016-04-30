/*************************************************
STM32串口驱动ESP8266 wifi模块STA TCP Client模式

引脚连接：
ESP8266_UTXD  --- STM32_A3(UART2_RXD)
ESP8266_URXD  --- STM32_A2(UART2_TXD)
ESP8266_VCC   --- STM32_3V3
ESP8266_GND   --- STM32_GND
ESP8266_RST   --- STM32_A1

配置路由器wifi SSID KEY、TCP Server后，开机自动连接  并发数据测试。
PC连接UART1，通信链路如下：
UART1 <--> UART2 <--> ESP8266 <--> 路由器 <--> TCP server
通信数据收发及显示均正常。
*************************************************/


/*************************************************
STM32 SPI1驱动微雪OLED模块 4线SPI模式

引脚连接：
OLED_VCC  --- STM32_3V3
OLED_DIN  --- STM32_A7(SPI1_MOSI)
OLED_CLK  --- STM32_A5(SPI1_SCK)
OLED_CS   --- STM32_A4(SPI1_NSS)
OLED_DC   --- STM32_B0
OLED_RES  --- STM32_B10

SPI_DataSize_8b
SPI_CPOL_High
SPI_CPHA_2Edge
SPI_FirstBit_MSB
*************************************************/



/*************************************************
STM32 SPI1驱动DHT11模块

DHT11_OUT <---> STM32_B11
DHT11_+   <---> STM32_3V3
DHT11_-   <---> STM32_GNG
*************************************************/