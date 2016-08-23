#ifndef _BEEP_GPIO_H
#define	_BEEP_GPIO_H


#include "stm32f10x.h"


#define Beep_Port  GPIOC
#define Beep_Pin   GPIO_Pin_8
#define Beep_RCC_Periph RCC_APB2Periph_GPIOC

#define TemperatureAlertSource      0
#define HumidityAlertSource         1
#define GasConcentrationAlertSource 2
#define LightAlertSource            3
#define PM2_5_AlertSource           4
#define CO2_AlertSource             5

extern uint16_t beep_flag;
void Beep_GPIO_Config(void);

void Beep_On(void);
void Beep_Off(void);
void SetBeepAlertSource(uint8_t alertSource);
void ResetBeepAlertSource(uint8_t alertSource);
#endif /* _BEEP_GPIO_H */
