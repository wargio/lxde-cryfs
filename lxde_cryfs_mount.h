#ifndef LXDE_CRYFS_MOUNT_H
#define LXDE_CRYFS_MOUNT_H
#include <gtk/gtk.h>
#include "lxde_cryfs.h"

GtkWidget *lxde_cryfs_mount_window (lxde_cryfs_settings_t *settings);
GtkWidget *lxde_cryfs_password_window (lxde_cryfs_settings_t *settings, const char* base, const char* mount, int index);

#endif