sim_plugin_LTLIBRARIES = replace.la

replace_la_SOURCES = \
 replace.cpp \
 replacecfg.cpp \
 replacecfgbase.ui

INCLUDES=-I../../sim -I../_core $(all_includes)

replace_la_METASOURCES = AUTO
replace_la_LIBADD = ../../sim/libsim.la  $(LIBUI) $(LIB_KDECORE) $(LIB_KDEUI) $(LIB_KFILE) $(LIB_QT) $(EXTRA_LIBS)
replace_la_LDFLAGS = -module -no-undefined -avoid-version $(QT_LDFLAGS) $(KDE_RPATH) $(all_libraries)

install-data-hook:
	rm -rf $(DESTDIR)$(sim_plugindir)/replace.la


