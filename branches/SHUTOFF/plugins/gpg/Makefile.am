sim_plugin_LTLIBRARIES = gpg.la

gpg_la_SOURCES = \
 gpg.cpp \
 gpgcfg.cpp \
 gpgcfgbase.ui \
 gpgadv.cpp \
 gpgadvbase.ui \
 gpggen.cpp \
 gpggenbase.ui \
 gpguser.cpp \
 gpguserbase.ui \
 passphrase.cpp \
 passphrasebase.ui

INCLUDES=-I../../sim -I../_core $(all_includes)

gpg_la_METASOURCES = AUTO
gpg_la_LIBADD = ../../sim/libsim.la  $(LIBUI) $(LIB_QT) $(LIB_KDECORE) $(EXTRA_LIBS)
gpg_la_LDFLAGS = -module -no-undefined -avoid-version $(QT_LDFLAGS) $(KDE_RPATH) $(all_libraries)

install-data-hook:
	rm -rf $(DESTDIR)$(sim_plugindir)/gpg.la


