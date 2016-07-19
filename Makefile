
all: kernel user

kernel: ./kernel/
	$(MAKE) -C $@ -B
	
user: ./user/
	$(MAKE) -C $@ -B