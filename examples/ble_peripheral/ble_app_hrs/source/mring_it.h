

#ifndef MRING_IT_H  
#define MRING_IT_H  
  
#ifdef __cplusplus  
extern "C"  
{  
#endif  

#include "nrf.h"
#include "hardfault.h"

#include "app_error.h"
#include "nrf_nvmc.h"
#include "nrf_soc.h"
#include "nrf_delay.h"
#include "nrf_nvic.h"  //里面有中断保护(sd_nvic_critical_region_enter)


#define USE_MUTEX	1

#if USE_MUTEX
#define MR_ENTER_CRITICAL()											\
		uint8_t is_nested_critical_region;                          \
		sd_nvic_critical_region_enter(&is_nested_critical_region);
#define MR_EXIT_CRITICAL()											\
		sd_nvic_critical_region_exit(is_nested_critical_region);
#else
#define MR_ENTER_CRITICAL()	
#define MR_EXIT_CRITICAL()	
#endif



#ifdef __cplusplus  
}  
#endif  
#endif /* MRING_IT_H */  


