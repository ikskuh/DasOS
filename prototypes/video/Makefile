##
# Builds the video test kernel
##

include ../config.mk

KERNEL = video.ker
LIBS  += -lboot -lvideo -lchardev -lserial -lbase -lgcc

all: builddir $(KERNEL)

include ../common.mk
include ../kernel.mk