#ifndef _SWTIMER_H_
#define _SWTIMER_H_

#include "app_timer.h"
#include "nrf_log.h"
#include "ble_gap.h"
#include "nrf_ble_scan.h"
//#define SCAN_TIMER_INTERVAL			5

bool swtimer_init(void);
bool mring_startBLEIndicateTimer(void);
bool mring_stopBleRawTimer(void);


#endif
