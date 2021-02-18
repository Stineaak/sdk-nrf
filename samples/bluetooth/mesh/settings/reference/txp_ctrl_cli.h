#include <bluetooth/mesh/txp_ctrl.h> // fix loactaion

/* INIT CLI */

/** Generic TX Power Control Client structure */
struct bt_mesh_txp_ctrl_cli {
	/** @brief TX power control status message handler.
	 *
	 * @param[in] cli Client that received the status message.
	 * @param[in] ctx Context of the incoming message.
	 * @param[in] status Status of the Server that
	 * published the message.
	 */
	void (*const status_handler)(struct bt_mesh_txp_ctrl_cli *cli,
				     struct bt_mesh_msg_ctx *ctx,
				     const struct bt_mesh_txp_ctrl_status *status);
	/** Current Transaction ID. */
	uint8_t tid;
	/** Response context for tracking acknowledged messages. */
	struct bt_mesh_model_ack_ctx ack_ctx;
	/** Publish parameters. */
	struct bt_mesh_model_pub pub;
	/** Access model pointer. */
	struct bt_mesh_model *model;
};

static int bt_mesh_txp_cli_init(struct bt_mesh_model *model);

/* SEND MESSAGE */

/** @brief Get the status of the bound srv.*/
int bt_mesh_txp_ctrl_cli_get(struct bt_mesh_txp_ctrl_cli *cli,
			  struct bt_mesh_msg_ctx *ctx,
			  struct bt_mesh_txp_ctrl_status *rsp);

/** @brief Set the state in the srv (optional unack: without requesting a response).*/
int bt_mesh_txp_ctrl_cli_set(struct bt_mesh_txp_ctrl_cli *cli,
			  struct bt_mesh_msg_ctx *ctx,
			  const struct bt_mesh_txp_ctrl_set *set,
			  struct bt_mesh_txp_ctrl_status *rsp); // remove for unack


/* STATUS HANDLER */


