#ifndef __TIMER4_CAP_H
#define __TIMER4_CAP_H
#include "sys.h"
#include <stdio.h>
#include "NVIC_CONFIG.H"

extern uint32_t Channel_1_UpTime;	//捕获总高电平的时间

void TIM4_Cap_Channel_1_Init(u16 arr, u16 psc);

#endif
