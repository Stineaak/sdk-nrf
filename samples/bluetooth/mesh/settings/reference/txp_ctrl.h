#include <bluetooth/mesh/model_types.h>

#define BT_MESH_TX_PWR_CTRL_MSG_LEN_GET 0
#define BT_MESH_TX_PWR_CTRL_MSG_MINLEN_SET 2
#define BT_MESH_TX_PWR_CTRL_MSG_MAXLEN_SET 4
#define BT_MESH_TX_PWR_CTRL_MSG_MINLEN_STATUS 2
#define BT_MESH_TX_PWR_CTRL_MSG_MAXLEN_STATUS 4
// check values! 

/* Copied from hci_vs.h (lines 156 to 185)*/
#define BT_HCI_VS_LL_HANDLE_TYPE_ADV       0x00
#define BT_HCI_VS_LL_HANDLE_TYPE_SCAN      0x01
#define BT_HCI_VS_LL_HANDLE_TYPE_CONN      0x02
#define BT_HCI_VS_LL_TX_POWER_LEVEL_NO_PREF     0x7F
#define BT_HCI_OP_VS_WRITE_TX_POWER_LEVEL       BT_OP(BT_OGF_VS, 0x000e)
struct bt_hci_cp_vs_write_tx_power_level {
	uint8_t  handle_type;
	uint16_t handle;
	int8_t  tx_power_level;
} __packed;

struct bt_hci_rp_vs_write_tx_power_level {
	uint8_t  status;
	uint8_t  handle_type;
	uint16_t handle;
	int8_t  selected_tx_power;
} __packed;

#define BT_HCI_OP_VS_READ_TX_POWER_LEVEL        BT_OP(BT_OGF_VS, 0x000f)
struct bt_hci_cp_vs_read_tx_power_level {
	uint8_t  handle_type;
	uint16_t handle;
} __packed;

struct bt_hci_rp_vs_read_tx_power_level {
	uint8_t  status;
	uint8_t  handle_type;
	uint16_t handle;
	int8_t  tx_power_level;
} __packed;


/*MESSAGE TYPES*/

/** Mandatory parameters for the Tx Power Control Set message. */
struct bt_mesh_txp_ctrl_set {
    /** State to set. Integer from 0 to 100 */
	int txp; 
	/** Transition parameters. */
    const struct bt_mesh_model_transition *transition;
};

/** Parameters for the Tx Power Control Status message. */
struct bt_mesh_txp_ctrl_status {
	/** The present value of the Tx Power Control state. */
	bool present_txp;
	/** The target value of the Tx Power Control state (optional). */
	bool target_txp;
	/** Remaining time value in milliseconds. */
	int32_t remaining_time;
};