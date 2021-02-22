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

static void handle_get_message(struct bt_mesh_model *model,
                                  struct bt_mesh_msg_ctx *ctx,
                                  struct net_buf_simple *buf)
{
    if (buf->len != BT_MESH_DEVICE_SETTINGS_MSG_MINLEN_GET {
		return;
	}
    send_status_message(model, &ctx, &buf);

}

static void handle_set_message(struct bt_mesh_model *model,
                                  struct bt_mesh_msg_ctx *ctx,
                                  struct net_buf_simple *buf)
{
    if (buf->len < BT_MESH_DEVICE_SETTINGS_MSG_MINLEN_SET || 
		buf->len > BT_MESH_DEVICE_SETTINGS_MSG_MAXLEN_SET{
		return;
	}
    uint8_t txp_value = net_buf_simple_pull_u8(buf);
    set_tx_power(BT_HCI_VS_LL_HANDLE_TYPE_ADV, 0, txp_value); // DO: check handle type

    // DO: add ACK OPCODE to list...
    BT_MESH_MODEL_BUF_DEFINE(reply, MESSAGE_ACK_OPCODE, MESSAGE_ACK_LEN);
    bt_mesh_model_msg_init(&reply, MESSAGE_ACK_OPCODE);

    // DO: Fill the reply buffer here...

    (void) bt_mesh_model_send(model, ctx, &reply, NULL, NULL);
}

// OPcode list: defines a list of messages that the server will receive
const struct bt_mesh_model_op _bt_mesh_settings_srv_op[] = {
    { BT_MESH_DEVICE_SETTINGS_GET_OP,    BT_MESH_DEVICE_SETTINGS_MSG_MINLEN_GET,    handle_get_message },
    { BT_MESH_DEVICE_SETTINGS_SET_OP,    BT_MESH_DEVICE_SETTINGS_MSG_MINLEN_SET,    handle_set_message },
    BT_MESH_MODEL_OP_END,
}

/* Sending messages */

static int send_status_message(struct bt_mesh_model *model,
		       struct bt_mesh_msg_ctx *rx_ctx,
		       const struct bt_mesh_settings_status *status)
{
    // prepare a buffer that will contain the message data together with the opcode:
    BT_MESH_MODEL_BUF_DEFINE(buf, 
        BT_MESH_DEVICE_SETTINGS_STATUS_OP,
        BT_MESH_DEVICE_SETTINGS_MSG_MINLEN_STATUS);
    // set the opcode of the message:
    bt_mesh_model_msg_init(&buf, BT_MESH_DEVICE_SETTINGS_STATUS_OP);

    int8_t txp_get = 0xFF;
	get_tx_power(BT_HCI_VS_LL_HANDLE_TYPE_ADV, 0, &txp_get); // DO: check handle type
	net_buf_simple_add_u8(buf, txp_get);

    return bt_mesh_model_send(model, &ctx, &buf, NULL, NULL);
}

/* ONOFF SERVER:

static void rsp_status(struct bt_mesh_model *model,
		       struct bt_mesh_msg_ctx *rx_ctx,
		       const struct bt_mesh_onoff_status *status)
{
	BT_MESH_MODEL_BUF_DEFINE(msg, BT_MESH_ONOFF_OP_STATUS,
				 BT_MESH_ONOFF_MSG_MAXLEN_STATUS);
	encode_status(&msg, status);

	(void)bt_mesh_model_send(model, rx_ctx, &msg, NULL, NULL);
}

*/


/* Callbacks: -> Additional model initialization */
static int bt_mesh_settings_srv_init(struct bt_mesh_model *model)
{
	struct bt_mesh_settings_srv *srv = model->user_data;

	srv->model = model;
	net_buf_simple_init(model->pub->msg, 0);

	if (IS_ENABLED(CONFIG_BT_SETTINGS)) { 
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

static void set_tx_power(uint8_t handle_type, uint16_t handle, int8_t tx_pwr_lvl)
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
	cp->tx_power_level = tx_pwr_lvl;

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


static void get_tx_power(uint8_t handle_type, uint16_t handle, int8_t *tx_pwr_lvl)
{
	struct bt_hci_cp_vs_read_tx_power_level *cp;
	struct bt_hci_rp_vs_read_tx_power_level *rp;
	struct net_buf *buf, *rsp = NULL;
	int err;

	*tx_pwr_lvl = 0xFF;
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
	*tx_pwr_lvl = rp->tx_power_level;

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