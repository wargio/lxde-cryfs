#ifndef LXDE_CRYFS_CONFIG_H
#define LXDE_CRYFS_CONFIG_H

#include "lxde_cryfs.h"
#ifndef CONFIG_TRUE
#define CONFIG_TRUE 1
#endif
#define LXDE_CRYFS_SETTING_DEFAULT_PATH "cryfs_default_path"
#define LXDE_CRYFS_SETTING_BLOCK_SIZE "cryfs_block_size"
#define LXDE_CRYFS_SETTING_UNMOUNT_IDLE "cryfs_unmount_idle"
#define LXDE_CRYFS_SETTING_LAST_INDEX "cryfs_last_index"
#define LXDE_CRYFS_SETTING_FS_NAME "cryfs_fs_%d"
#define LXDE_CRYFS_SETTING_FS_BASE "cryfs_fs_base%d"
#define LXDE_CRYFS_SETTING_FS_MOUNT "cryfs_fs_mount%d"
#define LXDE_CRYFS_SETTING_FS_ACTIVE "cryfs_fs_active%d"

const gchar* lxde_cryfs_get_name(lxde_cryfs_settings_t* settings, guint index);
void lxde_cryfs_add_fs(lxde_cryfs_settings_t* settings, const gchar* basedir, const gchar* mountpoint, int active);
int lxde_cryfs_info_fs(lxde_cryfs_settings_t* settings, const gchar** basedir, const gchar** mountpoint, int* active, guint index);
void lxde_cryfs_remove_fs(lxde_cryfs_settings_t* settings, guint index);
void lxde_cryfs_update_settings(lxde_cryfs_settings_t* settings);
void lxde_cryfs_load_default_settings(lxde_cryfs_settings_t *settings);
const char* lxde_cryfs_get_settings(lxde_cryfs_settings_t *settings, const gchar* setting_name);
void lxde_cryfs_set_active(lxde_cryfs_settings_t *settings, int active, int index);
int lxde_cryfs_get_active(lxde_cryfs_settings_t *settings, int index);

#endif