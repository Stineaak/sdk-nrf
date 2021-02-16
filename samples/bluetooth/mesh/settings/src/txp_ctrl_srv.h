#include <zephyr/types.h>
#include <stddef.h>
#include <sys/printk.h>
#include <sys/util.h>
#include <sys/byteorder.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_vs.h>

#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>
#include <bluetooth/services/hrs.h>

#include "txp_ctrl.h"

/* INIT SRV */
/**
 * Generic Tx power control Server instance. Should primarily be initialized with the
 * @ref BT_MESH_TXP_CTRL_SRV_INIT macro.
 */
struct bt_mesh_txp_ctrl_srv {
	/** Transaction ID tracker. */
	struct bt_mesh_tid_ctx prev_transaction;
	/** Handler function structure. */
	const struct bt_mesh_txp_ctrl_srv_handlers *handlers; // check
	/** Access model pointer. */
	struct bt_mesh_model *model;
	/** Publish parameters. */
	struct bt_mesh_model_pub pub;
	/* Scene entry */
	//struct bt_mesh_scene_entry scene;
};

/* GET AND SET TX POWER LEVEL ON LOCAL SERVER */ 
static void set_tx_power(uint8_t handle_type, uint16_t handle, int8_t txp_lvl):
static void get_tx_power(uint8_t handle_type, uint16_t handle, int8_t *txp_lvl);
// Add helper functions from hci_pwr_ctrl.h /.c 

/* MESSAGE HANDLERS */
static void handle_set(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx,
		       struct net_buf_simple *buf);
static void handle_get(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx,
		       struct net_buf_simple *buf);

/* SEND RESPONSE MESSAGE */
static void rsp_status(struct bt_mesh_model *model,
		       struct bt_mesh_msg_ctx *rx_ctx,
		       const struct bt_mesh_txp_ctrl_status *status);