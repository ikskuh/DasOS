
AS=gcc
CC=gcc
CXX=g++
LD=ld

LIBGCC = $(shell gcc -m32 -print-libgcc-file-name)
INCLUDE_DIRS = 

FLAGS = -mno-sse -ffreestanding -m32 -Werror -Wall -iquote include -iquote lists $(addprefix -I, $(INCLUDE_DIRS)) -O3 -g
ASFLAGS = $(FLAGS)
CFLAGS = $(FLAGS)	
CXXFLAGS = $(FLAGS) -std=c++14 -fno-rtti -fno-exceptions -fno-leading-underscore -fno-use-cxa-atexit -nostdlib -fno-builtin

SRCS = $(shell find -regextype egrep -regex '.*/.*\.(cpp|S|c)')
OBJS = $(addsuffix .o, $(notdir $(basename $(SRCS))))
LIBS = $(LIBGCC) $(LIBSTD) $(LIBVM)

all: build-dirs video.ker

build-dirs:
	mkdir -p obj

video.ker: $(OBJS)
	$(LD) -melf_i386 -Tlinker.ld -o $@ $(addprefix obj/, $^) $(LIBS)

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

boot: image
	qemu-system-i386 \
		-fda boot.img \
		-m 64 \
		-d cpu_reset,int \
		-serial stdio
#		-no-reboot \
#		-no-shutdown \

insight:
	objdump -d kernel-base.ker | c++filt | less

bnr: video.ker run

deploy: video.ker
	cp video.ker /srv/tftp/video.ker

image: video.ker
	mformat -C -f 1440 -v VIDEO -i boot.img ::
	mcopy -i boot.img video.ker syslinux.cfg /boot/syslinux/libcom32.c32 /boot/syslinux/mboot.c32  ::
	syslinux boot.img
	mdir -i boot.img ::