KERNEL=DasOS

CC=/opt/bin/i686-elf-gcc
CXX=/opt/bin/i686-elf-g++
LD=/opt/i686-elf/bin/ld
AR=/opt/i686-elf/bin/ar
AS=/opt/bin/i686-elf-gcc

LIBS += -L/opt/lib/gcc/i686-elf/6.1.0/
INCLUDE_DIRS += include/ /opt/lib/gcc/i686-elf/6.1.0/include/


FLAGS    += -ffreestanding -mno-sse -Werror -Wall -iquote include $(addprefix -I, $(INCLUDE_DIRS)) -O3 -g
ASFLAGS  += $(FLAGS)
CFLAGS   += $(FLAGS)	
CXXFLAGS += $(FLAGS) -std=c++14 -fno-rtti -fno-exceptions -fno-leading-underscore -fno-use-cxa-atexit -nostdlib -fno-builtin
LDFLAGS  += -L../libs/ -lgcc

SRCS += $(shell find -regextype egrep -regex '.*/.*\.(cpp|S|c)')
OBJS += $(addsuffix .o, $(notdir $(basename $(SRCS))))

all: dirs $(KERNEL) symbols

dirs:
	mkdir -p obj
	
symbols: $(KERNEL)
	nm DasOS | sed -r "s|(\w+)\s+(\w+)\s+(\w+)|\3 = \1;|" | grep -E "^\w+ = \w+;$$" > DasOS.syms

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o obj/$@ $<
  
%.o: %.c
	$(CC) $(CFLAGS) -c -o obj/$@ $<
  
%.o: %.S
	$(AS) $(ASFLAGS) -c -o obj/$@ $<

%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c -o obj/$@ $<

%.o: src/%.c
	$(CC) $(CFLAGS) -c -o obj/$@ $<
  
%.o: src/%.S
	$(AS) $(ASFLAGS) -c -o obj/$@ $<

builddir:
	mkdir -p ./obj/

$(KERNEL): $(OBJS)
	$(LD) -Tlinker.ld -o $@ $(addprefix obj/, $^) $(LDFLAGS) $(LIBS)