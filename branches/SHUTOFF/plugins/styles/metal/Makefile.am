sim_styles_LTLIBRARIES = metal.la

metal_la_SOURCES = \
 metal.cpp \
 metal.h

INCLUDES=-I../../../sim/api $(all_includes)

metal_la_METASOURCES = AUTO
metal_la_LIBADD = $(LIB_QT) $(EXTRA_LIBS)
metal_la_LDFLAGS = -module -no-undefined -avoid-version $(QT_LDFLAGS) $(KDE_RPATH) $(all_libraries)

install-data-hook:
	rm -rf $(DESTDIR)$(sim_stylesdir)/metal.la


