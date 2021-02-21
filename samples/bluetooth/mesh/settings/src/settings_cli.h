
#include <bluetooth/mesh/model_types.h>
#include "settings_mod.h"

#ifdef __cplusplus
extern "C" {
#endif

// Node composition data - The node composition data is passed to bt_mesh_init()

extern const struct bt_mesh_model_op _bt_mesh_settings_cli_op[];
extern const struct bt_mesh_model_cb _bt_mesh_settings_cli_cb;
static struct bt_mesh_model_pub pub_ctx;

#define BT_MESH_MODEL_SETTINGS_CLI(_cli)	
    BT_MESH_MODEL_VND_CB(
        BT_MESH_NORDIC_SEMI_COMPANY_ID, BT_MESH_MODEL_ID_SETTINGS_MOD,
        _bt_mesh_settings_cli_op,
        &pub_ctx, // for publishing SET messages
        NULL,
        NULL)

