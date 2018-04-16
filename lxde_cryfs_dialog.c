#include "lxde_cryfs_dialog.h"
// g_free(path);

gchar *lxde_cryfs_folder_dialog() {
	GtkWidget *dialog;
	int dialog_ret = 0;
	gchar *path = NULL;
	dialog = gtk_file_chooser_dialog_new("Select directory", NULL, GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);
	/* Set initial directory. If it doesnt exist "/" is used. */
	gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog), "/");
	dialog_ret = gtk_dialog_run(GTK_DIALOG(dialog));
	if (dialog_ret == GTK_RESPONSE_ACCEPT) {
		path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
	}
	gtk_widget_destroy(dialog);
	return path;
}

void lxde_cryfs_error_dialog(GtkWidget *window, const char* error_message) {

	GtkWidget *dialog;
	dialog = gtk_message_dialog_new (GTK_WINDOW(window),
						GTK_DIALOG_DESTROY_WITH_PARENT,
						GTK_MESSAGE_ERROR,
						GTK_BUTTONS_OK,
						error_message);
	gtk_window_set_title(GTK_WINDOW (dialog), "Error");
	gtk_dialog_run (GTK_DIALOG(dialog));
	gtk_widget_destroy (dialog);
}