#include <bluetooth/mesh/model_types.h>
#include "settings_mod.h"

#ifdef __cplusplus
extern "C" {
#endif

/*Node composition data */  // The node composition data is passed to bt_mesh_init()

extern const struct bt_mesh_model_op _bt_mesh_settings_srv_op[];
extern const struct bt_mesh_model_cb _bt_mesh_settings_srv_cb;

#define BT_MESH_MODEL_SETTINGS_SRV(_srv)	
    BT_MESH_MODEL_VND_CB(
        BT_MESH_NORDIC_SEMI_COMPANY_ID, BT_MESH_MODEL_ID_SETTINGS_SRV,
        _bt_mesh_settings_srv_op,
        NULL,
        BT_MESH_MODEL_USER_DATA(struct bt_mesh_settings_srv, _srv),
        &_bt_mesh_settings_srv_cb)


/* Client Initialization */

// DO: Define bt_mesh_settings_srv ...


/* Message handlers */

static void handle_get_message(struct bt_mesh_model *model,
                                  struct bt_mesh_msg_ctx *ctx,
                                  struct net_buf_simple *buf);

static void handle_set_message(struct bt_mesh_model *model,
                                  struct bt_mesh_msg_ctx *ctx,
                                  struct net_buf_simple *buf);                                


/* GET AND SET TX POWER LEVEL ON LOCAL SERVER */ 
static void set_tx_power(uint8_t handle_type, uint16_t handle, int8_t txp_lvl):
static void get_tx_power(uint8_t handle_type, uint16_t handle, int8_t *txp_lvl);

};