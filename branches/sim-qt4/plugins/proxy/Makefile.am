sim_plugin_LTLIBRARIES = proxy.la

proxy_la_SOURCES = \
 proxy.cpp \
 proxycfg.cpp \
 proxycfgbase.ui \
 proxyerror.cpp \
 proxyerrorbase.ui

INCLUDES=-I../../lib -I../_core $(all_includes)

proxy_la_METASOURCES = AUTO
proxy_la_LIBADD = ../../lib/libsim.la  $(LIB_QT) $(LIB_KDECORE) $(EXTRA_LIBS)
proxy_la_LDFLAGS = -module -no-undefined -avoid-version -L../../lib/.libs $(QT_LDFLAGS) $(KDE_RPATH) $(all_libraries)

install-data-hook:
	rm -rf $(DESTDIR)$(sim_plugindir)/proxy.la


