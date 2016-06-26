

$(KERNEL): $(OBJS)
	$(LD) -T../linker.ld -o ../kernels/$@ $(addprefix obj/, $^) $(LDFLAGS) $(LIBS)
	
deploy: $(KERNEL)
	cp ../kernels/$(KERNEL) /srv/tftp/$(KERNEL)
