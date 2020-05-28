#ifndef __RTC__H__
#define __RTC__H__

//use RTC2

#include "nrfx_rtc.h"
#include "nrf_delay.h"
#include "nrf_log.h"
#include "mring_queue.h"



void rtc2_enable(void);
void rtc2_disable(void);
uint32_t get_time(void);

#endif 