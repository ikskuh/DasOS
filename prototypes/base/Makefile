
AS=gcc
CC=gcc
CXX=g++
LD=ld

FLAGS = -ffreestanding -m32 -Werror -Wall -iquote include 
ASFLAGS = $(FLAGS)
CFLAGS = $(FLAGS)	
CXXFLAGS = $(FLAGS) -std=c++14 -fno-rtti -fno-exceptions -fno-leading-underscore -fno-use-cxa-atexit -nostdlib -fno-builtin

SRCS = $(shell find -regextype egrep -regex '.*/.*\.(cpp|S|c)')
OBJS = $(addsuffix .o, $(notdir $(basename $(SRCS))))

all: kernel-base.ker

kernel-base.ker: $(OBJS)
	$(LD) -melf_i386 -Tlinker.ld -o kernel-base.ker $(addprefix obj/, $^)

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
	qemu-system-i386 -kernel kernel-base.ker

bnr: kernel-base.ker run

deploy: kernel-base.ker
	cp kernel-base.ker /srv/tftp/kernel-base.ker