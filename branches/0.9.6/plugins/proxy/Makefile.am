sim_plugin_LTLIBRARIES = proxy.la

proxy_la_SOURCES = \
 proxy.cpp \
 proxycfg.cpp \
 proxycfgbase.ui \
 proxyerror.cpp \
 proxyerrorbase.ui

INCLUDES=-I$(srcdir)/../../sim -I../_core -I$(srcdir)/../_core $(all_includes)

proxy_la_METASOURCES = AUTO
proxy_la_LIBADD = ../../sim/libsim.la  $(LIB_QT) $(LIB_KDECORE) $(EXTRA_LIBS)
proxy_la_LDFLAGS = -module -no-undefined -avoid-version -L../../sim/.libs $(QT_LDFLAGS) $(KDE_RPATH) $(all_libraries)



