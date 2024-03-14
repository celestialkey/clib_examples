SUBDIRS := ringbuffer packet hashing queue scomms vtap commlinks map commands

.PHONY: all $(SUBDIRS)

all: $(SUBDIRS)

$(SUBDIRS):
	@echo "\nMaking $@..."
	$(MAKE) -C $@

.PHONY: clean

clean:
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir clean; \
	done