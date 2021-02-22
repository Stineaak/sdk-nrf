
#include <bluetooth/mesh/model_types.h>
#include "settings_mod.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Node composition data */  // The node composition data is passed to bt_mesh_init()

extern const struct bt_mesh_model_op _bt_mesh_settings_cli_op[];
extern const struct bt_mesh_model_cb _bt_mesh_settings_cli_cb;
static struct bt_mesh_model_pub pub_ctx;

#define BT_MESH_MODEL_SETTINGS_CLI(_cli)	
    BT_MESH_MODEL_VND_CB(
        BT_MESH_NORDIC_SEMI_COMPANY_ID, BT_MESH_MODEL_ID_SETTINGS_CLI,
        _bt_mesh_settings_cli_op,
        &pub_ctx, // for publishing SET messages // &(_cli)->pub (?)
        BT_MESH_MODEL_USER_DATA(struct bt_mesh_settings_cli, _cli),
        &_bt_mesh_settings_cli_cb) // enable callbacks


/* Client Initialization  */

// DO: Define bt_mesh_settings_cli ...

#define BT_MESH_SETTINGS_CLI_INIT(_status_handler)                                \
	{                                                                      \
		.status_handler = _status_handler,                             \
		.pub = {.msg = NET_BUF_SIMPLE(BT_MESH_MODEL_BUF_LEN(           \
				BT_MESH_DEVICE_SETTINGS_SET_OP,                          \
				BT_MESH_DEVICE_SETTINGS_MSG_MAXLEN_SET)) }               \
	}


/* Message handlers */

static void handle_status_message(struct bt_mesh_model *model,
			  struct bt_mesh_msg_ctx *ctx,
			  struct net_buf_simple *buf);
