// #include <zephyr.h>
// #include <sys/printk.h>

#include <string.h>
#include "model_utils.h"
#include "txp_ctrl_cli.h"
#include "txp_ctrl.h"

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

static void decode_status(struct net_buf_simple *buf,
			  struct bt_mesh_txp_ctrl_status *status)
{
	status->present_txp = net_buf_simple_pull_u8(buf);

	if (buf->len == 2) { // (?) Check len value
		status->target_txp = net_buf_simple_pull_u8(buf);
		status->remaining_time =
			model_transition_decode(net_buf_simple_pull_u8(buf));
	} else {
		status->target_txp = status->present_txp;
		status->remaining_time = 0;
	}
}

// STATUS HANDLER
static void handle_status(struct bt_mesh_model *model,
			  struct bt_mesh_msg_ctx *ctx,
			  struct net_buf_simple *buf)
{

	if (buf->len != BT_MESH_TX_PWR_CTRL_MSG_MINLEN_STATUS &&
	    buf->len != BT_MESH_TX_PWR_CTRL_MSG_MAXLEN_STATUS) { // check values
		return;
	}

	struct bt_mesh_txp_ctrl_cli *cli = model->user_data;
	struct bt_mesh_txp_ctrl_status status;

	decode_status(buf, &status);

	// not complete
}

static int bt_mesh_txp_cli_init(struct bt_mesh_model *model)
{
	struct bt_mesh_txp_cli *cli = model->user_data;

	cli->model = model;
	net_buf_simple_init(cli->pub.msg, 0);
	model_ack_init(&cli->ack_ctx);

	return 0;
}

const struct bt_mesh_model_cb _bt_mesh_txp_cli_cb = {
	.init = bt_mesh_txp_cli_init,
};

//______________________________________________________
int bt_mesh_txp_ctrl_cli_get(struct bt_mesh_txp_ctrl_cli *cli,
			  struct bt_mesh_msg_ctx *ctx,
			  struct bt_mesh_txp_ctrl_status *rsp)
{
	BT_MESH_MODEL_BUF_DEFINE(msg, BT_HCI_OP_VS_READ_TX_POWER_LEVEL,
				 BT_MESH_TX_PWR_CTRL_MSG_LEN_GET);
	bt_mesh_model_msg_init(&msg, BT_HCI_OP_VS_READ_TX_POWER_LEVEL);

	return model_ackd_send(cli->model, ctx, &msg,
			       rsp ? &cli->ack_ctx : NULL,
			       BT_MESH_TX_PWR_CTRL_OP_STATUS, rsp); // FIX: undefined macro
}

//________________________________________________________
int bt_mesh_txp_ctrl_cli_set(struct bt_mesh_txp_ctrl_cli *cli,
			  struct bt_mesh_msg_ctx *ctx,
			  const struct bt_mesh_txp_ctrl_set *set,
			  struct bt_mesh_txp_ctrl_status *rsp)
{
	BT_MESH_MODEL_BUF_DEFINE(msg, BT_HCI_OP_VS_WRITE_TX_POWER_LEVEL,
				 BT_MESH_TX_PWR_CTRL_MSG_MAXLEN_SET);
	bt_mesh_model_msg_init(&msg, BT_HCI_OP_VS_WRITE_TX_POWER_LEVEL);

	net_buf_simple_add_u8(&msg, set->txp);
	net_buf_simple_add_u8(&msg, cli->tid++);
	if (set->transition) {
		model_transition_buf_add(&msg, set->transition);
	}

	return model_ackd_send(cli->model, ctx, &msg,
			       rsp ? &cli->ack_ctx : NULL,
			       BT_MESH_TX_PWR_CTRL_OP_STATUS, rsp); // FIX: undefined macro
}





