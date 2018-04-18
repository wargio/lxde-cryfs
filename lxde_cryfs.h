#ifndef LXDE_CRYFS_H
#define LXDE_CRYFS_H

#include <gtk/gtk.h>
#include <lxpanel/plugin.h>

typedef struct {
	gchar* default_path;
	// --blocksize
	gchar* blocksize;
	// --unmount-idle
	gchar* unmount_idle;
	config_setting_t *config;
} lxde_cryfs_settings_t;

typedef struct {
	int id;
	GtkWidget *p;
	GtkWidget *systray;
	GtkWidget* menu;
#ifdef LXDE_CRYFS_PLUGIN_LABEL
	GtkWidget *label;
#endif
	GtkTooltips *tooltip;
	lxde_cryfs_settings_t settings;
} lxde_cryfs_plugin_t;

#endif