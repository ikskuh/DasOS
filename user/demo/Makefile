
PROGRAM = demo

all: dirs $(PROGRAM)

include ../../common.mk

INCLUDE_DIRS += ../../kernel/interface/ include/ stdlib/include/

all: dirs $(PROGRAM)

dirs:
	mkdir -p obj

icon.o: icon.bin
	objcopy -I binary -O elf32-i386 -B i386 --rename-section .data=icon icon.bin obj/icon.o

$(PROGRAM): $(OBJS) icon.o
	$(LD) -L ../stdlib/lib/ -T../stdlib/linker.ld -o $@ $(addprefix obj/, $^) $(LDFLAGS) $(LIBS)