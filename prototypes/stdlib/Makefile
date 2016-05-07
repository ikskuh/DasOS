
AS=gcc
CC=gcc
CXX=g++
LD=ld
AR=ar

FLAGS = -mno-sse -ffreestanding -m32 -Werror -Wall -iquote include -O3 -g
ASFLAGS = $(FLAGS)
CFLAGS = $(FLAGS)	
CXXFLAGS = $(FLAGS) -std=c++14 -fno-rtti -fno-exceptions -fno-leading-underscore -fno-use-cxa-atexit -nostdlib -fno-builtin

ARTIFACT = libstd.a

SRCS = $(shell find -regextype egrep -regex '.*/.*\.(cpp|S|c)')
OBJS = $(addsuffix .o, $(notdir $(basename $(SRCS))))

all: $(ARTIFACT)

$(ARTIFACT): $(OBJS)
	$(AR) rcs $(ARTIFACT) $(addprefix obj/, $^)

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

deploy: $(ARTIFACT)
	cp $(ARTIFACT) /srv/tftp/$(ARTIFACT)