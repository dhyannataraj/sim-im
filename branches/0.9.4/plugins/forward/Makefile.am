sim_plugin_LTLIBRARIES = forward.la

forward_la_SOURCES = \
 forward.cpp \
 forwardcfg.cpp \
 forwardcfgbase.ui 

INCLUDES=-I../../sim -I../_core $(all_includes)

forward_la_METASOURCES = AUTO
forward_la_LIBADD = ../../sim/libsim.la  $(LIB_QT) $(LIB_KDECORE) $(EXTRA_LIBS)
forward_la_LDFLAGS = -module -no-undefined -avoid-version -L../../sim/.libs $(QT_LDFLAGS) $(KDE_RPATH) $(all_libraries)



