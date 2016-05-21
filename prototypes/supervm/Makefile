
AS=gcc
CC=gcc
CXX=g++
LD=ld
AR=ar

FLAGS = -Werror -Wall -iquote include -O3 -g
VMFLAGS = $(FLAGS) -m32 -mno-sse -ffreestanding 

ARTIFACT = libvm.a
PROGRAM = vm

all: $(ARTIFACT) $(PROGRAM) 

$(PROGRAM): main.c vm.c vm.h
	$(CC) -o $@ $(FLAGS) main.c vm.c
	
$(ARTIFACT): vm.c vm.h
	$(CC) $(VMFLAGS) -c -o obj/vm.o.32 vm.c
	$(AR) rcs $(ARTIFACT) obj/vm.o.32
