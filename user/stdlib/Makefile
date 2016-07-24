all: paths libstd.a

include ../../common.mk

INCLUDE_DIRS += ../../kernel/interface

paths:
	mkdir -p obj lib

libstd.a: $(OBJS)
	$(AR) rcs lib/$@ $(addprefix obj/, $^)