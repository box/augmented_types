PHPDOC_SHARED_DEPENDENCIES = $(srcdir)/phpdoc/libphpdoc.a
PHPDOC_SHARED_LIBADD := ${PHPDOC_SHARED_LIBADD}
$(srcdir)/augmented_types.cc: $(srcdir)/phpdoc/libphpdoc.a
$(srcdir)/phpdoc/libphpdoc.a: FORCE
	$(MAKE) $(MFLAGS) -C $(srcdir)/phpdoc libphpdoc.a INCLUDES="$(INCLUDES)"

FORCE:
