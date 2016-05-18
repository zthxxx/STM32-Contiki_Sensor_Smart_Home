/*************************************************************
                             \(^o^)/
  Copyright (C), 2013-2020, ZheJiang University of Technology
  File name  : SHT15.h 
  Author     : ziye334    
  Version    : V1.0 
  Data       : 2014/3/10      
  Description: Digital temperature and humidity sensor driver code
  
*************************************************************/
#ifndef __SHT15_H__
#define __SHT15_H__
#include "stm32f10x.h"

enum {TEMP, HUMI};

/* GPIO相关宏定义 */
#define SHT15_AHB2_CLK        RCC_APB2Periph_GPIOA
#define SHT15_DATA_PIN        GPIO_Pin_12
#define SHT15_SCK_PIN         GPIO_Pin_11
#define SHT15_DATA_PORT       GPIOA
#define SHT15_SCK_PORT        GPIOA

#define SHT15_DATA_H()        GPIO_SetBits(SHT15_DATA_PORT, SHT15_DATA_PIN)                         //拉高DATA数据线
#define SHT15_DATA_L()        GPIO_ResetBits(SHT15_DATA_PORT, SHT15_DATA_PIN)                         //拉低DATA数据线
#define SHT15_DATA_R()        GPIO_ReadInputDataBit(SHT15_DATA_PORT, SHT15_DATA_PIN)         //读DATA数据线

#define SHT15_SCK_H()        GPIO_SetBits(SHT15_SCK_PORT, SHT15_SCK_PIN)                                 //拉高SCK时钟线
#define SHT15_SCK_L()        GPIO_ResetBits(SHT15_SCK_PORT, SHT15_SCK_PIN)                         //拉低SCK时钟线

/* 传感器相关宏定义 */
#define        noACK        0
#define ACK                1
                                                                //addr  command         r/w
#define STATUS_REG_W        0x06        //000         0011          0          写状态寄存器
#define STATUS_REG_R        0x07        //000         0011          1          读状态寄存器
#define MEASURE_TEMP         0x03        //000         0001          1          测量温度
#define MEASURE_HUMI         0x05        //000         0010          1          测量湿度
#define SOFTRESET       0x1E        //000         1111          0          复位


void SHT15_Init(void);
void SHT15_ConReset(void);
u8 SHT15_SoftReset(void);
u8 SHT15_Measure(u16 *p_value, u8 *p_checksum, u8 mode);
void SHT15_Calculate(u16 t, u16 rh,float *p_temperature, float *p_humidity);
float SHT15_CalcuDewPoint(float t, float h);


#endif



