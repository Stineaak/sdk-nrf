
#include <bluetooth/bluetooth.h>
#include <bluetooth/mesh/models.h>
#include <bluetooth/mesh/dk_prov.h>
#include <dk_buttons_and_leds.h>
//#include "model_handler.h"

// Mesh Initialization
static void bt_ready(int err)
{
	if (err) {
		printk("Bluetooth init failed (err %d)\n", err);
		return;
	}

	printk("Bluetooth initialized\n");

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

	printk("Initializing...\n");

	/* Initialize the Bluetooth Subsystem */
	err = bt_enable(bt_ready);
        
       // Add tx gain settings
        static const uint8_t txp[] = {8, 4, 0, -4};
        uint8_t tx = txp[3];
	
        set_tx_power(BT_HCI_VS_LL_HANDLE_TYPE_CONN,
				     default_conn_handle,
				     BT_HCI_VS_LL_TX_POWER_LEVEL_NO_PREF);
					
		get_tx_power(BT_HCI_VS_LL_HANDLE_TYPE_CONN,
				     default_conn_handle, &txp);
	
        if (err) {
		printk("Bluetooth init failed (err %d)\n", err);
	}
}