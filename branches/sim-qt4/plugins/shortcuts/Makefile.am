sim_plugin_LTLIBRARIES = shortcuts.la

shortcuts_la_SOURCES = \
 shortcuts.cpp \
 shortcutcfg.cpp \
 shortcutcfgbase.ui \
 mousecfg.cpp \
 mousecfgbase.ui

INCLUDES=-I../../lib -I../_core $(all_includes)

shortcuts_la_METASOURCES = AUTO
shortcuts_la_LIBADD = ../../lib/libsim.la  $(LIBUI) $(LIB_KDECORE) $(LIB_KDEUI) $(LIB_KFILE) $(LIB_QT) $(EXTRA_LIBS)
shortcuts_la_LDFLAGS = -module -no-undefined -avoid-version -L../../lib/.libs $(QT_LDFLAGS) $(KDE_RPATH) $(all_libraries)

install-data-hook:
	rm -rf $(DESTDIR)$(sim_plugindir)/shortcuts.la


