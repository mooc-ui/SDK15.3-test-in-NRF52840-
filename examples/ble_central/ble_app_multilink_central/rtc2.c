
#include "rtc2.h"

const  nrf_drv_rtc_t   		rtc2 = NRF_DRV_RTC_INSTANCE(2);
#define RTC2_INTERVAL					1														//uints : sec
#define RTC2_CC_VALUE 					(RTC2_INTERVAL * 32)					//Determines the RTC interrupt frequency and thereby the utc sampling frequency


static volatile uint32_t runSec = 0;


void gateway_rtc2_stop(void)
{
    NVIC_DisableIRQ(RTC2_IRQn);

    NRF_RTC2->EVTENCLR = RTC_EVTEN_COMPARE0_Msk;
    NRF_RTC2->INTENCLR = RTC_INTENSET_COMPARE0_Msk;

    NRF_RTC2->TASKS_STOP = 1;
    nrf_delay_us(MAX_RTC_TASKS_DELAY);

    NRF_RTC2->TASKS_CLEAR = 1;
    nrf_delay_us(MAX_RTC_TASKS_DELAY);
}


static void rtc2_handler(nrf_drv_rtc_int_type_t int_type)
{
    uint32_t err_code;
    static uint8_t flag_count;
    //NRF_LOG_INFO("***********enter rtc2  int_type = %d",int_type); 
    if (int_type == NRF_DRV_RTC_INT_COMPARE0)//比较中断
    {  
//        nrf_drv_rtc_disable(&rtc2);
//		err_code = nrf_drv_rtc_cc_set(&rtc2,0, 32,true);//
//        NRF_LOG_INFO("nrf_drv_rtc_cc_set return value= %d",err_code);
//		//APP_ERROR_CHECK(err_code);
//        nrf_drv_rtc_counter_clear(&rtc2);                               //Clear the RTC counter to start count from zero
//        nrf_drv_rtc_enable(&rtc2);
//        
//        runSec++;
//		if(runSec >= 0xFFFFFFFF){
//			runSec = 0;
//        }
//        NRF_LOG_INFO("*******************RTC2 SECOND TIME:%d",runSec);
    }
    if(int_type == NRFX_RTC_INT_OVERFLOW){
        NRF_LOG_INFO("***********NRFX_RTC_INT_OVERFLOW");
    }
    if(int_type == NRF_DRV_RTC_INT_TICK){
        flag_count++;
        if(flag_count == 8){ //a time 125ms   1s = 125ms*8
            flag_count = 0;
            runSec++;
            if(runSec >= 0xFFFFFFFF){
                runSec = 0;
            }
            //NRF_LOG_INFO("*******************RTC2 SECOND TIME:%d",runSec);            
        }

    }
}


void gateway_rtc2_enable(void)
{
    uint32_t err_code;
	// Stop RTC to prevent any running timers from expiring (in case of reinitialization)
	gateway_rtc2_stop();
	
    //Initialize RTC instance
    nrf_drv_rtc_config_t rtc_config;
    rtc_config.prescaler = 4095;//to 8hz
    err_code = nrf_drv_rtc_init(&rtc2, &rtc_config, rtc2_handler);              //Initialize the RTC with callback function rtc_handler. The rtc_handler must be implemented in this applicaiton. Passing NULL here for RTC configuration means that configuration will be taken from the nrf_drv_config.h file.
    NRF_LOG_INFO("*********nrf_drv_rtc_init return value= %d",err_code);
    APP_ERROR_CHECK(err_code);
    
    	//Enable tick event & interrupt
	nrf_drv_rtc_tick_enable(&rtc2, true);
	
    err_code = nrf_drv_rtc_cc_set(&rtc2,0,RTC2_CC_VALUE,true);           //Set RTC compare value to trigger interrupt. Configure the interrupt frequency by adjust RTC_CC_VALUE and RTC2_CONFIG_FREQUENCY constant in the nrf_drv_config.h file
    NRF_LOG_INFO("***********nrf_drv_rtc_cc_set return value= %d",err_code);
    APP_ERROR_CHECK(err_code);

    //Power on RTC instance
    nrf_drv_rtc_enable(&rtc2);                                          //Enable RTC
}

void gateway_rtc2_disable(void)
{

	ret_code_t err_code = nrf_drv_rtc_cc_disable(&rtc2, 0);
    NRF_LOG_INFO("nrf_drv_rtc_cc_disable return value= %d",err_code);
	APP_ERROR_CHECK(err_code);

	//Power off RTC instance
	nrf_drv_rtc_disable(&rtc2);                       

	nrf_drv_rtc_uninit(&rtc2);
}


uint32_t get_sec(void){
    return (runSec);
}




