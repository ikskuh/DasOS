##
# DasOS common Makefile targets
##

INCLUDE_DIRS += ../include/ /opt/lib/gcc/i686-elf/6.1.0/include/

FLAGS    = -ffreestanding -mno-sse -Werror -Wall -iquote include $(addprefix -I, $(INCLUDE_DIRS)) -O3 -g
ASFLAGS  = $(FLAGS)
CFLAGS   = $(FLAGS)	
CXXFLAGS = $(FLAGS) -std=c++14 -fno-rtti -fno-exceptions -fno-leading-underscore -fno-use-cxa-atexit -nostdlib -fno-builtin
LDFLAGS  = -L../libs/

SRCS = $(shell find -regextype egrep -regex '.*/.*\.(cpp|S|c)')
OBJS = $(addsuffix .o, $(notdir $(basename $(SRCS))))


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

builddir:
	mkdir -p ./obj/