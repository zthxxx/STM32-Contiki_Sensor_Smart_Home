#ifndef __GPIO_H
#define	__GPIO_H


#include "stm32f10x.h"

#define LED_Red_Port    GPIOA
#define LED_Red_Pin     GPIO_Pin_8
#define LED_Red_RCC_Periph RCC_APB2Periph_GPIOA

#define LED_Green_Port  GPIOD
#define LED_Green_Pin   GPIO_Pin_2
#define LED_Green_RCC_Periph RCC_APB2Periph_GPIOD

#define Gate_Port  GPIOC
#define Gate_Pin   GPIO_Pin_9
#define Gate_RCC_Periph RCC_APB2Periph_GPIOC

typedef struct LightLamp
{
    uint8_t SwitchIndex;
    char* DisplayName;
    uint8_t LightStatus;
    void (*LightOn)(void);
    void (*LightOff)(void);
}LightLamp;

extern LightLamp lamps[];
void LED_GPIO_Config(void);
void Gate_GPIO_Config(void);
void LED_Red_On(void);
void LED_Red_Off(void);
void LED_Green_On(void);
void LED_Green_Off(void);
void Gate_On(void);
void Gate_Off(void);

#endif /* __GPIO_H */

