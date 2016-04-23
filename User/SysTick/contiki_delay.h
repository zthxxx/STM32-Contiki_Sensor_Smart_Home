#include "stm32f10x.h"
#include "contiki-conf.h"
#include <stdint.h>
#include <stdio.h>
#include <debug-uart.h>
#include <process.h>
#include <procinit.h>
#include <etimer.h>
#include <autostart.h>
#include <clock.h>


#define Contiki_etimer_DelayMS(MS) \
    etimer_set(&et, MS/(1000/CLOCK_SECOND)); \
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et))



