/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-BSD-5-Clause-Nordic
 */

/** @file
 *  @defgroup bt_mesh_settings_cli Device Settings Client model
 *  @{
 *  @brief API for the Device Settings Client model.
 */

#ifndef BT_MESH_SETTINGS_CLI_H__
#define BT_MESH_SETTINGS_CLI_H__

#include <bluetooth/mesh/model_types.h>
#include "settings_mod.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Node composition data */  // The node composition data is passed to bt_mesh_init()

extern const struct bt_mesh_model_op _bt_mesh_settings_cli_op[];
extern const struct bt_mesh_model_cb _bt_mesh_settings_cli_cb;
//static struct bt_mesh_model_pub pub_ctx;

struct bt_mesh_settings_cli;

/** @def BT_MESH_MODEL_ONOFF_CLI
 *
 * @brief Device Settings Client model composition data entry.
 *
 * @param[in] _cli Pointer to a @ref bt_mesh_settings_cli instance.
 */
#define BT_MESH_MODEL_SETTINGS_CLI(_cli)                                       \	
    BT_MESH_MODEL_VND_CB(                                                  \
        BT_MESH_NORDIC_SEMI_COMPANY_ID, BT_MESH_MODEL_ID_SETTINGS_CLI, \
        _bt_mesh_settings_cli_op, &(_cli)->pub,                         \
        BT_MESH_MODEL_USER_DATA(struct bt_mesh_settings_cli, _cli),    \
        &_bt_mesh_settings_cli_cb)


/** @def BT_MESH_SETTINGS_CLI_INIT
 *
 * @brief Initialization parameters for a @ref bt_mesh_settings_cli instance.
 *
 * @param[in] _status_handler Optional status message handler.
 */
#define BT_MESH_SETTINGS_CLI_INIT(_status_handler)                                \
	{                                                                      \
		.status_handler = _status_handler,                             \
		.pub = {.msg = NET_BUF_SIMPLE(BT_MESH_MODEL_BUF_LEN(           \
				BT_MESH_DEVICE_SETTINGS_SET_OP,                          \
				BT_MESH_DEVICE_SETTINGS_MSG_MAXLEN_SET)) }               \
	}

/**
 * Settings Client structure.
 *
 * Should be initialized with the @ref BT_MESH_SETTINGS_CLI_INIT macro.
 */
struct bt_mesh_settings_cli {
	/** @brief Settings status message handler.
	 *
	 * @param[in] cli Client that received the status message.
	 * @param[in] ctx Context of the incoming message.
	 * @param[in] status TXP Status of the Settings Server that
	 * published the message.
	 */
	void (*const status_handler)(struct bt_mesh_settings_cli *cli,
				     struct bt_mesh_msg_ctx *ctx,
				     const struct bt_mesh_settings_status *status);
	/** Current Transaction ID. */
	uint8_t tid;
	/** Response context for tracking acknowledged messages. */
	struct bt_mesh_model_ack_ctx ack_ctx;
	/** Publish parameters. */
	struct bt_mesh_model_pub pub;
	/** Access model pointer. */
	struct bt_mesh_model *model;
};


/* Message handlers */

static void handle_status_message(struct bt_mesh_model *model,
			  struct bt_mesh_msg_ctx *ctx,
			  struct net_buf_simple *buf);


#ifdef __cplusplus
}
#endif

/** @} */

#endif /* BT_MESH_SETTINGS_CLI_H__ */