UTIL_TARGETS := bchenc lplr_checksum lzma
CFLAGS = -Wall -O0 -g -Wno-unused-result

distclean clean all preconfig::
	@for f in $(UTIL_TARGETS) ; do \
	    $(MAKE) -C $$f $@ || exit 1; \
	done

disclean ::
	-rm -rf bin/*

.PHONY: all preconfig distclean clean
