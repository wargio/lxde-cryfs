#include "lxde_cryfs.h"
#include "lxde_cryfs_mount.h"
#include "lxde_cryfs_dialog.h"
#include "lxde_cryfs_config.h"
#include "lxde_cryfs_exec.h"
#include <string.h>

#define LXDE_CRYFS_MOUNT_TITLE "CryFS - Mount"

// CRYFS_FRONTEND=noninteractive
typedef struct {
	GtkWidget *window;
	gchar* cipher;
	GtkWidget *basedir_label;
	GtkWidget *mountdir_label;
	GtkWidget *password_entry;
	lxde_cryfs_settings_t *settings;
} lxde_cryfs_mount_t;

void combo_selected(GtkWidget *widget, gpointer userdata) { 
	g_print("Changed");
	lxde_cryfs_mount_t* object = (lxde_cryfs_mount_t*) userdata;
	g_free (object->cipher);
	object->cipher = gtk_combo_box_get_active_text (GTK_COMBO_BOX(widget));
}


void basedir_button_click(GtkWidget *widget, gpointer userdata) {
	lxde_cryfs_mount_t* object = (lxde_cryfs_mount_t*) userdata;
	const gchar* default_path = lxde_cryfs_get_settings (object->settings, LXDE_CRYFS_SETTING_DEFAULT_PATH);
	gchar* path = lxde_cryfs_folder_dialog (default_path);
	if (path) {
		gtk_label_set_text (GTK_LABEL(object->basedir_label), path);
		g_free (path);
	}
}

void mountdir_button_click(GtkWidget *widget, gpointer userdata) {
	lxde_cryfs_mount_t* object = (lxde_cryfs_mount_t*) userdata;
	gchar* path = lxde_cryfs_folder_dialog (0);
	if (path) {
		gtk_label_set_text (GTK_LABEL(object->mountdir_label), path);
		g_free (path);
	}
}

void mount_filesystem(GtkWidget *widget, gpointer userdata) {
	lxde_cryfs_mount_t* object = (lxde_cryfs_mount_t*) userdata;
	const gchar* blocksize = lxde_cryfs_get_settings (object->settings, LXDE_CRYFS_SETTING_BLOCK_SIZE);
	const gchar* unmount_idle = lxde_cryfs_get_settings (object->settings, LXDE_CRYFS_SETTING_UNMOUNT_IDLE);

	if (gtk_entry_get_text_length (GTK_ENTRY(object->password_entry)) < 8) {
		lxde_cryfs_error_dialog (object->window, "Password cannot be less then 8 characters.");
		return;
	}
	const gchar* basedir  = gtk_label_get_text (GTK_LABEL(object->basedir_label));
	if (basedir[0] == '?' || strlen (basedir) < 1) {
		lxde_cryfs_error_dialog (object->window, basedir[0] == '?' ? "Base Directory is invalid." : "Base Directory cannot empty.");
		return;
	}
	const gchar* mountdir = gtk_label_get_text (GTK_LABEL(object->mountdir_label));
	if (mountdir[0] == '?' || strlen (mountdir) < 1) {
		lxde_cryfs_error_dialog (object->window, mountdir[0] == '?' ? "Mount Directory is invalid." : "Mount Directory cannot empty.");
		return;
	}
	const gchar* password = gtk_entry_get_text (GTK_ENTRY(object->password_entry));

	if (lxde_cryfs_mount (basedir, mountdir, object->cipher, password, blocksize, unmount_idle)) {
		lxde_cryfs_add_fs (object->settings, basedir, mountdir);
		lxde_cryfs_info_dialog (object->window, "Mounted.");
	} else {
		lxde_cryfs_error_dialog (object->window, "Mount failed.");
		return;
	}

	gtk_widget_destroy (GTK_WIDGET(object->window));
	g_free (object->cipher);
	g_free (object);
}

void close_window(GtkWidget *widget, gpointer userdata) {
	lxde_cryfs_mount_t* object = (lxde_cryfs_mount_t*) userdata;
	gtk_widget_destroy (GTK_WIDGET(object->window));
	g_free (object->cipher);
	g_free (object);
}

GtkWidget *lxde_cryfs_mount_window(lxde_cryfs_settings_t *settings) {
	GtkWidget *window;
	GtkWidget *mount_button;
	GtkWidget *cancel_button;

	GtkWidget *table;

	GtkWidget *basedir_button;
	GtkWidget *mountdir_button;
	GtkWidget *password_label;
	GtkWidget *cipher_label;

	GtkWidget *basedir_label;
	GtkWidget *mountdir_label;
	GtkWidget *password_entry;
	GtkWidget *cipher_combo;

	GtkWidget *vbox;
	GtkWidget *hbox;
	GtkWidget *halign;
	GtkWidget *valign;

	lxde_cryfs_mount_t *object = g_new0 (lxde_cryfs_mount_t, 1);
	object->settings = settings;

	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_position (GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_window_set_default_size (GTK_WINDOW(window), 350, 200);
	gtk_window_set_title (GTK_WINDOW(window), LXDE_CRYFS_MOUNT_TITLE);
	gtk_container_set_border_width (GTK_CONTAINER(window), 10);

	object->window = window;

	vbox = gtk_vbox_new (FALSE, 5);

	valign = gtk_alignment_new (0, 1, 0, 0);
	gtk_container_add (GTK_CONTAINER(vbox), valign);
	gtk_container_add (GTK_CONTAINER(window), vbox);

	table = gtk_table_new(4, 2, FALSE);
	gtk_container_add (GTK_CONTAINER(vbox), table);

	basedir_button = gtk_button_new_with_label ("Base Directory");
	mountdir_button = gtk_button_new_with_label ("Mount Point");
	password_label = gtk_label_new ("Password");
	cipher_label = gtk_label_new ("Cipher");

	gtk_table_attach (GTK_TABLE(table), basedir_button,  0, 1, 0, 1, GTK_FILL | GTK_SHRINK, GTK_FILL | GTK_SHRINK, 5, 5);
	gtk_table_attach (GTK_TABLE(table), mountdir_button, 0, 1, 1, 2, GTK_FILL | GTK_SHRINK, GTK_FILL | GTK_SHRINK, 5, 5);
	gtk_table_attach (GTK_TABLE(table), password_label,  0, 1, 2, 3, GTK_FILL | GTK_SHRINK, GTK_FILL | GTK_SHRINK, 5, 5);
	gtk_table_attach (GTK_TABLE(table), cipher_label,    0, 1, 3, 4, GTK_FILL | GTK_SHRINK, GTK_FILL | GTK_SHRINK, 5, 5);

	basedir_label = gtk_label_new("?");
	mountdir_label = gtk_label_new("?");
	password_entry = gtk_entry_new();
	cipher_combo = gtk_combo_box_new_text ();

	object->cipher = g_strdup_printf ("aes-256-gcm");
	gtk_combo_box_append_text (GTK_COMBO_BOX(cipher_combo), "aes-256-gcm");
	gtk_combo_box_append_text (GTK_COMBO_BOX(cipher_combo), "aes-256-cfb");
	gtk_combo_box_append_text (GTK_COMBO_BOX(cipher_combo), "aes-128-gcm");
	gtk_combo_box_append_text (GTK_COMBO_BOX(cipher_combo), "aes-128-cfb");
	gtk_combo_box_append_text (GTK_COMBO_BOX(cipher_combo), "twofish-256-gcm");
	gtk_combo_box_append_text (GTK_COMBO_BOX(cipher_combo), "twofish-256-cfb");
	gtk_combo_box_append_text (GTK_COMBO_BOX(cipher_combo), "twofish-128-gcm");
	gtk_combo_box_append_text (GTK_COMBO_BOX(cipher_combo), "twofish-128-cfb");
	gtk_combo_box_append_text (GTK_COMBO_BOX(cipher_combo), "serpent-256-gcm");
	gtk_combo_box_append_text (GTK_COMBO_BOX(cipher_combo), "serpent-256-cfb");
	gtk_combo_box_append_text (GTK_COMBO_BOX(cipher_combo), "serpent-128-gcm");
	gtk_combo_box_append_text (GTK_COMBO_BOX(cipher_combo), "serpent-128-cfb");
	gtk_combo_box_append_text (GTK_COMBO_BOX(cipher_combo), "cast-256-gcm");
	gtk_combo_box_append_text (GTK_COMBO_BOX(cipher_combo), "cast-256-cfb");
	gtk_combo_box_append_text (GTK_COMBO_BOX(cipher_combo), "mars-448-gcm");
	gtk_combo_box_append_text (GTK_COMBO_BOX(cipher_combo), "mars-448-cfb");
	gtk_combo_box_append_text (GTK_COMBO_BOX(cipher_combo), "mars-256-gcm");
	gtk_combo_box_append_text (GTK_COMBO_BOX(cipher_combo), "mars-256-cfb");
	gtk_combo_box_append_text (GTK_COMBO_BOX(cipher_combo), "mars-128-gcm");
	gtk_combo_box_append_text (GTK_COMBO_BOX(cipher_combo), "mars-128-cfb");

	gtk_combo_box_set_active (GTK_COMBO_BOX(cipher_combo), 0);

	object->basedir_label = basedir_label;
	object->mountdir_label = mountdir_label;
	object->password_entry = password_entry;

	gtk_table_attach (GTK_TABLE(table), basedir_label,  1, 2, 0, 1, GTK_FILL | GTK_SHRINK, GTK_FILL | GTK_SHRINK, 5, 5);
	gtk_table_attach (GTK_TABLE(table), mountdir_label, 1, 2, 1, 2, GTK_FILL | GTK_SHRINK, GTK_FILL | GTK_SHRINK, 5, 5);
	gtk_table_attach (GTK_TABLE(table), password_entry, 1, 2, 2, 3, GTK_FILL | GTK_SHRINK, GTK_FILL | GTK_SHRINK, 5, 5);
	gtk_table_attach (GTK_TABLE(table), cipher_combo,   1, 2, 3, 4, GTK_FILL | GTK_SHRINK, GTK_FILL | GTK_SHRINK, 5, 5);

	g_signal_connect (G_OBJECT(basedir_button),  "clicked", G_CALLBACK(basedir_button_click),  G_OBJECT(object));
	g_signal_connect (G_OBJECT(mountdir_button), "clicked", G_CALLBACK(mountdir_button_click), G_OBJECT(object));
	g_signal_connect (G_OBJECT(cipher_combo),    "changed", G_CALLBACK(combo_selected),        G_OBJECT(object));

	/* box containg "Mount" and "Cancel" buttons */
	hbox = gtk_hbox_new(TRUE, 3);

	mount_button  = gtk_button_new_with_label ("Mount");
	cancel_button = gtk_button_new_with_label ("Cancel");

	gtk_widget_set_size_request (mount_button, 70, 30);
	gtk_container_add (GTK_CONTAINER(hbox), mount_button);
	gtk_container_add (GTK_CONTAINER(hbox), cancel_button);

	g_signal_connect (G_OBJECT(mount_button),  "clicked", G_CALLBACK(mount_filesystem), G_OBJECT(object));
	g_signal_connect (G_OBJECT(cancel_button), "clicked", G_CALLBACK(close_window),     G_OBJECT(object));

	halign = gtk_alignment_new (1, 0, 0, 0);
	gtk_container_add (GTK_CONTAINER(halign), hbox);
	gtk_box_pack_start (GTK_BOX(vbox), halign, FALSE, FALSE, 0);

	return window;
}

