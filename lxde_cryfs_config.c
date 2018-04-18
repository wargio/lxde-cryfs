#include "lxde_cryfs_config.h"
#include <glib.h>


gboolean is_numeric(const gchar* str) {
	if (!str) {
		return FALSE;
	}
	while (*str) {
		if (!g_ascii_isdigit (*str)) {
			return FALSE;
		}
		str++;
	}
	return TRUE;
}

gchar* fs_get_name(const gchar* path) {
	gchar* name = 0;
	const gchar* pos = path;
	while (*path) {
		if (*path == '/' && *(path + 1)) {
			pos = path + 1;
		}
		path++;
	}
	path--;
	name = g_strdup (pos);
	if (*path == '/') {
		name[path - pos] = 0;
	}
	return name;
}

void set_fs_at_index(lxde_cryfs_settings_t* settings, const gchar* name, const gchar* basedir, const gchar* mountpoint, int index) {
	gchar* setting_name = g_strdup_printf(LXDE_CRYFS_SETTING_FS_NAME, index);
	config_group_set_string (settings->config, setting_name, name);
	g_free (setting_name);
	setting_name = g_strdup_printf(LXDE_CRYFS_SETTING_FS_BASE, index);
	config_group_set_string (settings->config, setting_name, basedir);
	g_free (setting_name);
	setting_name = g_strdup_printf(LXDE_CRYFS_SETTING_FS_MOUNT, index);
	config_group_set_string (settings->config, setting_name, mountpoint);
	g_free (setting_name);
}

void set_fs_move_index(lxde_cryfs_settings_t* settings, int index) {
	gboolean fail = FALSE;
	int next = index + 1;
	const gchar* name;
	const gchar* basedir;
	const gchar* mountpoint;
	gchar* setting_name = g_strdup_printf(LXDE_CRYFS_SETTING_FS_NAME, next);
	if (config_setting_lookup_string (settings->config, setting_name, &name) != CONFIG_TRUE) {
		fail = TRUE;
	}
	g_free (setting_name);
	setting_name = g_strdup_printf(LXDE_CRYFS_SETTING_FS_BASE, next);
	if (config_setting_lookup_string (settings->config, setting_name, &basedir) != CONFIG_TRUE) {
		fail = TRUE;
	}
	g_free (setting_name);
	setting_name = g_strdup_printf(LXDE_CRYFS_SETTING_FS_MOUNT, next);
	if (config_setting_lookup_string (settings->config, setting_name, &mountpoint) != CONFIG_TRUE) {
		fail = TRUE;
	}
	g_free (setting_name);
	if (!fail) {
		set_fs_at_index (settings, name, basedir, mountpoint, index);
	}
}

void set_fs_remove_index(lxde_cryfs_settings_t* settings, int index) {
	gchar* setting_name = g_strdup_printf(LXDE_CRYFS_SETTING_FS_NAME, index);
	config_setting_remove (settings->config, setting_name);
	g_free (setting_name);
	setting_name = g_strdup_printf(LXDE_CRYFS_SETTING_FS_BASE, index);
	config_setting_remove (settings->config, setting_name);
	g_free (setting_name);
	setting_name = g_strdup_printf(LXDE_CRYFS_SETTING_FS_MOUNT, index);
	config_setting_remove (settings->config, setting_name);
	g_free (setting_name);
}

const gchar* lxde_cryfs_get_name(lxde_cryfs_settings_t* settings, guint index) {
	const gchar* name;
	gchar* setting_name = g_strdup_printf(LXDE_CRYFS_SETTING_FS_NAME, index);
	if (config_setting_lookup_string (settings->config, setting_name, &name) != CONFIG_TRUE) {
		g_free (setting_name);
		return 0;
	}
	g_free (setting_name);
	return name;
}

int lxde_cryfs_info_fs(lxde_cryfs_settings_t* settings, const gchar** basedir, const gchar** mountpoint, guint index) {
	gchar* setting_name = g_strdup_printf(LXDE_CRYFS_SETTING_FS_BASE, index);
	if (config_setting_lookup_string (settings->config, setting_name, basedir) != CONFIG_TRUE) {
		g_free (setting_name);
		return 0;
	}
	g_free (setting_name);
	setting_name = g_strdup_printf(LXDE_CRYFS_SETTING_FS_MOUNT, index);
	if (config_setting_lookup_string (settings->config, setting_name, mountpoint) != CONFIG_TRUE) {
		g_free (setting_name);
		return 0;
	}
	g_free (setting_name);
	return 1;
}

void lxde_cryfs_add_fs(lxde_cryfs_settings_t* settings, const gchar* basedir, const gchar* mountpoint) {
	int max = 0;
	if (config_setting_lookup_int (settings->config, LXDE_CRYFS_SETTING_LAST_INDEX, &max) != CONFIG_TRUE) {
		return;
	}
	gchar* name = fs_get_name (basedir);
	set_fs_at_index (settings, name, basedir, mountpoint, max);
	g_free (name);
	config_group_set_int (settings->config, LXDE_CRYFS_SETTING_LAST_INDEX, max + 1);
}

void lxde_cryfs_remove_fs(lxde_cryfs_settings_t* settings, guint index) {
	int max = 0;
	if (config_setting_lookup_int (settings->config, LXDE_CRYFS_SETTING_LAST_INDEX, &max) != CONFIG_TRUE || max < 1 || max <= index) {
		return;
	}
	max--;
	for (int i = index; i < max; ++i) {
		set_fs_move_index (settings, i);
	}
	set_fs_remove_index (settings, max + 1);
	config_group_set_int (settings->config, LXDE_CRYFS_SETTING_LAST_INDEX, max);
}

void lxde_cryfs_update_settings(lxde_cryfs_settings_t* settings) {
	if (settings->default_path) {
		config_group_set_string (settings->config, LXDE_CRYFS_SETTING_DEFAULT_PATH, settings->default_path);
		g_free (settings->default_path);
		settings->default_path = 0;
	}
	if (settings->blocksize && is_numeric (settings->blocksize)) {
		config_group_set_string (settings->config, LXDE_CRYFS_SETTING_BLOCK_SIZE, settings->blocksize);
		g_free (settings->blocksize);
		settings->blocksize = 0;
	}
	if (settings->unmount_idle && is_numeric (settings->unmount_idle)) {
		config_group_set_string (settings->config, LXDE_CRYFS_SETTING_UNMOUNT_IDLE, settings->unmount_idle);
		g_free (settings->unmount_idle);
		settings->unmount_idle = 0;
	}
}

void lxde_cryfs_load_default_settings(lxde_cryfs_settings_t *settings) {
	int num;
	const char *str;
	if (config_setting_lookup_string (settings->config, LXDE_CRYFS_SETTING_DEFAULT_PATH, &str) != CONFIG_TRUE) {
		config_group_set_string (settings->config, LXDE_CRYFS_SETTING_DEFAULT_PATH, "/");
	}

	if (config_setting_lookup_string (settings->config, LXDE_CRYFS_SETTING_BLOCK_SIZE, &str) != CONFIG_TRUE) {
		config_group_set_string (settings->config, LXDE_CRYFS_SETTING_BLOCK_SIZE, "32768");
	}

	if (config_setting_lookup_string (settings->config, LXDE_CRYFS_SETTING_UNMOUNT_IDLE, &str) != CONFIG_TRUE) {
		config_group_set_string (settings->config, LXDE_CRYFS_SETTING_UNMOUNT_IDLE, "");
	}

	if (config_setting_lookup_int (settings->config, LXDE_CRYFS_SETTING_LAST_INDEX, &num) != CONFIG_TRUE) {
		config_group_set_int (settings->config, LXDE_CRYFS_SETTING_LAST_INDEX, 0);
	}
}

const char* lxde_cryfs_get_settings(lxde_cryfs_settings_t *settings, const gchar* setting_name) {
	const char *str = 0;
	if (config_setting_lookup_string (settings->config, setting_name, &str) != CONFIG_TRUE) {
		// should never happen
		return 0;
	}
	return str;
}