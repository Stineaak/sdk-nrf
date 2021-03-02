/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-BSD-5-Clause-Nordic
 */
#include <string.h>
#include <stdlib.h>
#include "model_utils.h"
#include <bluetooth/mesh/models.h>

#include "settings_cli.h"

/* Receiving messages */ 

// Message handlers: bt_mesh_model_op.func, used to process the received message
static void handle_status_message(struct bt_mesh_model *model,
                                  struct bt_mesh_msg_ctx *ctx,
                                  struct net_buf_simple *buf)
{
    // DO: Message handler code...
}

// OPcode list - defines a list of messages that the client will receive
const struct bt_mesh_model_op _bt_mesh_settings_cli_op[] = {
    { BT_MESH_DEVICE_SETTINGS_STATUS_OP,    BT_MESH_DEVICE_SETTINGS_MSG_MINLEN_STATUS,
		handle_status_message },
    BT_MESH_MODEL_OP_END,
};

/* Sending messages */

static int send_get_message(struct bt_mesh_settings_cli *cli,
			  struct bt_mesh_msg_ctx *ctx,
			  struct bt_mesh_settings_status *rsp) // (struct bt_mesh_model *model, uint16_t addr)
{
    // DO: Check on specific node? :
	
	/* If you want your model to control a destination address,
     or some other parameters of a message,
     you can initialize bt_mesh_msg_ctx with custom parameters: */
    /*struct bt_mesh_msg_ctx ctx = {
            .addr = addr,
            .app_idx = model->keys[0],
            .send_ttl = BT_MESH_TTL_DEFAULT,
    };*/

    // prepare a buffer that will contain the message data together with the opcode:
    BT_MESH_MODEL_BUF_DEFINE(msg, 
        BT_MESH_DEVICE_SETTINGS_GET_OP,
        BT_MESH_DEVICE_SETTINGS_MSG_LEN_GET);
    // set the opcode of the message:
    bt_mesh_model_msg_init(&msg, BT_MESH_DEVICE_SETTINGS_GET_OP);

	return model_ackd_send(cli->model, ctx, &msg,
			       rsp ? &cli->ack_ctx : NULL,
			       BT_MESH_DEVICE_SETTINGS_STATUS_OP, rsp);
    //return bt_mesh_model_send(cli->model, &ctx, &buf, NULL, NULL);
};

// Initialize the publication buffer for SET message
static struct bt_mesh_model_pub pub_ctx = {
    .msg = NET_BUF_SIMPLE(BT_MESH_MODEL_BUF_LEN(BT_MESH_DEVICE_SETTINGS_SET_OP,
                                                BT_MESH_DEVICE_SETTINGS_MSG_MAXLEN_SET)),
}

//static int update_handler(struct bt_mesh_model *model) // DO: complete

static int send_set_message(struct bt_mesh_settings_cli *cli,
			  struct bt_mesh_msg_ctx *ctx,
			  const struct bt_mesh_settings_set *set,
			  struct bt_mesh_settings_status *rsp)
{

    // prepare a buffer that will contain the message data together with the opcode:
    BT_MESH_MODEL_BUF_DEFINE(msg, 
        BT_MESH_DEVICE_SETTINGS_SET_OP,
        BT_MESH_DEVICE_SETTINGS_MSG_MINLEN_SET);
    
	// set the opcode of the message:
    bt_mesh_model_msg_init(&msg, BT_MESH_DEVICE_SETTINGS_SET_OP);

	// add to buffer:
	net_buf_simple_add_u8(&msg, set->txp_value);

    return model_ackd_send(cli->model, ctx, &msg,
			       rsp ? &cli->ack_ctx : NULL,
			       BT_MESH_DEVICE_SETTINGS_STATUS_OP, rsp);
};


/* Callbacks: -> Additional model initialization */
static int bt_mesh_settings_cli_init(struct bt_mesh_model *model)
{
	struct bt_mesh_settings_cli *cli = model->user_data;

	cli->model = model;
	net_buf_simple_init(cli->pub.msg, 0);
	model_ack_init(&cli->ack_ctx);

	return 0;
}

const struct bt_mesh_model_cb _bt_mesh_settings_cli_cb = {
	.init = bt_mesh_settings_cli_init,
};



/* ONOFF CLIENT:

int bt_mesh_onoff_cli_get(struct bt_mesh_onoff_cli *cli,
			  struct bt_mesh_msg_ctx *ctx,
			  struct bt_mesh_onoff_status *rsp)
{
	BT_MESH_MODEL_BUF_DEFINE(msg, BT_MESH_ONOFF_OP_GET,
				 BT_MESH_ONOFF_MSG_LEN_GET);
	bt_mesh_model_msg_init(&msg, BT_MESH_ONOFF_OP_GET);

	return model_ackd_send(cli->model, ctx, &msg,
			       rsp ? &cli->ack_ctx : NULL,
			       BT_MESH_ONOFF_OP_STATUS, rsp);
}

int bt_mesh_onoff_cli_set(struct bt_mesh_onoff_cli *cli,
			  struct bt_mesh_msg_ctx *ctx,
			  const struct bt_mesh_onoff_set *set,
			  struct bt_mesh_onoff_status *rsp)
{
	BT_MESH_MODEL_BUF_DEFINE(msg, BT_MESH_ONOFF_OP_SET,
				 BT_MESH_ONOFF_MSG_MAXLEN_SET);
	bt_mesh_model_msg_init(&msg, BT_MESH_ONOFF_OP_SET);

	net_buf_simple_add_u8(&msg, set->on_off);
	net_buf_simple_add_u8(&msg, cli->tid++);
	if (set->transition) {
		model_transition_buf_add(&msg, set->transition);
	}

	return model_ackd_send(cli->model, ctx, &msg,
			       rsp ? &cli->ack_ctx : NULL,
			       BT_MESH_ONOFF_OP_STATUS, rsp);
}

*/