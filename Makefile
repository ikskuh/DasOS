
all: kernel

kernel: ./kernel/
	$(MAKE) -C $@