
AS=gcc
CC=gcc
CXX=g++
LD=ld

IDT_DISPATCH = _ZN3IDT8dispatchEP8CpuState

FLAGS = -mno-sse -DIDT_DISPATCH=$(IDT_DISPATCH) -ffreestanding -m32 -Werror -Wall -iquote include -iquote lists -O3 -g
ASFLAGS = $(FLAGS)
CFLAGS = $(FLAGS)	
CXXFLAGS = $(FLAGS) -std=c++14 -fno-rtti -fno-exceptions -fno-leading-underscore -fno-use-cxa-atexit -nostdlib -fno-builtin

SRCS = $(shell find -regextype egrep -regex '.*/.*\.(cpp|S|c)')
OBJS = $(addsuffix .o, $(notdir $(basename $(SRCS))))

LIBGCC = $(shell gcc -m32 -print-libgcc-file-name)

all: kernel-base.ker

kernel-base.ker: $(OBJS)
	$(LD) -melf_i386 -Tlinker.ld -o kernel-base.ker $(addprefix obj/, $^) $(LIBGCC) 

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o obj/$@ $<
  
%.o: %.c
	$(CC) $(ASFLAGS) -c -o obj/$@ $<
  
%.o: %.S
	$(AS) $(CFLAGS) -c -o obj/$@ $<
	
%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c -o obj/$@ $<
  
%.o: src/%.c
	$(CC) $(ASFLAGS) -c -o obj/$@ $<
  
%.o: src/%.S
	$(AS) $(CFLAGS) -c -o obj/$@ $<

# Linux/Multiboot boot specific:
# -kernel bzImage use 'bzImage' as kernel image
# -append cmdline use 'cmdline' as kernel command line
# -initrd file    use 'file' as initial ram disk
# -dtb    file    use 'file' as device tree image


run:
	qemu-system-i386 \
		-kernel kernel-base.ker \
		-m 64 \
		-d cpu_reset,int \
		-no-reboot \
		-no-shutdown \
		-serial stdio \
		-initrd ../program0/program0.bin 

insight:
	objdump -d kernel-base.ker | c++filt | less

bnr: kernel-base.ker run

deploy: kernel-base.ker
	cp kernel-base.ker /srv/tftp/kernel-base.ker