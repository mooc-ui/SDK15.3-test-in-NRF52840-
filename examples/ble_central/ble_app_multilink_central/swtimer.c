
#include "swtimer.h"

APP_TIMER_DEF(m_scan_timer_id);

static ble_gap_addr_t source_addr = {
    0x01,
    0x00,
    {0xE9, 0X31, 0X6D, 0X5C, 0x7A, 0xE9}
};

static void scan_timeout_handle(void * p_context){
    //m_again_scan_flag = 1;
   // APP_ERROR_CHECK(sd_ble_gap_addr_set(&source_addr));//modify mac
}

bool swtimer_init(void)
{
	uint32_t err_code;
    // Create timers.
    err_code = app_timer_create(&m_scan_timer_id,
                                APP_TIMER_MODE_REPEATED,
                                scan_timeout_handle);
    APP_ERROR_CHECK(err_code);

	if(err_code != NRF_SUCCESS)
	{
		NRF_LOG_INFO("scan start softtimer fail\r\n");
		return false;
	}
	NRF_LOG_INFO(" scan start softtimer ok\r\n");
	return true;
}

bool mring_startBLEIndicateTimer(void)
{
	uint32_t err_code;
	err_code = app_timer_start(m_scan_timer_id, APP_TIMER_TICKS(10000), NULL);
	APP_ERROR_CHECK(err_code);
	if(err_code != NRF_SUCCESS)
	{
		NRF_LOG_INFO("start ble indicate timer fail\r\n");
		return false;
	}
	return true;
}

bool mring_stopBleRawTimer(void)
{
	uint32_t err_code;
	err_code = app_timer_stop(m_scan_timer_id);
	APP_ERROR_CHECK(err_code);
	if(err_code != NRF_SUCCESS)
	{
		NRF_LOG_INFO("stop scan timer fail\r\n");
		return false;
	}
	return true;
}


