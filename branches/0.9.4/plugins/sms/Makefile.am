sim_plugin_LTLIBRARIES = sms.la

sms_la_SOURCES = \
 sms.cpp \
 smssetup.cpp \
 smssetupbase.ui \
 gsm_ta.cpp \
 serial.cpp 

SUBDIRS=sounds
EXTRA_DIST = xpm/simcard.xpm

INCLUDES=-I../../sim -I../_core $(all_includes)

sms_la_METASOURCES = AUTO
sms_la_LIBADD = ../../sim/libsim.la  $(LIBUI) $(LIB_KDECORE) $(LIB_KDEUI) $(LIB_KFILE) $(LIB_QT) $(EXTRA_LIBS)
sms_la_LDFLAGS = -module -no-undefined -avoid-version -L../../sim/.libs $(QT_LDFLAGS) $(KDE_RPATH) $(all_libraries)



