
all: kernel user boot.img

kernel: ./kernel/
	$(MAKE) -C $@ -B
	
user: ./user/
	$(MAKE) -C $@ -B

.PHONY: boot.img
boot.img:
	mformat -C -f 1440 -v VIDEO -i boot.img ::
	mcopy -i boot.img \
		kernel/DasOS \
		user/demo/demo \
		user/game/game \
		syslinux.cfg \
		/boot/syslinux/libcom32.c32 \
		/boot/syslinux/libutil.c32 \
		/boot/syslinux/menu.c32 \
		/boot/syslinux/mboot.c32 \
		/boot/syslinux/poweroff.c32 \
		::
	mmd -i boot.img ::/resource
	mcopy -i boot.img resources/* ::/resource/
	syslinux boot.img
	mdir -i boot.img ::
