#ifndef __MQ2_H
#define __MQ2_H	
#include "sys.h"
#include "adc.h"

#define MQ02_AO_GPIO_Pin        GPIO_Pin_0
#define MQ02_AO_GPIO_Side       GPIOA
#define MQ02_ADC_CHANNEL        ADC_Channel_0


void MQ02_Init(void);
void MQ02_GPIO_Config(void);
uint16_t MQ02_Read_Value(void);
float MQ02_Get_Percentage(void);
 
#endif 


