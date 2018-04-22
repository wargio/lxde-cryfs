#include "lxde_cryfs_menu.h"
#include "lxde_cryfs_mount.h"
#include "lxde_cryfs_config.h"
#include "lxde_cryfs_dialog.h"
#include "lxde_cryfs_exec.h"

GtkWidget *lxde_cryfs_password_window (lxde_cryfs_settings_t *settings, const char* base, const char* mount, int index);

typedef struct {
	int index;
	GtkWidget* item;
	GtkWidget* mount;
	GtkWidget* umount;
	lxde_cryfs_plugin_t* plugin;
} mounted_t;

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

void on_mount_pressed(GtkWidget *item, gpointer userdata) {
	(void)item;
	lxde_cryfs_plugin_t* plugin = (lxde_cryfs_plugin_t*) userdata;
	GtkWidget *window = lxde_cryfs_mount_window (&plugin->settings);
	gtk_widget_show_all (window);
}

void on_mount_pressed_info(GtkWidget *item, gpointer userdata) {
	(void)item;
	const gchar* basedir = 0;
	const gchar* mountpoint = 0;
	int active = FALSE;
	mounted_t* m = (mounted_t*) userdata;

	if (!lxde_cryfs_info_fs (&m->plugin->settings, &basedir, &mountpoint, &active, m->index)) {
		lxde_cryfs_error_dialog (0, "Unable to mount FS.");
	} else {
		GtkWidget *window = lxde_cryfs_password_window (&m->plugin->settings, basedir, mountpoint, m->index);
		gtk_widget_show_all (window);
	}
}

void on_umount_pressed(GtkWidget *item, gpointer userdata) {
	(void)item;
	const gchar* basedir = 0;
	const gchar* mountpoint = 0;
	int active = FALSE;
	mounted_t* m = (mounted_t*) userdata;
	if (!lxde_cryfs_info_fs (&m->plugin->settings, &basedir, &mountpoint, &active, m->index) || !lxde_cryfs_exec_umount (mountpoint)) {
		lxde_cryfs_set_active (&m->plugin->settings, FALSE, m->index);
		lxde_cryfs_info_dialog (0, "Umounted.");
	} else {
		lxde_cryfs_error_dialog (0, "Unable to mount FS.");
	}
	//lxde_cryfs_remove_fs (&m->plugin->settings, m->index);
}

void on_remove_pressed(GtkWidget *item, gpointer userdata) {
	(void)item;
	mounted_t* m = (mounted_t*) userdata;
	lxde_cryfs_remove_fs (&m->plugin->settings, m->index);
}

GtkWidget* add_mount_umount_menu(GtkWidget* item, mounted_t* m) {
	if (!item) {
		return 0;
	}
	GtkWidget* submenu = gtk_menu_new ();	
	add_submenu (item, submenu);
	if (lxde_cryfs_get_active (&m->plugin->settings, m->index)) {
		//enable
		add_menu_item (submenu, "Umount", G_CALLBACK(on_umount_pressed), (gpointer) m);
	} else {
		//disable
		add_menu_item (submenu, "Mount", G_CALLBACK(on_mount_pressed_info), (gpointer) m);
	}
	add_menu_separator (submenu);
	add_menu_item (submenu, "Remove this entry.", G_CALLBACK(on_remove_pressed), (gpointer) m);
	return submenu;
}

void free_item(GtkWidget* item, gpointer userdata) {
	gtk_widget_destroy (item);
}

void lxde_cryfs_mouse_menu (lxde_cryfs_plugin_t* plugin) {
	if (!plugin->menu) {		
		GtkWidget *menu = gtk_menu_new ();
		plugin->menu = menu;
	} else {
        gtk_container_foreach (GTK_CONTAINER(plugin->menu), (GtkCallback)free_item, NULL);
	}

	add_menu_item (plugin->menu, "Mount new CryFS..", G_CALLBACK(on_mount_pressed), (gpointer) plugin);
	int max = 0;
	if (config_setting_lookup_int (plugin->settings.config, LXDE_CRYFS_SETTING_LAST_INDEX, &max) == CONFIG_TRUE && max > 0) {
		add_menu_separator (plugin->menu);
		for (int i = 0; i < max; ++i) {
			const char* name = lxde_cryfs_get_name (&plugin->settings, i);
			if (!name) {
				continue;
			}
			mounted_t* m = g_new0 (mounted_t, 1);
			if (!name) {
				continue;
			}
			m->index = i;
			m->plugin = plugin;
			gchar* item_name = g_strdup_printf("fs: %s", name);
			GtkWidget* item = add_menu_item (plugin->menu, item_name, /*G_CALLBACK(on_umount_pressed), (gpointer) m*/0 ,0);
			add_mount_umount_menu (item, m);
			g_free (item_name);
			m->item = item;
			//add_mount_umount (item);
		}
	}
}