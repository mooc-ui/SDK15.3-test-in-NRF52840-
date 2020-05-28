#ifndef __BLE_LOG_H  
#define __BLE_LOG_H 

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "ble.h"
#include "ble_srv_common.h"
#include "nrf_sdh_ble.h"
#include "nrf_ble_gatt.h"
#include "nrf_log.h"
//#define BLE_EEG_SERVICE_UUID            0xFEE0 

//#define BLE_EEG_CTRL_CHAR_UUID			0xFEE1 
//#define BLE_EEG_DATA_CHAR_UUID			0xFEE2 
//#define BLE_EEG_READ_CHAR_UUID			0xFEE3 



///**@brief Nordic vendor-specific base UUID.
// */
//#define BLE_EEG_VENDOR_BASE_UUID                 \
//{{                                                  \
//    0x1A, 0x08, 0xE0, 0x89, 0xA5, 0x80, 0xEB, 0xB0, \
//    0xE9, 0x11, 0x6A, 0x8B, 0x00, 0x00, 0x6A, 0xF8  \
//}}


#define BLE_EEG_BLE_OBSERVER_PRIO 2
#define LOG_UUID_BASE {0x23, 0xD1, 0xBC, 0xEA, 0x5F, 0x78, 0x23, 0x15, 0xDE, \
                       0xEF, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00}

#define BLE_LOG_UUID_SERVICE     0xFFEF//0x1523
                       
#define BLE_LOG_UUID_LED_CHAR    0x1573
#define BLE_LOG_UUID_BUTTON_CHAR 0x1524
                       
                       
#define BLE_LOG_DEF(_name)                                                                          \
static ble_log_t _name;                                                                             \
NRF_SDH_BLE_OBSERVER(_name ## _obs,                                                                 \
                     BLE_EEG_BLE_OBSERVER_PRIO,                                                     \
                     ble_log_on_ble_evt, &_name)
                       
typedef enum
{
    BLE_LOG_EVT_NOTIFICATION_ENABLED,   /**< notification enabled event. */
    BLE_LOG_EVT_NOTIFICATION_DISABLED,   /**< notification disabled event. */
    BLE_LOG_EVT_INDICATION_ENABLED,   /**< indication enabled event. */
    BLE_LOG_EVT_INDICATION_DISABLED   /**< indication disabled event. */
} ble_log_evt_type_t;

/**@brief EEG event. */
typedef struct
{
    ble_log_evt_type_t evt_type;    /**< Type of event. */
} ble_log_evt_t;

// Forward declaration of the ble_eeg_t type.
typedef struct ble_log_s ble_log_t;

/**@brief EEG event handler type. */
typedef void (*ble_log_evt_handler_t) (ble_log_t * p_log, ble_log_evt_t * p_evt);

/**@brief EEG init structure. This contains all options and data needed for
 *        initialization of the service. */
typedef struct
{
    ble_log_evt_handler_t        evt_handler;                                          /**< Event handler to be called for handling events in the EEG. */
} ble_log_init_t;

/**@brief EEG structure. This contains various status information for the service. */
struct ble_log_s
{
	ble_log_evt_handler_t        evt_handler;                                          /**< Event handler to be called for handling events in the EEG. */
	uint8_t                      uuid_type;                      		               /**< UUID type for DFU UUID. */
	bool						 is_notify_enabled;
	bool						 is_indicate_enabled;
	bool						 is_confirm_indicated;
    uint16_t                     service_handle;                                       
    ble_gatts_char_handles_t     ctrl_handles;                                        /** write + indicate ack character */
    ble_gatts_char_handles_t     data_handles;										  /** write without response + notify character */ 
    ble_gatts_char_handles_t     read_handles;                                        	
	uint16_t					 conn_handle;										   /**< Handle of the current connection (as provided by the BLE stack, is BLE_CONN_HANDLE_INVALID if not in a connection). */
	uint8_t                      max_eeg_len;                                          /**< Current maximum length, adjusted according to the current ATT MTU. */
};



void ble_log_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context);
uint32_t ble_log_init(ble_log_t * p_log, ble_log_init_t const * p_log_init);


#endif