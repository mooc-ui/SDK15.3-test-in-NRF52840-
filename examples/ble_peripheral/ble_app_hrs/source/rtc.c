#include "rtc.h"
#include "event_config.h"


#define MAX_RTC_TASKS_DELAY     		47 
/*时钟频率是32.768kHZ   1tick=1/32768 ?*/
#define RTC2_CC_VALUE                   32768
#define INT_TYPE                        NRFX_RTC_INT_COMPARE0

const nrfx_rtc_t rtc2 = NRFX_RTC_INSTANCE(2);
static volatile uint32_t runSec = 0;

void rtc2_reload(nrfx_rtc_int_type_t int_type){
    nrfx_rtc_cc_set(&rtc2, int_type, RTC2_CC_VALUE, true);//一定要再次设置值才可以自动1s进入
    nrfx_rtc_counter_clear(&rtc2);//清除计数 
}

static void rtc2_handler(nrfx_rtc_int_type_t int_type){
    uint32_t err_code;
    switch(int_type){
        case NRFX_RTC_INT_COMPARE0:
            //NRF_LOG_INFO("into compare0");
            rtc2_reload(NRFX_RTC_INT_COMPARE0);      
            runSec++;
            if(runSec >= 0xFFFFFFFF){runSec = 0;}
            if(NULL != mrEvtQueue){//mrEvtQueue是一个全局变量,定义在main.c  extern 在mring_queue.h中
                stEvtBuffer buffer = {SYS_OP, NULL};
                mring_queueSend(mrEvtQueue, &buffer);
            }
            break;
        case NRFX_RTC_INT_COMPARE1:
            rtc2_reload(NRFX_RTC_INT_COMPARE1);             
            NRF_LOG_INFO("into compare1");
            break;
        case NRFX_RTC_INT_COMPARE2:
            rtc2_reload(NRFX_RTC_INT_COMPARE2); 
            NRF_LOG_INFO("into compare2");
            break;
        case NRFX_RTC_INT_COMPARE3:
            rtc2_reload(NRFX_RTC_INT_COMPARE3); 
            NRF_LOG_INFO("into compare3");
            break;
        case NRFX_RTC_INT_TICK://每次tick中断都进这里1tick = 1/32768s
            NRF_LOG_INFO("into NRFX_RTC_INT_TICK");//nrfx_rtc_tick_enable函数可以使能该通道
            break;
        case NRFX_RTC_INT_OVERFLOW:
            NRF_LOG_INFO("into NRFX_RTC_INT_OVERFLOW");
            break;
        default:
            break;
        }
}

void rtc2_enable(void){
    uint32_t err_code;
    nrfx_rtc_uninit(&rtc2);//to stop rtc2
    //Initialize RTC instance
    nrfx_rtc_config_t config = NRFX_RTC_DEFAULT_CONFIG;
    err_code = nrfx_rtc_init(&rtc2, &config, rtc2_handler);              //Initialize the RTC with callback function rtc_handler. The rtc_handler must be implemented in this applicaiton. Passing NULL here for RTC configuration means that configuration will be taken from the nrf_drv_config.h file.
    APP_ERROR_CHECK(err_code);
    //enable tick and interrupt
    //nrfx_rtc_tick_enable(&rtc2,true);//使能tick通道
    
    //set value
    err_code = nrfx_rtc_cc_set(&rtc2, INT_TYPE, RTC2_CC_VALUE, true);
    APP_ERROR_CHECK(err_code);
    
    nrfx_rtc_enable(&rtc2);
}

void rtc2_disable(void){
    uint32_t err_code;
    err_code = nrfx_rtc_cc_disable(&rtc2, 0);//通道失能
    nrfx_rtc_disable(&rtc2);//rtc2关闭
    nrfx_rtc_uninit(&rtc2);
}

uint32_t get_time(void){
    return runSec;
}















