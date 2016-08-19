#ifndef __i2c_H
#define __i2c_H
#include "sys.h" 	    	   		  

#define I2C_ANALOG_SDA_GPIO_Port    GPIOC
#define I2C_ANALOG_SDA_GPIO_Pin     GPIO_Pin_4
#define I2C_ANALOG_SDA_RCC_Periph   RCC_APB2Periph_GPIOC
#define I2C_ANALOG_SCL_GPIO_Port    GPIOC
#define I2C_ANALOG_SCL_GPIO_Pin     GPIO_Pin_5
#define I2C_ANALOG_SCL_RCC_Periph   RCC_APB2Periph_GPIOC
//IO操作函数	 
#define I2C_SCL    PCout(5) //SCL
#define I2C_SDA    PCout(4) //SDA
#define SDA_Read   PCin(4)  //输入SDA

//IIC所有操作函数
void I2C_Analog_Port_Init(void);                //初始化IIC的IO口				 
void I2C_Start(void);				//发送IIC开始信号 成功则返回1
void I2C_Stop(void);	  			//发送IIC停止信号
void I2C_Send_Byte(uint8_t txd);			//IIC发送一个字节
uint8_t I2C_Read_Byte(unsigned char ack);//IIC读取一个字节
uint8_t I2C_Wait_Ack(void); 				//IIC等待ACK信号
void I2C_Ack(void);					//IIC发送ACK信号
void I2C_NAck(void);				//IIC不发送ACK信号
	  
#endif

