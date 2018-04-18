SOURCE := lxde_cryfs_plugin.c lxde_cryfs_mount.c lxde_cryfs_dialog.c lxde_cryfs_config.c lxde_cryfs_menu.c lxde_cryfs_exec.c
LIBNAME := lxde_cryfs_plugin.so

all:
	@echo "[LD]" $(LIBNAME)
	@gcc -Wall `pkg-config --cflags gtk+-2.0 lxpanel` -shared -fPIC $(SOURCE) -o $(LIBNAME) `pkg-config --libs lxpanel`

install: uninstall
	cp $(LIBNAME) /usr/lib/lxpanel/plugins

uninstall:
	rm /usr/lib/lxpanel/plugins/$(LIBNAME)

restart-panel:
	lxpanelctl restart

clean:
	@rm $(LIBNAME)
