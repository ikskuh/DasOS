
PROJECTS = $(shell ls --file-type | grep / | sed "s|/||")

all: $(PROJECTS)

.PHONY: $(PROJECTS)
$(PROJECTS):
	make -C $@ $(ARGS)
