#include <stdlib.h>
#include "model_utils.h"
#include <bluetooth/mesh/models.h>

// Defining model identifiers

#define BT_MESH_NORDIC_SEMI_COMPANY_ID 0x0059
#define BT_MESH_MODEL_ID_SETTINGS_CLI 0x0005
#define BT_MESH_MODEL_ID_SETTINGS_SRV 0x0006 

// Defining OPcodes for messages

#define BT_MESH_DEVICE_SETTINGS_GET_OP  
    BT_MESH_MODEL_OP_3(0x01, BT_MESH_NORDIC_SEMI_COMPANY_ID)
#define BT_MESH_DEVICE_SETTINGS_SET_OP    
    BT_MESH_MODEL_OP_3(0x02, BT_MESH_NORDIC_SEMI_COMPANY_ID)
#define BT_MESH_DEVICE_SETTINGS_STATUS_OP    
    BT_MESH_MODEL_OP_3(0x03, BT_MESH_NORDIC_SEMI_COMPANY_ID)

// DO: add MESSAGE_ACK_OPCODE

#define BT_MESH_DEVICE_SETTINGS_MSG_MINLEN_GET 0 
#define BT_MESH_DEVICE_SETTINGS_MSG_MINLEN_SET 2
#define BT_MESH_DEVICE_SETTINGS_MSG_MAXLEN_SET 5
#define BT_MESH_DEVICE_SETTINGS_MSG_MINLEN_STATUS 2

/** Mandatory parameters for the Settings Set message. */
struct bt_mesh_settings_set {
	/** State to set. */
	int8_t txp_value;
	/** Transition parameters. */
	const struct bt_mesh_model_transition *transition;
};

/** Parameters for the Settings Status message. */
struct bt_mesh_settings_status {
	/** The present value of the Generic OnOff state. */
	
    int8_t txp_present;

	/** Remaining time value in milliseconds. */
	int32_t remaining_time;
};