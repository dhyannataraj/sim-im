sim_plugin_LTLIBRARIES = dock.la

dock_la_SOURCES = \
 dockcfgbase.ui \
 dockcfg.cpp \
 dockwnd.cpp \
 dock.cpp

INCLUDES=-I$(srcdir)/../../sim -I../_core -I$(srcdir)/../_core $(all_includes)

dock_la_METASOURCES = AUTO
dock_la_LIBADD = ../../sim/libsim.la  $(LIB_QT) $(LIB_KDECORE) $(EXTRA_LIBS)
dock_la_LDFLAGS = -module -no-undefined -avoid-version -L../../sim/.libs $(QT_LDFLAGS) $(KDE_RPATH) $(all_libraries)



