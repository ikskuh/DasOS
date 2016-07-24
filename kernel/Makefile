KERNEL=DasOS

all: dirs $(KERNEL)

include ../common.mk

INCLUDE_DIRS += interface/ include/ 

FLAGS    += -DDASOS_KERNEL 

dirs:
	mkdir -p obj

$(KERNEL): $(OBJS)
	$(LD) -Tlinker.ld -o $@ $(addprefix obj/, $^) $(LDFLAGS) $(LIBS)