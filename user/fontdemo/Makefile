
PROGRAM = fontdemo
MKICON = ../../mkicon/mkicon

all: dirs $(PROGRAM)

OBJS += tfont.o

include ../../common.mk

INCLUDE_DIRS += ../../kernel/interface/ include/ stdlib/include/ ../../../turtlefont

all: dirs $(PROGRAM)

dirs:
	mkdir -p obj

icon.o: icon.png
	$(MKICON) icon.png
	objcopy -I binary -O elf32-i386 -B i386 --rename-section .data=icon icon.bin obj/icon.o
	rm icon.bin

tfont.o: ../../../turtlefont/tfont.c
	$(CC) $(CFLAGS) -c -o obj/$@ $<

$(PROGRAM): $(OBJS) icon.o
	$(LD) -L ../stdlib/lib/ -T../stdlib/linker.ld -o $@ $(addprefix obj/, $^) $(LDFLAGS) $(LIBS)