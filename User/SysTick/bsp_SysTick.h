#ifndef __SYSTICK_H
#define __SYSTICK_H


#include "stm32f10x.h"


void SysTick_Init( void );
extern void TimingDelay_Decrement( void );
extern void Delay_ms( __IO u32 nTime );


#endif /* __SYSTICK_H */
