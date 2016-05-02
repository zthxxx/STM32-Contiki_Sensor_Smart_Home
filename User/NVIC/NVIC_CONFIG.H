#ifndef  __NVIC_CONFIG_H__
#define  __NVIC_CONFIG_H__

#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include <stdbool.h>

void NVIC_Configuration_Init(void);
void NVIC_IRQChannel_Configuration_Set(
    uint8_t NVIC_IRQChannel, 
    uint8_t NVIC_IRQChannelPreemptionPriority, 
    uint8_t NVIC_IRQChannelSubPriority, 
    FunctionalState NVIC_IRQChannelCmd
);


#endif  



