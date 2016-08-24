#ifndef __PWM_H
#define	__PWM_H

#include "stm32f10x.h"

void TIM1_PWM_Config(uint16_t TIM_Period,uint16_t TIM_Prescaler);//自动重装周期值  预分频值
void TIM1_PWM_Init(void);
void TIM1_CH1_PWM_Init(void);
void TIM1_CH4_PWM_Init(void);
void Set_TIM1_CH1_Percentage(uint8_t percentage);
void Set_TIM1_CH4_Percentage(uint8_t percentage);
#endif /* __PWM_H */

