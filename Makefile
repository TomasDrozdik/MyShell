// TODO
.PHONY: cstyle all clean $(SUBDIRS)

MAKE= make
SUBDIRS= src

all: $(SUBDIRS)

$(SUBDIRS) :
	@echo "=== $@"
	@$(MAKE) -C $@
