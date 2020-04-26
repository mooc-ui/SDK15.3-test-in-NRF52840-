
#include "nrf_drv_rtc.h"
#include "nrf_delay.h"
#include "nrf_log.h"
#include <nrfx_rtc.h>


#define MAX_RTC_TASKS_DELAY     47  


void gateway_rtc2_stop(void);
void gateway_rtc2_enable(void);
void gateway_rtc2_disable(void);
uint32_t get_sec(void);