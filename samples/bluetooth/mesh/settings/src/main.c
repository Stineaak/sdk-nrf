/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-BSD-5-Clause-Nordic
 */

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

#include <bluetooth/mesh.h>
#include <bluetooth/mesh/models.h>
#include <bluetooth/mesh/dk_prov.h>
#include <dk_buttons_and_leds.h>

#define BT_MESH_NORDIC_SEMI_COMPANY_ID 0x0059

static struct bt_conn *default_conn;
static uint16_t default_conn_handle;

struct bt_mesh_TEST_set {
	/** State to set - based on Button ID. int */
	int val;
	/** Transition parameters. */
	const struct bt_mesh_model_transition *transition;
};

static const struct bt_data ad[] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
	BT_DATA_BYTES(BT_DATA_UUID16_ALL, BT_UUID_16_ENCODE(BT_UUID_HRS_VAL)),
};

#define DEVICE_NAME CONFIG_BT_DEVICE_NAME
#define DEVICE_NAME_LEN (sizeof(DEVICE_NAME) - 1)
#define DEVICE_BEACON_TXPOWER_NUM  8

//static struct k_thread pwr_thread_data;
//static K_THREAD_STACK_DEFINE(pwr_thread_stack, 320);

static const int8_t txp[DEVICE_BEACON_TXPOWER_NUM] = {4, 0, -3, -8,
						    -15, -18, -23, -30};
static const struct bt_le_adv_param *param =
	BT_LE_ADV_PARAM(BT_LE_ADV_OPT_CONNECTABLE | BT_LE_ADV_OPT_USE_NAME,
			0x0020, 0x0020, NULL);

/* ONOFF Server [Testing] */

/*
static void TEST_read_conn_rssi(uint16_t handle, int8_t *rssi)
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
} */

static void TEST_set_tx_power(uint8_t handle_type, uint16_t handle, uint8_t txp_lvl)
{
	struct bt_hci_cp_vs_write_tx_power_level *cp;
	struct bt_hci_rp_vs_write_tx_power_level *rp;
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
	cp->tx_power_level = txp_lvl;

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


static void TEST_get_tx_power(uint8_t handle_type, uint16_t handle, uint8_t *txp_lvl)
{
	struct bt_hci_cp_vs_read_tx_power_level *cp;
	struct bt_hci_rp_vs_read_tx_power_level *rp;
	struct net_buf *buf, *rsp = NULL;
	int err;

	*txp_lvl = 0xFF;
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
	*txp_lvl = rp->tx_power_level;

	net_buf_unref(rsp);
}

static void led_set(struct bt_mesh_onoff_srv *srv, struct bt_mesh_msg_ctx *ctx,
		    const struct bt_mesh_TEST_set *set,
		    struct bt_mesh_onoff_status *rsp)
{
	int8_t txp_get = 0xFF;
	int idx;

	//uint8_t bid = net_buf_simple_pull_u8(set);
	//printk("Button pressed: %d\n", bid);

	//printk("TX Power set testing...\n");
	if (set->val == 0){ // BUTTON 1
		idx = 1; // txp -> 0 dB
	}else if (set->val == 1){ // BUTTON 2
		idx = 3; // txp -> -8 dB
	}else{
		idx = 4; // -15 dB
	}
	printk("\n");
	printk("#Button id: %d\n\n", set->val);
	//int8_t rssi = 0xFF;
	
	if (!default_conn) {
		printk("Getting initial TX Power level....\n");
		TEST_get_tx_power(BT_HCI_VS_LL_HANDLE_TYPE_ADV,
				     0, &txp_get);
		printk("Current TX Power is %d", txp_get);
		printk(" dB\n");

		k_sleep(K_SECONDS(2));

		printk("Setting TX Power level to %d", txp[idx]);
		printk(" dB...\n");
		TEST_set_tx_power(BT_HCI_VS_LL_HANDLE_TYPE_ADV,
				     0, txp[idx]);

		k_sleep(K_SECONDS(2));

		TEST_get_tx_power(BT_HCI_VS_LL_HANDLE_TYPE_ADV,
				     0, &txp_get);
		printk("New TX Power level is set to %d", txp_get); 
		printk(" dB\n\n");
			/*
			TEST_read_conn_rssi(default_conn_handle, &rssi);
			printk("Connected (%d) - RSSI = %d\n",
			       default_conn_handle, rssi);
			*/
	}
}

static void led_get(struct bt_mesh_onoff_srv *srv, struct bt_mesh_msg_ctx *ctx,
		    struct bt_mesh_onoff_status *rsp)
{
	//printk("LED get\n");
}

static const struct bt_mesh_onoff_srv_handlers onoff_handlers = {
	.set = led_set,
	.get = led_get,
	//.set = handle_set_message,
	//.get = handle_get_message,
};

static struct bt_mesh_onoff_srv onoff_srv = BT_MESH_ONOFF_SRV_INIT(&onoff_handlers);


// ***********************************************

/** Configuration server definition */
static struct bt_mesh_cfg_srv cfg_srv = {
	.relay = IS_ENABLED(CONFIG_BT_MESH_RELAY),
	.beacon = BT_MESH_BEACON_ENABLED,
	.frnd = IS_ENABLED(CONFIG_BT_MESH_FRIEND),
	.gatt_proxy = IS_ENABLED(CONFIG_BT_MESH_GATT_PROXY),
	.default_ttl = 7,

	/* 3 transmissions with 20ms interval */
	.net_transmit = BT_MESH_TRANSMIT(2, 20),
	.relay_retransmit = BT_MESH_TRANSMIT(2, 20),
};

/* Set up a repeating delayed work to blink the DK's LEDs when attention is
 * requested.
 */
static struct k_delayed_work attention_blink_work;

static void attention_blink(struct k_work *work)
{
	static int idx;
	const uint8_t pattern[] = {
		BIT(0) | BIT(1),
		BIT(1) | BIT(2),
		BIT(2) | BIT(3),
		BIT(3) | BIT(0),
	};

	dk_set_leds(pattern[idx++ % ARRAY_SIZE(pattern)]);
	k_delayed_work_submit(&attention_blink_work, K_MSEC(30));
}

static void attention_on(struct bt_mesh_model *mod)
{
	k_delayed_work_submit(&attention_blink_work, K_NO_WAIT);
}

static void attention_off(struct bt_mesh_model *mod)
{
	k_delayed_work_cancel(&attention_blink_work);
	dk_set_leds(DK_NO_LEDS_MSK);
}

static const struct bt_mesh_health_srv_cb health_srv_cb = {
	.attn_on = attention_on,
	.attn_off = attention_off,
};

static struct bt_mesh_health_srv health_srv = {
	.cb = &health_srv_cb,
};

BT_MESH_HEALTH_PUB_DEFINE(health_pub, 0);

// ENDRES TIL KORREKT MODELL
static struct bt_mesh_elem elements[] = {
	BT_MESH_ELEM(1,
		     BT_MESH_MODEL_LIST(BT_MESH_MODEL_CFG_SRV(&cfg_srv),
					BT_MESH_MODEL_HEALTH_SRV(&health_srv,
								 &health_pub),
					BT_MESH_MODEL_ONOFF_SRV(&onoff_srv)), // BARE ENDRE DENNE LINJEN(E)
		     BT_MESH_MODEL_NONE),
	// BT_MESH_ELEM(
	// 	2, BT_MESH_MODEL_LIST(BT_MESH_MODEL_ONOFF_SRV(&onoff_srv)),
	// 	BT_MESH_MODEL_NONE),
	// BT_MESH_ELEM(
	// 	3, BT_MESH_MODEL_LIST(BT_MESH_MODEL_ONOFF_SRV(&onoff_srv)),
	// 	BT_MESH_MODEL_NONE),
	// BT_MESH_ELEM(
	// 	4, BT_MESH_MODEL_LIST(BT_MESH_MODEL_ONOFF_SRV(&onoff_srv)),
	// 	BT_MESH_MODEL_NONE),
};

static const struct bt_mesh_comp comp = {
	.cid = BT_MESH_NORDIC_SEMI_COMPANY_ID,
	.elem = elements,
	.elem_count = ARRAY_SIZE(elements),
};

const struct bt_mesh_comp *model_handler_init(void)
{
	k_delayed_work_init(&attention_blink_work, attention_blink);

	// INITIALIZATION IF NEEDED

	return &comp;
}

// *********************************************************************

static void bt_ready(int err)
{
	if (err) {
		printk("Bluetooth init failed (err %d)\n", err);
		return;
	}

	printk("Bluetooth initialized\n");

	/* Start advertising */ /*
	err = bt_le_adv_start(param, ad, ARRAY_SIZE(ad),
			      NULL, 0);
	if (err) {
		printk("Advertising failed to start (err %d)\n", err);
		return;
	}

	printk("Dynamic Tx power Beacon started\n");
	*/

	dk_leds_init();
	dk_buttons_init(NULL);

	err = bt_mesh_init(bt_mesh_dk_prov_init(), model_handler_init());
	if (err) {
		printk("Initializing mesh failed (err %d)\n", err);
		return;
	}

	if (IS_ENABLED(CONFIG_SETTINGS)) {
		settings_load();
	}

	/* This will be a no-op if settings_load() loaded provisioning info */
	bt_mesh_prov_enable(BT_MESH_PROV_ADV | BT_MESH_PROV_GATT);

	printk("Mesh initialized\n");
}

void main(void)
{
	int err;
	default_conn = NULL;

	printk("Initializing...\n");

	err = bt_enable(bt_ready);
	if (err) {
		printk("Bluetooth init failed (err %d)\n", err);
	}
}