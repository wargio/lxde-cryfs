#include "lxde_cryfs_plugin.h"
#include "lxde_cryfs_config.h"
#include "lxde_cryfs_menu.h"

static int m_plugin_instance = 0;

GtkWidget *lxde_cryfs_constructor(LXPanel *panel, config_setting_t *settings) {
	/* panel is a pointer to the panel and
		settings is a pointer to the configuration data
		since we don't use it, we'll make sure it doesn't
		give us an error at compilation time */
	(void)panel;


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

	// need to create a container to be able to set a border
	GtkWidget *p = gtk_event_box_new ();
	plugin->p = p;

	plugin->settings.default_path = 0;
	plugin->settings.blocksize = 0;
	plugin->settings.unmount_idle = 0;
	plugin->settings.config = settings;

	lxde_cryfs_load_default_settings (&plugin->settings);

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

gboolean lxde_cryfs_button_press_event(GtkWidget *widget, GdkEventButton *event, LXPanel *panel) {
	if (event->type == GDK_BUTTON_PRESS && event->button == EVENT_MOUSE_BUTTON_PRESS_RIGHT) {
		lxde_cryfs_plugin_t* plugin = (lxde_cryfs_plugin_t*) lxpanel_plugin_get_data(widget);
		GtkWidget* menu = lxde_cryfs_mouse_menu (plugin);
		gtk_widget_show_all(menu);
		gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL, NULL, (event != NULL) ? event->button : 0, gdk_event_get_time((GdkEvent*) event));
		gtk_widget_destroy(menu);
		return TRUE;
	}
	return FALSE;
}

static gboolean apply_configuration(gpointer userdata) {
	lxde_cryfs_settings_t *settings = (lxde_cryfs_settings_t*) userdata;
	lxde_cryfs_update_settings (settings);
	return TRUE;
}

static GtkWidget *lxde_cryfs_config(LXPanel *panel, GtkWidget *p) {
    lxde_cryfs_plugin_t *plugin = (lxde_cryfs_plugin_t*) lxpanel_plugin_get_data(p);
    return lxpanel_generic_config_dlg("CryFS", panel, apply_configuration, (gpointer) &plugin->settings,
            "Default Directory",          &plugin->settings.default_path, CONF_TYPE_DIRECTORY_ENTRY,
            "Block Size (default 32768)", &plugin->settings.blocksize, CONF_TYPE_STR,
            "Unmount Idle (minutes)",     &plugin->settings.unmount_idle, CONF_TYPE_STR,
            NULL);
}


FM_DEFINE_MODULE(lxpanel_gtk, lxde_cryfs)

/* Plugin descriptor. */
/* https://wiki.lxde.org/en/How_to_write_plugins_for_LXPanel */
LXPanelPluginInit fm_module_init_lxpanel_gtk = {
	.name = "CryFS Mount/Unmount",
	.description = "Handles CryFS Mountpoints",
	.config = lxde_cryfs_config,
	.new_instance = lxde_cryfs_constructor,
	.button_press_event = lxde_cryfs_button_press_event
};
