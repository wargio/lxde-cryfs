SOURCE := lxde_cryfs_plugin.c lxde_cryfs_mount.c lxde_cryfs_dialog.c

all:
	gcc -Wall `pkg-config --cflags gtk+-2.0 lxpanel` -shared -fPIC $(SOURCE) -o lxde_cryfs_plugin.so `pkg-config --libs lxpanel`

install:
	cp lxde_cryfs_plugin.so /usr/lib/lxpanel/plugins

uninstall:
	rm /usr/lib/lxpanel/plugins/lxde_cryfs_plugin.so

restart-panel:
	lxpanelctl restart

clean:
	rm lxde_cryfs_plugin.so
