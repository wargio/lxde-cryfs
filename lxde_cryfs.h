#ifndef LXDE_CRYFS_H
#define LXDE_CRYFS_H

#include <gtk/gtk.h>

typedef struct {
	int id;
	GtkWidget *p;
	GtkWidget *systray;
#ifdef LXDE_CRYFS_PLUGIN_LABEL
	GtkWidget *label;
#endif
	GtkTooltips *tooltip;
	GtkWidget *menu;
} lxde_cryfs_plugin_t;


#endif