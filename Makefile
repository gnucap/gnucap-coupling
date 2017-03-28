GNUCAP_CONF = $(shell which gnucap-conf$(SUFFIX))
PACKAGE_NAME = gnucap-coupling

-include Make.override

ifneq ($(GNUCAP_CONF),)
    CXX = $(shell $(GNUCAP_CONF) --cxx)
    GNUCAP_CPPFLAGS = $(shell $(GNUCAP_CONF) --cppflags) -DADD_VERSION -DPIC
    GNUCAP_CXXFLAGS = $(shell $(GNUCAP_CONF) --cxxflags)
    GNUCAP_LDFLAGS = $(shell $(GNUCAP_CONF) --ldflags)
	 GNUCAP_LIBDIR   = $(shell $(GNUCAP_CONF) --libdir)
	 GNUCAP_PKGLIBDIR = $(shell $(GNUCAP_CONF) --pkglibdir)
	 GNUCAP_EXEC_PREFIX = $(shell $(GNUCAP_CONF) --exec-prefix)
#	 GNUCAP_PREFIX = $(shell $(GNUCAP_CONF) --prefix)
# TODO complete gnucap-conf
	 GNUCAP_PREFIX   = $(shell $(GNUCAP_CONF) --exec-prefix)# BUG, should be prefix!
	 GNUCAP_PKGLIBDIR = $(GNUCAP_LIBDIR)/gnucap
	 GNUCAP_DOCDIR = $(GNUCAP_PREFIX)/share/doc
else
    $(info no gnucap-conf, this will not work)
endif

GNUCAP_CXXFLAGS+= -fPIC -shared
PLUGIN_FILES = gnucap-coupling.so
#EXAMPLES = scale.1.gc

gnucap_coupling_SOURCES = \
	bm_file.cc \
	bm_file_wrap.cc

# is this a bug in upstream meter? use this for now.
gnucap_coupling_SOURCES += d_meter.cc


CHECK_PLUGINS = ${gnucap_coupling_SOURCES:%.cc=%.so}

CLEANFILES = ${EXEC_FILES} *.so *.o

all: $(EXEC_FILES) ${PLUGIN_FILES} ${CHECK_PLUGINS}

LIBS=

%.so: %.cc
	$(CXX) $(CXXFLAGS) $(GNUCAP_CXXFLAGS) $(CPPFLAGS) $(GNUCAP_CPPFLAGS) -o $@ $< ${LIBS}

%.o: %.cc
	$(CXX) $(CXXFLAGS) $(GNUCAP_CXXFLAGS) $(CPPFLAGS) $(GNUCAP_CPPFLAGS) -o $@ -c $<

check:
	:

gnucap_coupling_OBJS = ${gnucap_coupling_SOURCES:%.cc=%.o}
gnucap_coupling_SOBJS = ${gnucap_coupling_SOURCES:%.cc=%.so}

gnucap-coupling.so: $(gnucap_coupling_OBJS)
	$(CXX) -shared $+ -o $@ ${LIBS}

.PHONY: check

install: $(EXEC_FILES) ${PLUGIN_FILES}
	-install -d $(DESTDIR)/$(GNUCAP_PKGLIBDIR)
	install $(PLUGIN_FILES) $(DESTDIR)/$(GNUCAP_PKGLIBDIR)

	install -d $(DESTDIR)$(GNUCAP_DOCDIR)/$(PACKAGE_NAME)
	install README $(DESTDIR)$(GNUCAP_DOCDIR)/$(PACKAGE_NAME)

	install -d $(DESTDIR)$(GNUCAP_DOCDIR)/$(PACKAGE_NAME)/examples
	install $(EXAMPLES) $(DESTDIR)$(GNUCAP_DOCDIR)/$(PACKAGE_NAME)/examples

clean:
	rm -f $(CLEANFILES)

distclean: clean
	rm Make.override

Make.override:
	[ -e $@ ] || echo "# here you may override settings" > $@
