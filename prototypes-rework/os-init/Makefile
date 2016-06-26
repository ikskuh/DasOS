##
# Builds the os initialization kernel
##

include ../config.mk

KERNEL   = os-init.ker
LIBS    += -lbase -lgcc -lboot
FLAGS   += -D IDT_DISPATCH=interrupt_dispatch

all: builddir $(KERNEL)

include ../common.mk
include ../kernel.mk