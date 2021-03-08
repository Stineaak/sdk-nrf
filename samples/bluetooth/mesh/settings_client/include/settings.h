/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-BSD-5-Clause-Nordic
 */

/** @file
 *  @defgroup bt_mesh_settings Device_Settings Models
 *  @{
 *  @brief API for the Device_Settings model.
 */

#ifndef BT_MESH_SETTINGS_CLI_H__
#define BT_MESH_SETTINGS_CLI_H__

#include <stdlib.h>
#include "model_utils.h"
#include <bluetooth/mesh/models.h>
#include <bluetooth/mesh/model_types.h>

#ifdef __cplusplus
extern "C" {
#endif

// Defining model identifiers

#define BT_MESH_NORDIC_SEMI_COMPANY_ID 0x0059
#define BT_MESH_MODEL_ID_SETTINGS_CLI 0x0005
#define BT_MESH_MODEL_ID_SETTINGS_SRV 0x0006 

// Defining OPcodes for messages

#define BT_MESH_DEVICE_SETTINGS_GET_OP BT_MESH_MODEL_OP_3(0x01, BT_MESH_NORDIC_SEMI_COMPANY_ID)

#define BT_MESH_DEVICE_SETTINGS_SET_OP BT_MESH_MODEL_OP_3(0x02, BT_MESH_NORDIC_SEMI_COMPANY_ID)

#define BT_MESH_DEVICE_SETTINGS_STATUS_OP BT_MESH_MODEL_OP_3(0x03, BT_MESH_NORDIC_SEMI_COMPANY_ID)

//#define BT_MESH_DEVICE_SETTINGS_ACK_OP BT_MESH_MODEL_OP_3(0x04, BT_MESH_NORDIC_SEMI_COMPANY_ID)


#define BT_MESH_DEVICE_SETTINGS_MSG_LEN_GET 0 
#define BT_MESH_DEVICE_SETTINGS_MSG_MINLEN_SET 2
#define BT_MESH_DEVICE_SETTINGS_MSG_MAXLEN_SET 5
#define BT_MESH_DEVICE_SETTINGS_MSG_LEN_STATUS 3 
//#define BT_MESH_DEVICE_SETTINGS_MSG_LEN_ACK 0

/** Mandatory parameters for the Settings Set message. */
struct bt_mesh_settings_set {
	/** State to set. */
	int8_t txp_value;
	/** Transition parameters. */
	const struct bt_mesh_model_transition *transition;
};

struct bt_mesh_TEST_set {
	/** State to set - based on Button ID. int */
	int val;
	/** Transition parameters. */
	const struct bt_mesh_model_transition *transition;
};

/** Parameters for the Settings Status message. */
struct bt_mesh_settings_status {
	/** The present value of the Settiings state. */
    int8_t txp_present;
	/** The target value of the Settings state (optional). */
	int8_t txp_target;

	/** Remaining time value in milliseconds. */
	int32_t remaining_time;
};

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* BT_MESH_SETTINGS_CLI_H__ */