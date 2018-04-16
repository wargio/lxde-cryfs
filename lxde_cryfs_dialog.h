#ifndef LXDE_CRYFS_DIALOG_H
#define LXDE_CRYFS_DIALOG_H
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <lxpanel/plugin.h>

gchar *lxde_cryfs_folder_dialog(const char* default_path);
void lxde_cryfs_error_dialog(GtkWidget *window, const char* error_message);

#endif