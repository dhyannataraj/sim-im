sim_plugin_LTLIBRARIES = _core.la

_core_la_SOURCES = \
 core.cpp \
 kpopup.cpp \
 commands.cpp \
 cmenu.cpp \
 mainwin.cpp \
 userlist.cpp \
 userview.cpp \
 cfgdlg.cpp \
 cfgdlgbase.ui \
 plugincfg.cpp \
 plugincfgbase.ui \
 prefcfg.cpp \
 prefcfgbase.ui \
 usercfg.cpp \
 toolsetup.cpp \
 toolsetupbase.ui \
 interfacecfg.cpp \
 interfacecfgbase.ui \
 userviewcfg.cpp \
 userviewcfgbase.ui \
 historycfg.cpp \
 historycfgbase.ui \
 userhistorycfg.cpp \
 userhistorycfgbase.ui \
 maininfo.cpp \
 maininfobase.ui \
 editmail.cpp \
 editmailbase.ui \
 editphone.cpp \
 editphonebase.ui \
 phonedetails.cpp \
 phonebase.ui \
 pagerdetails.cpp \
 pagerbase.ui \
 search.cpp \
 searchbase.ui \
 searchall.cpp \
 searchallbase.ui \
 nonim.cpp \
 nonimbase.ui \
 connectionsettings.cpp \
 connectionsettingsbase.ui \
 connectwnd.cpp \
 connectwndbase.ui \
 logindlg.cpp \
 logindlgbase.ui \
 manager.cpp \
 managerbase.ui \
 newprotocol.cpp \
 newprotocolbase.ui \
 status.cpp \
 statuswnd.cpp \
 container.cpp \
 userwnd.cpp \
 msgedit.cpp \
 msggen.cpp \
 msgsms.cpp \
 msgfile.cpp \
 msgview.cpp \
 msgauth.cpp \
 msgurl.cpp \
 msgcontacts.cpp \
 msgrecv.cpp \
 history.cpp \
 historywnd.cpp \
 libintl.cpp \
 autoreply.cpp \
 autoreplybase.ui \
 msgcfg.cpp \
 msgcfgbase.ui \
 filecfg.cpp \
 filecfgbase.ui \
 smscfg.cpp \
 smscfgbase.ui \
 arcfg.cpp \
 arcfgbase.ui \
 tmpl.cpp \
 filetransfer.cpp \
 filetransferbase.ui \
 declinedlg.cpp \
 declinedlgbase.ui 

SUBDIRS=pict styles jisp smiles icqlite additional icq5 icq5.1 GPL-Icons

INCLUDES=-I../../sim $(all_includes)

_core_la_METASOURCES = AUTO
_core_la_LIBADD = ../../sim/libsim.la  $(LIBUI) $(LIB_QT) $(LIB_KDECORE) $(LIB_KDEUI) $(LIB_KFILE) $(EXTRA_LIBS)  
_core_la_LDFLAGS = -module -no-undefined -avoid-version -L../../sim/.libs $(QT_LDFLAGS) $(KDE_RPATH) $(all_libraries)



