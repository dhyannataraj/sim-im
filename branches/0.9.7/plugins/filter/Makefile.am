sim_plugin_LTLIBRARIES = filter.la

filter_la_SOURCES = \
 filter.cpp \
 filtercfg.cpp \
 filtercfgbase.ui \
 ignorelist.cpp \
 ignorelistbase.ui

INCLUDES=-I$(srcdir)/../../sim -I../_core -I$(srcdir)/../_core $(all_includes)

filter_la_METASOURCES = AUTO
filter_la_LIBADD = ../../sim/libsim.la  $(LIBUI) $(LIB_QT) $(LIB_KDECORE) $(EXTRA_LIBS)
filter_la_LDFLAGS = -module -no-undefined -avoid-version -L../../sim/.libs $(QT_LDFLAGS) $(KDE_RPATH) $(all_libraries)



