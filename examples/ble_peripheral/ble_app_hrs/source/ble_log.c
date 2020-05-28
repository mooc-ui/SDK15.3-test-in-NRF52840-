#include "ble_log.h"

#define OPCODE_LENGTH 1                                                              /**< Length of opcode inside EEG packet. */
#define HANDLE_LENGTH 2   
#define MAX_EEG_LEN      (NRF_SDH_BLE_GATT_MAX_MTU_SIZE - OPCODE_LENGTH - HANDLE_LENGTH) 


static uint32_t ble_log_ctrl_char_add(ble_log_t * p_log)
{
    ble_add_char_params_t add_char_params;

    memset(&add_char_params, 0, sizeof(add_char_params));
    add_char_params.uuid                = BLE_LOG_UUID_LED_CHAR;//通道uuid
    add_char_params.uuid_type           = p_log->uuid_type;
    add_char_params.char_props.indicate = 1;//对应手机app nrf connect INDICATE
    add_char_params.char_props.write    = 1;//对应手机app nrf connect WRITE
    //add_char_params.is_defered_write    = true;
    add_char_params.is_var_len          = true;
    add_char_params.max_len             = MAX_EEG_LEN;

    add_char_params.cccd_write_access = SEC_OPEN;
    add_char_params.write_access      = SEC_OPEN;

    return characteristic_add(p_log->service_handle, &add_char_params, &p_log->ctrl_handles);
}


static uint32_t ble_log_data_char_add(ble_log_t * p_log)
{
    ble_add_char_params_t add_char_params;

    memset(&add_char_params, 0, sizeof(add_char_params));
    add_char_params.uuid                = BLE_LOG_UUID_BUTTON_CHAR;//通道uuid
    add_char_params.uuid_type           = p_log->uuid_type;
    add_char_params.char_props.indicate = 1;//对应手机app nrf connect INDICATE
    add_char_params.char_props.write    = 1;//对应手机app nrf connect WRITE
    //add_char_params.is_defered_write    = true;
    add_char_params.is_var_len          = true;
    add_char_params.max_len             = MAX_EEG_LEN;

    add_char_params.cccd_write_access = SEC_OPEN;
    add_char_params.write_access      = SEC_OPEN;

    return characteristic_add(p_log->service_handle, &add_char_params, &p_log->ctrl_handles);
}


uint32_t ble_log_init(ble_log_t * p_log, ble_log_init_t const * p_log_init)
{
	uint32_t err_code;
    ble_uuid_t service_uuid;
    ble_uuid128_t eeg_base_uuid = LOG_UUID_BASE;

    // Initialize service structure
    p_log->evt_handler					= p_log_init->evt_handler;
	p_log->is_indicate_enabled			= false;
	p_log->is_notify_enabled			= false;
	p_log->is_confirm_indicated			= false;
    p_log->conn_handle 					= BLE_CONN_HANDLE_INVALID;
    p_log->max_eeg_len					= MAX_EEG_LEN;
	
    
	// Add vendor specific base UUID to use with the Buttonless DFU characteristic.
    err_code = sd_ble_uuid_vs_add(&eeg_base_uuid, &p_log->uuid_type);
    if (err_code != NRF_SUCCESS) {
        return err_code;
    }

	// Add service
    //BLE_UUID_BLE_ASSIGN(service_uuid, BLE_EEG_SERVICE_UUID);
	service_uuid.type = p_log->uuid_type;
	service_uuid.uuid = BLE_LOG_UUID_SERVICE;//添加服务uuid
    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
                                        &service_uuid,
                                        &p_log->service_handle);
	
    if (err_code != NRF_SUCCESS) {
        return err_code;
    }	

	// Add the ctrl char.
    err_code = ble_log_ctrl_char_add(p_log);//里面有添加通道uuid
	if (err_code != NRF_SUCCESS) {
        return err_code;
    }

	// Add the data char.  这个地方要是打开的话需要调整start ram 和NRF_SDH_BLE_GATTS_ATTR_TAB_SIZE的大小
    //NRF_SDH_BLE_VS_UUID_COUNT 3  也需要更改
//    err_code = ble_log_data_char_add(p_log);
//	if (err_code != NRF_SUCCESS) {
//        return err_code;
//    }

//	// Add the read char.
//    err_code = ble_log_read_char_add(p_log);
//	if (err_code != NRF_SUCCESS) {
//        return err_code;
//    }
	
    return NRF_SUCCESS;
}


void ble_log_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context)
{
    ble_log_t * p_eeg = (ble_log_t *) p_context;

    switch (p_ble_evt->header.evt_id)
    {
		case BLE_GAP_EVT_CONNECTED:
		    //on_connect(p_eeg, p_ble_evt);
		    break;

		case BLE_GAP_EVT_DISCONNECTED:
		    //on_disconnect(p_eeg, p_ble_evt);
		    break;

		case BLE_GATTS_EVT_WRITE:
		    //on_write(p_eeg, p_ble_evt);
		    break;

		case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST:
		    //on_rw_authorize_req(p_ble_evt);
		    break;

		case BLE_GATTS_EVT_HVC:
		    //on_hvc(p_eeg, p_ble_evt);
		    break;

		default:
		    // No implementation needed.
		    break;
    }
}

