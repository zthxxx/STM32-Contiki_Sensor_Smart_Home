#ifndef _BEEP_GPIO_H
#define	_BEEP_GPIO_H


#include "stm32f10x.h"


#define Beep_Port  GPIOC
#define Beep_Pin   GPIO_Pin_8
#define Beep_RCC_Periph RCC_APB2Periph_GPIOC


extern uint16_t beep_flag;

void Beep_GPIO_Config(void);

void Beep_On(void);
void Beep_Off(void);

#endif /* _BEEP_GPIO_H */
