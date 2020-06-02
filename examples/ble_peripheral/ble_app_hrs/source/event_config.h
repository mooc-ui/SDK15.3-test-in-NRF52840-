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

typedef struct
{
	uint16_t bleWriteOp;
	uint16_t len;
	uint8_t data[20];
	uint8_t status;
}st_bleWrite_t;



/*
brief:ble property include write/indicate/notify/read ect.
*/
typedef enum{
    BLE_INDICATE = 1,
    BLE_NOTIFY,
    BLE_IDLE,
    
    BLE_PROPERTY_MAX,//can't to here
}ble_property;

//typedef enum{
//    //
//}board_status;

typedef void (*userDefFunc)(void* pText);

typedef struct _loopCallback
{
	uint16_t event;
	userDefFunc func;
}loopCallback;