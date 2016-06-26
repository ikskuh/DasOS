
$(LIBRARY): $(OBJS)
	$(AR) rcs ../libs/$(LIBRARY) $(addprefix obj/, $^)