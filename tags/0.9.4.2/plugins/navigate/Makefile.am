sim_plugin_LTLIBRARIES = navigate.la

navigate_la_SOURCES = \
 navigate.cpp \
 navcfg.cpp \
 navcfgbase.ui \
 parseurl.ll

INCLUDES=-I../../sim -I../_core $(all_includes)

navigate_la_METASOURCES = AUTO
navigate_la_LIBADD = ../../sim/libsim.la  $(LIB_QT) $(LIB_KDECORE) $(EXTRA_LIBS)
navigate_la_LDFLAGS = -module -no-undefined -avoid-version -L../../sim/.libs $(QT_LDFLAGS) $(KDE_RPATH) $(all_libraries)



