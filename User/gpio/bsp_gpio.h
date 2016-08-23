#ifndef __GPIO_H
#define	__GPIO_H


#include "stm32f10x.h"

#define LED_Red_Port    GPIOC
#define LED_Red_Pin     GPIO_Pin_6
#define LED_Red_RCC_Periph RCC_APB2Periph_GPIOC

#define LED_Green_Port  GPIOC
#define LED_Green_Pin   GPIO_Pin_7
#define LED_Green_RCC_Periph RCC_APB2Periph_GPIOC

#define Beep_Port  GPIOC
#define Beep_Pin   GPIO_Pin_8
#define Beep_RCC_Periph RCC_APB2Periph_GPIOC

typedef struct LightLamp
{
    uint8_t SwitchIndex;
    char* DisplayName;
    uint8_t LightStatus;
    void (*LightOn)(void);
    void (*LightOff)(void);
}LightLamp;

extern LightLamp lamps[];
extern uint16_t beep_flag;
void LED_GPIO_Config(void);
void Beep_GPIO_Config(void);
void LED_Red_On(void);
void LED_Red_Off(void);
void LED_Green_On(void);
void LED_Green_Off(void);
void Beep_On(void);
void Beep_Off(void);

#endif /* __GPIO_H */
