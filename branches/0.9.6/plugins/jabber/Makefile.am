sim_plugin_LTLIBRARIES = jabber.la

jabber_la_SOURCES = \
 jabber.cpp \
 jabberconfig.cpp \
 jabberconfigbase.ui \
 jabberclient.cpp \
 jabber_auth.cpp \
 jabber_rosters.cpp \
 jabber_ssl.cpp \
 jabberinfo.cpp \
 jabberinfobase.ui \
 jabberhomeinfo.cpp \
 jabberhomeinfobase.ui \
 jabberworkinfo.cpp \
 jabberworkinfobase.ui \
 jabberaboutinfo.cpp \
 jabberaboutinfobase.ui \
 jabberadd.cpp \
 jabberaddbase.ui \
 jabberbuffer.cpp \
 jabberfiletransfer.cpp \
 jabbersearch.cpp \
 jabbermessage.cpp \
 jabberpicture.cpp \
 jabberpicturebase.ui \
 jabberbrowser.cpp \
 discoinfo.cpp \
 discoinfobase.ui \
 infoproxy.cpp \
 infoproxybase.ui \
 jidsearch.cpp \
 jidsearchbase.ui \
 jidadvsearch.cpp \
 jidadvsearchbase.ui \
 httppoll.cpp 

INCLUDES=-I$(srcdir)/../../sim -I../_core -I$(srcdir)/../_core $(all_includes)

jabber_la_METASOURCES = AUTO
jabber_la_LIBADD = ../../sim/libsim.la  $(LIBUI) $(LIB_QT) $(LIB_KDECORE) $(LIBSSL) $(EXTRA_LIBS)
jabber_la_LDFLAGS = -module -no-undefined -avoid-version -L../../sim/.libs $(QT_LDFLAGS) $(KDE_RPATH) $(all_libraries)


