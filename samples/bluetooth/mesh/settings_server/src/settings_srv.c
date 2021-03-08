#include <string.h>
#include "settings_srv.h"
#include "model_utils.h"
#include <bluetooth/mesh/models.h>

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


/* Receiving messages - Message handlers: bt_mesh_model_op.func, used to process the received message
*/

static void handle_get(struct bt_mesh_model *model,
                                  struct bt_mesh_msg_ctx *ctx,
                                  struct net_buf_simple *buf)
{
    if (buf->len != BT_MESH_DEVICE_SETTINGS_MSG_LEN_GET) {
		return;
	}

	struct bt_mesh_settings_srv *srv = model->user_data;
	struct bt_mesh_settings_status status = { 0 };

	// run handler
	srv->handlers->get(srv, ctx, &status);

    rsp_status(model, &ctx, &buf);
}

// For unack, set ack = false
static void handle_set(struct bt_mesh_model *model,
                                  struct bt_mesh_msg_ctx *ctx,
                                  struct net_buf_simple *buf, bool ack)
{
    if (buf->len != BT_MESH_DEVICE_SETTINGS_MSG_MINLEN_SET &&
	    buf->len != BT_MESH_DEVICE_SETTINGS_MSG_MAXLEN_SET) {
		return;
	}

	struct bt_mesh_settings_srv *srv = model->user_data;
	struct bt_mesh_settings_status status = { 0 };
	struct bt_mesh_model_transition transition;
	struct bt_mesh_settings_set set;

	// extract set message
	int8_t txp = net_buf_simple_pull_u8(buf);
	uint8_t tid = net_buf_simple_pull_u8(buf);

	set.txp_value = txp;

	if (tid_check_and_update(&srv->prev_transaction, tid, ctx) != 0) {
		/* If this is the same transaction, we don't need to send it
		 * to the app, but we still have to respond with a status.
		 */
		srv->handlers->get(srv, NULL, &status);
		goto respond;
	}

	if (buf->len == 2) {
		model_transition_buf_pull(buf, &transition);
	} else {
		bt_mesh_dtt_srv_transition_get(srv->model, &transition);
	}

	set.transition = &transition;

	// run handler
	srv->handlers->set(srv, ctx, &set, &status);

	(void)bt_mesh_onoff_srv_pub(srv, NULL, &status);

respond:
	if (ack) {
		rsp_status(model, ctx, &status);
	}
}


/* OP CODE LIST: defines a list of messages that the server will receive */
const struct bt_mesh_model_op _bt_mesh_settings_srv_op[] = {
    { BT_MESH_DEVICE_SETTINGS_GET_OP,    BT_MESH_DEVICE_SETTINGS_MSG_LEN_GET,    handle_get },
    { BT_MESH_DEVICE_SETTINGS_SET_OP,    BT_MESH_DEVICE_SETTINGS_MSG_MINLEN_SET,    handle_set },
    BT_MESH_MODEL_OP_END,
}

/* Sending messages */

static void encode_status(struct net_buf_simple *buf,
			  const struct bt_mesh_settings_status *status)
{
	bt_mesh_model_msg_init(buf, BT_MESH_DEVICE_SETTINGS_STATUS_OP);
	net_buf_simple_add_u8(buf, status->txp_present);

	if (status->remaining_time != 0) {
		net_buf_simple_add_u8(buf, status->txp_target);
		net_buf_simple_add_u8(
			buf, model_transition_encode(status->remaining_time));
	}
};

static int rsp_status(struct bt_mesh_model *model,
		       struct bt_mesh_msg_ctx *ctx,
		       const struct bt_mesh_settings_status *status)
{
    // prepare a buffer that will contain the message data together with the opcode:
    BT_MESH_MODEL_BUF_DEFINE(buf, 
        BT_MESH_DEVICE_SETTINGS_STATUS_OP,
        BT_MESH_DEVICE_SETTINGS_MSG_LEN_STATUS);
    // set the opcode of the message:
    bt_mesh_model_msg_init(&buf, BT_MESH_DEVICE_SETTINGS_STATUS_OP);

    encode_status(&buf, status);

    return bt_mesh_model_send(model, &ctx, &buf, NULL, NULL);
};

/* Callbacks: -> Additional model initialization */
static int bt_mesh_settings_srv_init(struct bt_mesh_model *model)
{
	struct bt_mesh_settings_srv *srv = model->user_data;

	srv->model = model;
	net_buf_simple_init(model->pub->msg, 0);

	if (IS_ENABLED(CONFIG_BT_SETTINGS)) { // enable persistent storage
		// DO: (?) research...
	}

	return 0;
}

const struct bt_mesh_model_cb _bt_mesh_settings_srv_cb = {
	.init = bt_mesh_settings_srv_init,
};

// TX POWER
/** Functions form Bluetooth: HCI Power Control
Source: zephyr/samples/bluetooth/hci_pwr_ctrl/src/main.c

Note: stop advertising before changing the transmit power then restart advertising
*/

static void set_tx_power(uint8_t handle_type, uint16_t handle, uint8_t txp_lvl)
{
	struct bt_hci_cp_vs_write_tx_power_level *cp; // command power (?)
	struct bt_hci_rp_vs_write_tx_power_level *rp; // real power (?)
	struct net_buf *buf, *rsp = NULL;
	int err;

	// Allocate a HCI command buffer
	buf = bt_hci_cmd_create(BT_HCI_OP_VS_WRITE_TX_POWER_LEVEL,
				sizeof(*cp));
	if (!buf) {
		printk("Unable to allocate command buffer\n");
		return;
	}

	cp = net_buf_add(buf, sizeof(*cp));
	cp->handle = sys_cpu_to_le16(handle);
	cp->handle_type = handle_type;
	cp->tx_power_level = txp_lvl;

	//Send a HCI command synchronously
	err = bt_hci_cmd_send_sync(BT_HCI_OP_VS_WRITE_TX_POWER_LEVEL,
				   buf, &rsp);
	if (err) {
		uint8_t reason = rsp ?
			((struct bt_hci_rp_vs_write_tx_power_level *)
			  rsp->data)->status : 0;
		printk("Set Tx power err: %d reason 0x%02x\n", err, reason);
		return;
	}

	rp = (void *)rsp->data;
	printk("Actual Tx Power: %d\n", rp->selected_tx_power);

	net_buf_unref(rsp);
}


static void get_tx_power(uint8_t handle_type, uint16_t handle, uint8_t *txp_lvl)
{
	struct bt_hci_cp_vs_read_tx_power_level *cp;
	struct bt_hci_rp_vs_read_tx_power_level *rp;
	struct net_buf *buf, *rsp = NULL;
	int err;

	*txp_lvl = 0xFF;
	buf = bt_hci_cmd_create(BT_HCI_OP_VS_READ_TX_POWER_LEVEL,
				sizeof(*cp));
	if (!buf) {
		printk("Unable to allocate command buffer\n");
		return;
	}

	cp = net_buf_add(buf, sizeof(*cp));
	cp->handle = sys_cpu_to_le16(handle);
	cp->handle_type = handle_type;

	err = bt_hci_cmd_send_sync(BT_HCI_OP_VS_READ_TX_POWER_LEVEL,
				   buf, &rsp);
	if (err) {
		uint8_t reason = rsp ?
			((struct bt_hci_rp_vs_read_tx_power_level *)
			  rsp->data)->status : 0;
		printk("Read Tx power err: %d reason 0x%02x\n", err, reason);
		return;
	}

	rp = (void *)rsp->data;
	*txp_lvl = rp->tx_power_level;

	net_buf_unref(rsp);
}

static void read_conn_rssi(uint16_t handle, int8_t *rssi)
{
	struct net_buf *buf, *rsp = NULL;
	struct bt_hci_cp_read_rssi *cp;
	struct bt_hci_rp_read_rssi *rp;

	int err;

	buf = bt_hci_cmd_create(BT_HCI_OP_READ_RSSI, sizeof(*cp));
	if (!buf) {
		printk("Unable to allocate command buffer\n");
		return;
	}

	cp = net_buf_add(buf, sizeof(*cp));
	cp->handle = sys_cpu_to_le16(handle);

	err = bt_hci_cmd_send_sync(BT_HCI_OP_READ_RSSI, buf, &rsp);
	if (err) {
		uint8_t reason = rsp ?
			((struct bt_hci_rp_read_rssi *)rsp->data)->status : 0;
		printk("Read RSSI err: %d reason 0x%02x\n", err, reason);
		return;
	}

	rp = (void *)rsp->data;
	*rssi = rp->rssi;

	net_buf_unref(rsp);
}