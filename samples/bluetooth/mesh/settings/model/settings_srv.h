/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-BSD-5-Clause-Nordic
 */

/** @file
 *  @defgroup bt_mesh_settings_srv Device Settings Server model
 *  @{
 *  @brief API for the Device Settings Server model.
 */

#ifndef BT_MESH_SETTINGS_SRV_H__
#define BT_MESH_SETTINGS_SRV_H__

#include <bluetooth/mesh/model_types.h>
#include "settings_mod.h"

#ifdef __cplusplus
extern "C" {
#endif

/*Node composition data */  // The node composition data is passed to bt_mesh_init()

extern const struct bt_mesh_model_op _bt_mesh_settings_srv_op[];
extern const struct bt_mesh_model_cb _bt_mesh_settings_srv_cb;
// add update handler?

struct bt_mesh_settings_srv;

/** @def BT_MESH_SETTINGS_SRV_INIT
 *
 * @brief Init parameters for a @ref bt_mesh_settings_srv instance.
 *
 * @param[in] _handlers State access handlers to use in the model instance.
 */
#define BT_MESH_SETTINGS_SRV_INIT(_handlers)                                      \
	{                                                                      \
		.handlers = _handlers,                                         \
		.pub = {                                                       \
			.msg = NET_BUF_SIMPLE(BT_MESH_MODEL_BUF_LEN(           \
				BT_MESH_SETTINGS_OP_STATUS,                       \
				BT_MESH_SETTINGS_MSG_LEN_STATUS)),             \
		},                                                             \
	}

/** @def BT_MESH_MODEL_SETTINGS_SRV
 *
 * @brief Device Settings Server model composition data entry.
 *
 * @param[in] _srv Pointer to a @ref bt_mesh_settings_srv instance.
 */
#define BT_MESH_MODEL_SETTINGS_SRV(_srv)                                       \
	BT_MESH_MODEL_VND_CB(                                                  \
		BT_MESH_NORDIC_SEMI_COMPANY_ID, BT_MESH_MODEL_ID_SETTINGS_SRV, \
		_bt_mesh_settings_srv_op, NULL,                        \
		BT_MESH_MODEL_USER_DATA(struct bt_mesh_settings_srv, _srv),    \
		&_bt_mesh_settings_srv_cb)
	
/**
 * Device Settings Server instance. Should primarily be initialized with the
 * @ref BT_MESH_SETTINGS_SRV_INIT macro.
 */
struct bt_mesh_settings_srv {
	/** Transaction ID tracker. */
	struct bt_mesh_tid_ctx prev_transaction;
	/** Handler function structure. */
	const struct bt_mesh_settings_srv_handlers *handlers;
	/** Access model pointer. */
	struct bt_mesh_model *model;
	/** Publish parameters. */
	struct bt_mesh_model_pub pub;
};

/** Device Settings Server state access handlers. */
struct bt_mesh_settings_srv_handlers {
	/** @brief Set the Settings state.
	 *
	 * @note This handler is mandatory.
	 *
	 * @param[in] srv Server instance to set the state of.
	 * @param[in] ctx Message context for the message that triggered the
	 * change, or NULL if the change is not coming from a message.
	 * @param[in] set Parameters of the state change.
	 * @param[out] rsp Response structure to be filled, or NULL if no
	 * response is required.
	 */
	void (*const set)(struct bt_mesh_settings_srv *srv,
			  struct bt_mesh_msg_ctx *ctx,
			  const struct bt_mesh_settings_set *set,
			  struct bt_mesh_settings_status *rsp);

	/** @brief Get the Settings state.
	 *
	 * @note This handler is mandatory.
	 *
	 * @param[in] srv Server instance to get the state of.
	 * @param[in] ctx Message context for the message that triggered the
	 * change, or NULL if the change is not coming from a message.
	 * @param[out] rsp Response structure to be filled.
	 */
	void (*const get)(struct bt_mesh_settings_srv *srv,
			  struct bt_mesh_msg_ctx *ctx,
			  struct bt_mesh_settings_status *rsp);
};

/* Message handlers */

static void handle_get_message(struct bt_mesh_model *model,
                                  struct bt_mesh_msg_ctx *ctx,
                                  struct net_buf_simple *buf);

static void handle_set_message(struct bt_mesh_model *model,
                                  struct bt_mesh_msg_ctx *ctx,
                                  struct net_buf_simple *buf);                                


/* GET AND SET TX POWER LEVEL ON LOCAL SERVER */ 
static void set_tx_power(uint8_t handle_type, uint16_t handle, uint8_t txp_lvl);
static void get_tx_power(uint8_t handle_type, uint16_t handle, uint8_t *txp_lvl);

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* BT_MESH_SETTINGS_SRV_H__ */