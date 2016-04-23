#ifndef __OLED_H
#define __OLED_H

/* includes */
#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "SPI.h"

/* defines */
#define     RCC_APB2Periph_OLED_PORT        RCC_APB2Periph_GPIOA

#define     OLED_PORT                       GPIOA
#define     OLED_DC_RES_PORT                GPIOB
#define     OLED_RST_PIN                    GPIO_Pin_10

#define     OLED_RST_L                      GPIO_ResetBits(GPIOB, OLED_RST_PIN)
#define     OLED_RST_H                      GPIO_SetBits(GPIOB, OLED_RST_PIN)

#define     OLED_DC_PIN                     GPIO_Pin_0

#define	    OLED_DC_L                       GPIO_ResetBits(GPIOB, OLED_DC_PIN);
#define     OLED_DC_H                       GPIO_SetBits(GPIOB, OLED_DC_PIN);

#define  	OLED_SCK_PIN					GPIO_Pin_5//对应SLK
#define		OLED_SDA_PIN					GPIO_Pin_7
   
/* OLED_WriteByte */
void OLED_WB(uint8_t data);
void OLED_DLY_ms(unsigned int ms);
void delay_us(u16 time);
void LCD_Init(void);
void OLED_CLS(void);
void LCD_Fill(unsigned char dat);

//SSD1306 OLED 驱动IC驱动代码
//驱动方式:SPI
//版本:V0.1

//2010-6-3

//OLED模式设置
//4线串行模式
		    						  
//OLED控制用函数
void OLED_WR_Byte(u8 dat,u8 cmd);	    
void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_Refresh_Gram(void);		   
							   		    
void OLED_Init(void);
void OLED_DrawPoint(u8 x,u8 y,u8 t);
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot);
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode);
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size);
void OLED_ShowString(u8 x,u8 y,const u8 *p);	


#endif
