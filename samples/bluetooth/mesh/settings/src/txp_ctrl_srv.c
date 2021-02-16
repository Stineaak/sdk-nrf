// #include <zephyr.h>
//#include <sys/printk.h>
#include "txp_ctrl_srv.h"

static void rsp_status(struct bt_mesh_model *model,
		       struct bt_mesh_msg_ctx *rx_ctx,
		       const struct bt_mesh_txp_ctrl_status *status)
{
	BT_MESH_MODEL_BUF_DEFINE(msg, BT_MESH_TX_PWR_CTRL_OP_STATUS,
				 BT_MESH_TX_PWR_CTRL_MSG_MAXLEN_STATUS);
	encode_status(&msg, status);

	(void)bt_mesh_model_send(model, rx_ctx, &msg, NULL, NULL);
}

//______________________________________________________
static void handle_set(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx,
		       struct net_buf_simple *buf)
{
	// extract from buffer...

	set_tx_power(uint8_t handle_type, uint16_t handle, int8_t tx_pwr_lvl); // DO: fix parameters...
}

//______________________________________________________
static void handle_get(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx,
		       struct net_buf_simple *buf)
{
	if (buf->len != BT_MESH_TX_PWR_CTRL_MSG_LEN_GET) {
		return;
	}

	//struct bt_mesh_txp_ctrl_srv *srv = model->user_data;
	struct bt_mesh_txp_ctrl_status status = { 0 };

	// Get status update
    get_tx_power(BT_HCI_VS_LL_HANDLE_TYPE_ADV, 0, &status->present_txp);

	// Send status response
	rsp_status(model, ctx, &status);
}

// TX POWER
/** Set and get functions form Bluetooth: HCI Power Control
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

//______________________________________________________
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

