#include <stdint.h>
enum
{
	NONE_OP	= 0x0000,
    SENSOR_OP,
    RADIO_TX_OP,
    RADIO_RX_OP,
    FLASH_WRITE_READ_OP,
	SYS_OP,
    SENSOR_DATA_PROCESS_OP,
    POWER_MANAGEMENT_OP,
   
	// user define should not beyound here.
	MAX_OPERA_NUM,
};

typedef void (*userDefFunc)(void* pText);

typedef struct _loopCallback
{
	uint16_t event;
	userDefFunc func;
}loopCallback;