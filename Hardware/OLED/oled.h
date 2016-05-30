#ifndef __OLED_H
#define __OLED_H

/* includes */
#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "SPI.h"

/* defines */

#define     OLED_RST_PORT                   GPIOC
#define     OLED_RST_PIN                    GPIO_Pin_0

#define     OLED_RST_L                      GPIO_ResetBits(OLED_RST_PORT, OLED_RST_PIN)
#define     OLED_RST_H                      GPIO_SetBits(OLED_RST_PORT, OLED_RST_PIN)

#define     OLED_DC_PORT                    GPIOB
#define     OLED_DC_PIN                     GPIO_Pin_0

#define	    OLED_DC_L                       GPIO_ResetBits(OLED_DC_PORT, OLED_DC_PIN);
#define     OLED_DC_H                       GPIO_SetBits(OLED_DC_PORT, OLED_DC_PIN);

#define  	OLED_SCK_PIN					GPIO_Pin_5//对应SLK
#define		OLED_SDA_PIN					GPIO_Pin_7
#define     OLED_SPI_CONFIG                 SPI1_Init

#define     OLED_Max_Row    4
#define     OLED_Max_Col    16
/* OLED_WriteByte */
void OLED_WB(uint8_t data);
void OLED_DLY_ms(unsigned int ms);
void OLED_DLY_us(u16 time);
void LCD_Init(void);
void OLED_CLS(void);
void LCD_Fill(unsigned char dat);
void OLED_GPIO_Config(void);

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
void OLED_CLS(void);
void OLED_Fill_Alphabet(uint8_t row_x,uint8_t col_y,uint8_t count);
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode);
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size);
void OLED_ShowString(u8 x,u8 y,const u8 *p);
void OLED_ShowAlphabet(uint8_t row,uint8_t col,uint8_t alphabet);
void OLED_ShowAlphabets(uint8_t row,uint8_t col,uint8_t* alphabets);
void OLED_Show_Head_End_Word(uint8_t row,uint8_t col_head,uint8_t* head,uint8_t col_end,uint8_t* endding);
void OLED_ShowFloat(uint8_t row, uint8_t col_start, uint8_t col_end, float value, uint8_t* last_length);

#endif
