#include <stdlib.h>
#include "model_utils.h"
#include <bluetooth/mesh/models.h>
#include <modules/hal/nordic/nrfx/hal/nrf_radio.h>

// Defining model identifiers

#define BT_MESH_NORDIC_SEMI_COMPANY_ID 0x0059
#define BT_MESH_MODEL_ID_SETTINGS_MOD 0x0005 // DO: get new model id

// Defining OPcodes for messages

#define MESSAGE_DEVICE_SETTINGS_GET_OP    
    BT_MESH_MODEL_OP_3(0x01, BT_MESH_NORDIC_SEMI_COMPANY_ID)
#define MESSAGE_DEVICE_SETTINGS_SET_OP    
    BT_MESH_MODEL_OP_3(0x02, BT_MESH_NORDIC_SEMI_COMPANY_ID)
#define MESSAGE_DEVICE_SETTINGS_STATUS_OP    
    BT_MESH_MODEL_OP_3(0x03, BT_MESH_NORDIC_SEMI_COMPANY_ID)

#define BT_MESH_DEVICE_SETTINGS_MSG_MINLEN_GET 0 
#define BT_MESH_DEVICE_SETTINGS_MSG_MINLEN_SET 2
#define BT_MESH_DEVICE_SETTINGS_MSG_MAXLEN_SET 5 // DO: fix values
#define BT_MESH_DEVICE_SETTINGS_MSG_MINLEN_STATUS 2

/** TXP values */
struct bt_mesh_settings_txp {
	/**< 0 dBm. */
	nrf_radio_txpower_t zerodB;
	/**< -4 dBm. */
	nrf_radio_txpower_t neg4dB;
	/**< -12 dBm. */
	nrf_radio_txpower_t neg12dB;
    /**< -20 dBm. */
    nrf_radio_txpower_t neg20dB;
};

/** Mandatory parameters for the Settings Set message. */
struct bt_mesh_settings_set {
	/** State to set. */
	nrf_radio_txpower_t txp_value; // TXP value
	/** Transition parameters. */
	const struct bt_mesh_model_transition *transition;
};

/** Parameters for the Settings Status message. */
struct bt_mesh_settings_status {
	/** The present value of the Generic OnOff state. */
	
    // DO: add present value... type (?) 

	/** Remaining time value in milliseconds. */
	int32_t remaining_time;
};