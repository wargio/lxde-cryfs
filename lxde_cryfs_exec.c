#include "lxde_cryfs_exec.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int lxde_cryfs_exec_mount (const char* base, const char* mount, const char* cipher, const char* password, const char* blocksize, const char* unmount_idle) {
	char prompt[1024] = {0};
	FILE *shell = 0;
	if (!blocksize || strlen (blocksize) < 1) {
		blocksize = "32768";
	}
	if (!unmount_idle || strlen (unmount_idle) < 1) {
		snprintf (prompt, sizeof(prompt), "CRYFS_FRONTEND=noninteractive cryfs --cipher %s --blocksize %s \"%s\" \"%s\"", 
			cipher, blocksize, base, mount);
	} else {
		snprintf (prompt, sizeof(prompt), "CRYFS_FRONTEND=noninteractive cryfs --cipher %s --blocksize %s --unmount-idle %s \"%s\" \"%s\"", 
			cipher, blocksize, unmount_idle, base, mount);
	}
	shell = popen (prompt, "w");
	if (!shell) {
		return 0;
	}
	fprintf(shell, "%s\n", password);
	if (pclose (shell) != 0) {
		return 0;
	}
	return 1;
}

int lxde_cryfs_exec_umount (const char* mount) {
	char prompt[1024] = {0};
	FILE *shell = 0;
	if (!mount || strlen (mount) < 1) {
		return 0;
	}
	snprintf (prompt, sizeof(prompt), "fusermount -u \"%s\"", mount);
	shell = popen (prompt, "w");
	if (!shell) {
		return 0;
	}
	if (pclose (shell) != 0) {
		return 0;
	}
	return 1;
}

int mounted_fs_size () {
	char buffer[32] = {0};
	int size = 0;  
	FILE* shell = popen("mount | grep cryfs | cut -d ' ' -f3 | wc -l", "r");
	if (!shell) {
		return size;
	}
	fread (buffer, 1, sizeof(buffer), shell);
	if (pclose (shell) != 0) {
		return size;
	}
	size = strtol (buffer, 0, 10);
	return (size < 0 ? 0 : (size > 64 ? 64 : size));
}

int lxde_cryfs_mounted_fs (paths_t* path) {
	char *line = NULL;
	size_t len = 0;
	ssize_t nread;
	int size = mounted_fs_size ();
	if (size < 1) {
		return 0;
	}
	path->size = 0;
	path->entry = calloc (size, sizeof(char*));
	if (!path->entry) {
		return 0;
	}

	FILE* shell = popen("mount | grep cryfs | cut -d ' ' -f3", "r");
	if (!shell) {
		free (path->entry);
		path->entry = 0;
		return 0;
	}
	for (int i = 0; i < size; ++i) {
		if ((nread = getline (&line, &len, shell)) != -1) {
			line[nread-1] = 0;
			path->entry[i] = line;
			path->size++;
			line = NULL;
			len = 0;
		}
	}
	if (pclose (shell) != 0) {
		free (path->entry);
		path->entry = 0;
		return 0;
	}
	return 1;
}
