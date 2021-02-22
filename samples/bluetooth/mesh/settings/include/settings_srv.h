#include <bluetooth/mesh/model_types.h>
#include "settings_mod.h"

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

#ifdef __cplusplus
extern "C" {
#endif

// Node composition data - The node composition data is passed to bt_mesh_init()

extern const struct bt_mesh_model_op _bt_mesh_settings_srv_op[];
extern const struct bt_mesh_model_cb _bt_mesh_settings_srv_cb;

// DO: Define bt_mesh_settings_srv ...

#define BT_MESH_MODEL_SETTINGS_SRV(_srv)	
    BT_MESH_MODEL_VND_CB(
        BT_MESH_NORDIC_SEMI_COMPANY_ID, BT_MESH_MODEL_ID_SETTINGS_SRV,
        _bt_mesh_settings_srv_op,
        NULL,
        BT_MESH_MODEL_USER_DATA(struct bt_mesh_settings_srv, _srv),
        &_bt_mesh_settings_srv_cb)


/* Message handlers */

static void handle_message_get(struct bt_mesh_model *model,
                                  struct bt_mesh_msg_ctx *ctx,
                                  struct net_buf_simple *buf);

static void handle_message_set(struct bt_mesh_model *model,
                                  struct bt_mesh_msg_ctx *ctx,
                                  struct net_buf_simple *buf);                                


/* GET AND SET TX POWER LEVEL ON LOCAL SERVER */ 
static void set_tx_power(uint8_t handle_type, uint16_t handle, int8_t txp_lvl):
static void get_tx_power(uint8_t handle_type, uint16_t handle, int8_t *txp_lvl);

};