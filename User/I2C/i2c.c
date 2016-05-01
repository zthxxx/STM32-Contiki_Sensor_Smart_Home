#include "i2c.h"
#include "delay.h"  

//初始化IIC
void I2C_Analog_Port_Init(void)
{					     
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOC, ENABLE );	
	   
	GPIO_InitStructure.GPIO_Pin = I2C_ANALOG_SDA_GPIO_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(I2C_ANALOG_SDA_GPIO_Port, &GPIO_InitStructure);
 
    GPIO_InitStructure.GPIO_Pin = I2C_ANALOG_SCL_GPIO_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(I2C_ANALOG_SCL_GPIO_Port, &GPIO_InitStructure);
    
	I2C_SCL=1;
	I2C_SDA=1;

}
void SDA_OUT(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  GPIO_InitStructure.GPIO_Pin = I2C_ANALOG_SDA_GPIO_Pin;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; //开漏输出
  GPIO_Init(I2C_ANALOG_SDA_GPIO_Port,&GPIO_InitStructure);
}
void SDA_IN(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  GPIO_InitStructure.GPIO_Pin = I2C_ANALOG_SDA_GPIO_Pin;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(I2C_ANALOG_SDA_GPIO_Port,&GPIO_InitStructure);
}
//产生IIC起始信号
void I2C_Start(void)
{
	SDA_OUT();     //sda线输出
	I2C_SDA=1;	  	  
	I2C_SCL=1;
	Delay_NOP_us(4);
 	I2C_SDA=0;//START:when CLK is high,DATA change form high to low 
	Delay_NOP_us(4);
	I2C_SCL=0;//钳住I2C总线，准备发送或接收数据 
}	  
//产生IIC停止信号
void I2C_Stop(void)
{
	SDA_OUT();//sda线输出
	I2C_SCL=0;
	I2C_SDA=0;//STOP:when CLK is high DATA change form low to high
 	Delay_NOP_us(4);
	I2C_SCL=1; 
	I2C_SDA=1;//发送I2C总线结束信号
	Delay_NOP_us(4);							   	
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
uint8_t I2C_Wait_Ack(void)
{
	uint8_t ucErrTime=0;
	SDA_IN();      //SDA设置为输入  
	I2C_SDA=1;Delay_NOP_us(1);	   
	I2C_SCL=1;Delay_NOP_us(1);
	//
	SDA_IN();      //SDA设置为输入	 
	while(SDA_Read)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			I2C_Stop();
			return 1;
		}
	}
	I2C_SCL=0;//时钟输出0 	   
	return 0;  
} 
//产生ACK应答
void I2C_Ack(void)
{
	I2C_SCL=0;
	SDA_OUT();
	I2C_SDA=0;
	Delay_NOP_us(2);
	I2C_SCL=1;
	Delay_NOP_us(2);
	I2C_SCL=0;
}
//不产生ACK应答		    
void I2C_NAck(void)
{
	I2C_SCL=0;
	SDA_OUT();
	I2C_SDA=1;
	Delay_NOP_us(2);
	I2C_SCL=1;
	Delay_NOP_us(2);
	I2C_SCL=0;
}					 				     
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void I2C_Send_Byte(uint8_t txd)
{                        
    uint8_t t;   
	SDA_OUT(); 	    
    I2C_SCL=0;//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
        I2C_SDA=(txd&0x80)>>7;
        txd<<=1; 	  
		Delay_NOP_us(2);   //对TEA5767这三个延时都是必须的
		I2C_SCL=1;
		Delay_NOP_us(2); 
		I2C_SCL=0;	
		Delay_NOP_us(2);
    }	 
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
uint8_t I2C_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA设置为输入
    for(i=0;i<8;i++ )
	{
        I2C_SCL=0; 
        Delay_NOP_us(2);
		I2C_SCL=1;
        receive<<=1;
        if(SDA_Read)receive++;   
		Delay_NOP_us(1); 
    }					 
    if (!ack)
        I2C_NAck();//发送nACK
    else
        I2C_Ack(); //发送ACK   
    return receive;
}


