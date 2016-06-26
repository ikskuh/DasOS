##
# Build all projects
##

PROJECTS = $(shell ls --file-type --ignore=libs --ignore=kernels --ignore=include | grep / | sed "s|/||")

LIBS    = $(filter lib%, $(PROJECTS))
KERNELS = $(filter-out lib%, $(PROJECTS))

all: $(KERNELS) boot.img

.PHONY: $(PROJECTS)
$(KERNELS): $(LIBS)
	make -C $@ $(ARGS)

$(LIBS):
	make -C $@ $(ARGS)

boot.img: $(PROJECTS)
	mformat -C -f 1440 -v VIDEO -i boot.img ::
	mcopy -i boot.img \
		kernels/* \
		syslinux.cfg \
		/boot/syslinux/libcom32.c32 \
		/boot/syslinux/libutil.c32 \
		/boot/syslinux/menu.c32 \
		/boot/syslinux/mboot.c32 \
		::
	syslinux boot.img
	mdir -i boot.img ::

run: boot.img
	qemu-system-i386 boot.img -serial stdio