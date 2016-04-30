
AS=clang
CC=clang
CXX=clang++
LD=ld

FLAGS = -ffreestanding -m32
ASFLAGS = $(FLAGS)
CFLAGS = $(FLAGS)
CXXFLAGS = $(FLAGS)

SRCS = $(shell find -name '*.[cS]')
OBJS = $(addsuffix .o, $(notdir $(basename $(SRCS))))

all: kernel-base.ker

kernel-base.ker: $(OBJS)
	$(LD) -melf_i386 -Ttext=0x1000000 -o kernel-base.ker $(addprefix obj/, $^)

%.o: %.c
	$(CC) $(ASFLAGS) -c -o obj/$@ $<
  
%.o: %.S
	$(AS) $(CFLAGS) -c -o obj/$@ $<
  
%.o: %.cpp
	$(CPP) $(CXXFLAGS) -c -o obj/$@ $<