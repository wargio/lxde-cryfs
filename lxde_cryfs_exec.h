#ifndef LXDE_CRYFS_EXEC_H
#define LXDE_CRYFS_EXEC_H

typedef struct {
	int size;
	char** entry;
} paths_t;

int lxde_cryfs_exec_mount (const char* base, const char* mount, const char* cipher, const char* password, const char* blocksize, const char* unmount_idle);
int lxde_cryfs_exec_umount (const char* mount);
int lxde_cryfs_exec_mounted_fs (paths_t* path);

#endif