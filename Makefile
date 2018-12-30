MAKE=make

SUBDIRS=

all: $(SUBDIRS) cstyle

$(SUBDIRS) :
	@echo "=== $@"
	@$(MAKE) -C $@

.PHONY: cstyle all clean $(SUBDIRS)