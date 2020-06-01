#include "ble_log.h"

#define OPCODE_LENGTH 1                                                              /**< Length of opcode inside EEG packet. */
#define HANDLE_LENGTH 2   
#define MAX_EEG_LEN      (NRF_SDH_BLE_GATT_MAX_MTU_SIZE - OPCODE_LENGTH - HANDLE_LENGTH) 
#define CMD_LEN 20
static uint8_t m_cmd[CMD_LEN] = {0};

static uint32_t ble_log_ctrl_char_add(ble_log_t * p_log)
{
    ble_add_char_params_t add_char_params;

    memset(&add_char_params, 0, sizeof(add_char_params));
    add_char_params.uuid                = BLE_LOG_UUID_LED_CHAR;//通道uuid
    add_char_params.uuid_type           = p_log->uuid_type;
    //add_char_params.char_props.indicate = 1;//对应手机app nrf connect INDICATE
    //add_char_params.char_props.write    = 1;//对应手机app nrf connect WRITE
    //add_char_params.char_props.write      = 1;
    add_char_params.char_props.notify     = 1;
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
    //add_char_params.char_props.indicate = 1;//对应手机app nrf connect INDICATE
    add_char_params.char_props.write    = 1;//对应手机app nrf connect WRITE
    add_char_params.char_props.indicate = 1;
    //add_char_params.is_defered_write    = true;
    add_char_params.is_var_len          = true;
    add_char_params.max_len             = MAX_EEG_LEN;

    add_char_params.cccd_write_access = SEC_OPEN;
    add_char_params.write_access      = SEC_OPEN;

    return characteristic_add(p_log->service_handle, &add_char_params, &p_log->data_handles);
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
    err_code = ble_log_data_char_add(p_log);
	if (err_code != NRF_SUCCESS) {
        return err_code;
    }
    return NRF_SUCCESS;
}


void on_write(ble_log_t *p_eeg, ble_evt_t const *p_ble_evt){
    ble_gatts_evt_write_t* p_evt_write =  (ble_gatts_evt_write_t *)&p_ble_evt->evt.gatts_evt.params.write;
    uint16_t channel = p_evt_write->handle;
    //ctrl 
    if (p_evt_write->handle == p_eeg->ctrl_handles.cccd_handle) {//ctrl 通道的通知
		if (p_evt_write->len == 2) {
                //p_eeg->is_indicate_enabled = ble_srv_is_indication_enabled(p_evt_write->data);
                p_eeg->is_notify_enabled = ble_srv_is_notification_enabled(p_evt_write->data);
            }//当在手机端nrf connect 点击UUID为fee1的通知图标;就会执行到这里
        NRF_LOG_INFO("ctrl_handles UUID=0xFEE2 button p_eeg->is_indicate_enabled = %d p_eeg->is_notify_enabled = %d.",p_eeg->is_indicate_enabled, p_eeg->is_notify_enabled);        
    } 
    if(p_evt_write->handle == p_eeg->ctrl_handles.value_handle){//ctrl 通道的写
        //手机通过uuid为fee1的write通道发过来的数据;p_evt_write->data是一个数组
        NRF_LOG_INFO("data[0] = %d data[1] = %d data[2] = %d.", p_evt_write->data[0],p_evt_write->data[1],p_evt_write->data[2]);
        //接收的指令格式:[cmd_len][cmd1][cmd2][....][cmd20]
        memcpy(m_cmd, &p_evt_write->data[0], p_evt_write->data[0]);//m_cmd里面不包含cmd的长度字段
    }
    
    //data
    if(p_evt_write->handle == p_eeg->data_handles.cccd_handle){
        if (p_evt_write->len == 2) {
                p_eeg->is_indicate_enabled = ble_srv_is_indication_enabled(p_evt_write->data);
                //p_eeg->is_notify_enabled = ble_srv_is_notification_enabled(p_evt_write->data);
            }//当在手机端nrf connect 点击UUID为fee1的通知图标;就会执行到这里
        NRF_LOG_INFO("p_eeg->is_indicate_enabled  = %d UUID=0xFEE3 LED.", p_eeg->is_indicate_enabled);         
    } 
    if (p_evt_write->handle == p_eeg->data_handles.value_handle) {//data 通道的通知
        NRF_LOG_INFO("data_handles UUID=0x1573 unknow");
	}

}

/**@brief Function for handling the Connect event.
 *
 * @param[in]	 p_aux		 AUX structure.
 * @param[in]	 p_ble_evt	 Event received from the BLE stack.
 */
static void on_connect(ble_log_t * p_aux, ble_evt_t const * p_ble_evt)
    {//当手机和开发板连上之后,需要修改p_aux->conn_handle的值;这将作为设备互联的标识
	p_aux->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
}

static void on_disconnect(ble_log_t * p_aux, ble_evt_t const * p_ble_evt)
{
	UNUSED_PARAMETER(p_ble_evt);
	p_aux->conn_handle = BLE_CONN_HANDLE_INVALID;
}

void ble_log_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context)
{
    ble_log_t * p_eeg = (ble_log_t *) p_context;

    switch (p_ble_evt->header.evt_id)
    {
		case BLE_GAP_EVT_CONNECTED:
		    on_connect(p_eeg, p_ble_evt);
		    break;

		case BLE_GAP_EVT_DISCONNECTED:
		    on_disconnect(p_eeg, p_ble_evt);
		    break;

		case BLE_GATTS_EVT_WRITE:
		    on_write(p_eeg, p_ble_evt);
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

void get_from_app_data(uint8_t *data){
    memcpy(data, m_cmd, CMD_LEN);
}

