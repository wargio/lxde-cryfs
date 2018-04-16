#include "lxde_cryfs_plugin.h"
#include "lxde_cryfs_mount.h"

static int m_plugin_instance = 0;

void add_menu_separator(lxde_cryfs_plugin_t *plugin){
	if (!plugin || !plugin->menu) {
		return;
	}
	gtk_menu_shell_append(GTK_MENU_SHELL(plugin->menu), gtk_separator_menu_item_new());
}

void add_menu_item(lxde_cryfs_plugin_t *plugin, const char* name, GCallback callback, gpointer userdata) {
	if (!plugin || !plugin->menu || !name) {
		return;
	}
	GtkWidget* item = gtk_menu_item_new_with_label (name);
	gtk_menu_shell_append (GTK_MENU_SHELL(plugin->menu), item);
	if (callback) {
		g_signal_connect (item, "activate", callback, userdata);
	}
}

void on_mount_pressed (GtkWidget *item, gpointer userdata) {
	(void)item;
	GtkWidget *window = lxde_cryfs_mount_window ();

	gtk_widget_show_all(window);
	g_print ("Mount me!!!\n");
}

GtkWidget *lxde_cyfs_constructor(LXPanel *panel, config_setting_t *settings) {
	/* panel is a pointer to the panel and
		settings is a pointer to the configuration data
		since we don't use it, we'll make sure it doesn't
		give us an error at compilation time */
	(void)panel;
	(void)settings;

	// allocate our private structure instance
	lxde_cryfs_plugin_t *plugin = g_new0 (lxde_cryfs_plugin_t, 1);

	// update the instance count
	plugin->id = ++m_plugin_instance;

#ifdef LXDE_CRYFS_PLUGIN_LABEL
	plugin->label = gtk_label_new ("CryFS");
#endif
	plugin->systray = gtk_image_new_from_stock (GTK_STOCK_HARDDISK, GTK_ICON_SIZE_SMALL_TOOLBAR);

	// set the systray to be visible
	gtk_widget_show (plugin->systray);

	plugin->menu = gtk_menu_new ();
	add_menu_item (plugin, "Mount..", (GCallback) on_mount_pressed, 0);
	add_menu_separator (plugin);
	add_menu_item (plugin, "FSNAME", (GCallback) 0, 0);

	// need to create a container to be able to set a border
	GtkWidget *p = gtk_event_box_new ();
	plugin->p = p;

	// our widget doesn't have a window...
	// it is usually illegal to call gtk_widget_set_has_window() from application but for GtkEventBox it doesn't hurt
	gtk_widget_set_has_window (p, FALSE);

	// bind private structure to the widget assuming it should be freed using g_free()
	lxpanel_plugin_set_data (p, plugin, g_free);

	// set border width
	gtk_container_set_border_width (GTK_CONTAINER(p), 1);

	// add the systray to the container
	gtk_container_add (GTK_CONTAINER(p), plugin->systray);
#ifdef LXDE_CRYFS_PLUGIN_LABEL
	gtk_container_add (GTK_CONTAINER(p), plugin->label);
#endif

	plugin->tooltip = gtk_tooltips_new();
	gtk_tooltips_set_tip (plugin->tooltip, p, "Handles CryFS Mountpoints", NULL);

	return p;
}


gboolean lxde_cyfs_button_press_event(GtkWidget *widget, GdkEventButton *event, LXPanel *panel) {
	if (event->type == GDK_2BUTTON_PRESS && event->button == EVENT_MOUSE_BUTTON_PRESS_RIGHT) {
		lxde_cryfs_plugin_t* plugin = (lxde_cryfs_plugin_t*) lxpanel_plugin_get_data(widget);
		gtk_widget_show_all(plugin->menu);
		gtk_menu_popup(GTK_MENU(plugin->menu), NULL, NULL, NULL, NULL, (event != NULL) ? event->button : 0, gdk_event_get_time((GdkEvent*) event));
		return TRUE;
	}
	return FALSE;
}

FM_DEFINE_MODULE(lxpanel_gtk, lxde_cryfs)

/* Plugin descriptor. */
/* https://wiki.lxde.org/en/How_to_write_plugins_for_LXPanel */
LXPanelPluginInit fm_module_init_lxpanel_gtk = {
	.name = "CryFS Mount/Unmount",
	.description = "Handles CryFS Mountpoints",
	.new_instance = lxde_cyfs_constructor,
	.button_press_event = lxde_cyfs_button_press_event
};
