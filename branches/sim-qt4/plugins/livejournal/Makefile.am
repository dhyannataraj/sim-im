sim_plugin_LTLIBRARIES = livejournal.la

livejournal_la_SOURCES = \
 livejournal.cpp \
 livejournalcfg.cpp \
 livejournalcfgbase.ui \
 msgjournal.cpp \
 msgjournalbase.ui \
 journalsearch.cpp \
 journalsearchbase.ui 

INCLUDES=-I../../lib -I../_core $(all_includes)

livejournal_la_METASOURCES = AUTO
livejournal_la_LIBADD = ../../lib/libsim.la  $(LIBUI) $(LIB_KDECORE) $(LIB_KDEUI) $(LIB_KFILE) $(LIB_QT) $(LIBSSL) $(EXTRA_LIBS)
livejournal_la_LDFLAGS = -module -no-undefined -avoid-version -L../../lib/.libs $(QT_LDFLAGS) $(KDE_RPATH) $(all_libraries)

install-data-hook:
	rm -rf $(DESTDIR)$(sim_plugindir)/livejournal.la


