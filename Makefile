
all: kernel user boot.img

kernel: ./kernel/
	$(MAKE) -C $@ -B
	
user: ./user/
	$(MAKE) -C $@ -B

boot.img: kernel/DasOS user/counter
	mformat -C -f 1440 -v VIDEO -i boot.img ::
	mcopy -i boot.img \
		kernel/DasOS \
		user/counter \
		syslinux.cfg \
		/boot/syslinux/libcom32.c32 \
		/boot/syslinux/libutil.c32 \
		/boot/syslinux/menu.c32 \
		/boot/syslinux/mboot.c32 \
		::
	syslinux boot.img
	mdir -i boot.img ::
