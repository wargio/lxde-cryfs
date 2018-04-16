#include "lxde_cryfs_menu.h"
#include "lxde_cryfs_mount.h"
#include "lxde_cryfs_config.h"


void add_menu_separator(GtkWidget *menu){
	if (!menu) {
		return;
	}
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), gtk_separator_menu_item_new());
}

GtkWidget* add_menu_item(GtkWidget *menu, const char* name, GCallback callback, gpointer userdata) {
	if (!menu || !name) {
		return 0;
	}
	GtkWidget* item = gtk_menu_item_new_with_label (name);
	gtk_menu_shell_append (GTK_MENU_SHELL(menu), item);
	if (callback) {
		g_signal_connect (item, "activate", callback, userdata);
	}
	return item;
}

void add_submenu(GtkWidget* item, GtkWidget* submenu) {
	if (!item || !submenu) {
		return;
	}
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(item), submenu);
}

void add_mount_umount(GtkWidget* item) {
	if (!item) {
		return;
	}
	GtkWidget* submenu = 0;	
}

void on_mount_pressed(GtkWidget *item, gpointer userdata) {
	(void)item;
	lxde_cryfs_plugin_t* plugin = (lxde_cryfs_plugin_t*) userdata;
	GtkWidget *window = lxde_cryfs_mount_window (&plugin->settings);
	gtk_widget_show_all(window);
}

GtkWidget* lxde_cryfs_mouse_menu (lxde_cryfs_plugin_t* plugin) {
	GtkWidget *menu = gtk_menu_new ();
	add_menu_item (menu, "Mount..", (GCallback) on_mount_pressed, (gpointer) plugin);
	int max = 0;
	if (!config_setting_lookup_int (plugin->settings.config, LXDE_CRYFS_SETTING_LAST_INDEX, &max) && max > 0) {
		add_menu_separator (menu);
		for (int i = 0; i < max; ++i) {
			GtkWidget* item = add_menu_item (menu, lxde_cryfs_get_name (&plugin->settings, i), 0, 0);
			//add_mount_umount (item);
		}
	}
	return menu;
}