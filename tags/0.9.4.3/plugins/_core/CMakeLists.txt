#################
# _core library #
#################
ADD_SUBDIRECTORY(pict)
ADD_SUBDIRECTORY(styles)

SET(_core_SRCS
        arcfg.cpp
        autoreply.cpp
        cfgdlg.cpp
        cmenu.cpp
        commands.cpp
        connectionsettings.cpp
        connectwnd.cpp
        container.cpp
        core.cpp
        declinedlg.cpp
        editmail.cpp
        editphone.cpp
        filecfg.cpp
        filetransfer.cpp
        history.cpp
        historycfg.cpp
        historywnd.cpp
        interfacecfg.cpp
        kpopup.cpp
        logindlg.cpp
        maininfo.cpp
        mainwin.cpp
        manager.cpp
        msgauth.cpp
        msgcfg.cpp
        msgcontacts.cpp
        msgedit.cpp
        msgfile.cpp
        msggen.cpp
        msgrecv.cpp
        msgsms.cpp
        msgurl.cpp
        msgview.cpp
        newprotocol.cpp
        nonim.cpp
        pagerdetails.cpp
        phonedetails.cpp
        plugincfg.cpp
        prefcfg.cpp
        search.cpp
        searchall.cpp
        smscfg.cpp
        status.cpp
        statuswnd.cpp
        tmpl.cpp
        toolbarcfg.cpp
        toolsetup.cpp
        usercfg.cpp
        userhistorycfg.cpp
        userlist.cpp
        userview.cpp
        userviewcfg.cpp
        userwnd.cpp
)

# is this really needed?
IF(NOT WIN32)
SET(_core_SRCS
        ${_core_SRCS}
        libintl.cpp
    )
ENDIF(NOT WIN32)

SET(_core_HDRS
        arcfg.h
        autoreply.h
        cfgdlg.h
        cmenu.h
        commands.h
        connectionsettings.h
        connectwnd.h
        container.h
        core.h
        core.h
        declinedlg.h
        editmail.h
        editphone.h
        filecfg.h
        filetransfer.h
        history.h
        historycfg.h
        historywnd.h
        interfacecfg.h
        kpopup.h
        logindlg.h
        maininfo.h
        mainwin.h
        manager.h
        msgauth.h
        msgcfg.h
        msgcontacts.h
        msgedit.h
        msgfile.h
        msggen.h
        msgrecv.h
        msgsms.h
        msgurl.h
        msgview.h
        newprotocol.h
        nonim.h
        pagerdetails.h
        phonedetails.h
        plugincfg.h
        prefcfg.h
        search.h
        searchall.h
        smscfg.h
        status.h
        statuswnd.h
        tmpl.h
        toolbarcfg.h
        toolsetup.h
        usercfg.h
        userhistorycfg.h
        userlist.h
        userview.h
        userviewcfg.h
        userwnd.h
)

SET(_core_UICS
        arcfgbase.ui
        autoreplybase.ui
        cfgdlgbase.ui
        connectionsettingsbase.ui
        connectwndbase.ui
        declinedlgbase.ui
        editmailbase.ui
        editphonebase.ui
        filecfgbase.ui
        filetransferbase.ui
        fontconfigbase.ui
        historycfgbase.ui
        interfacecfgbase.ui
        logindlgbase.ui
        maininfobase.ui
        managerbase.ui
        msgcfgbase.ui
        newprotocolbase.ui
        nonimbase.ui
        pagerbase.ui
        phonebase.ui
        plugincfgbase.ui
        prefcfgbase.ui
        searchallbase.ui
        searchbase.ui
        smscfgbase.ui
        toolsetupbase.ui
        userhistorycfgbase.ui
        userviewcfgbase.ui
)

# so we don't need an extra CMakeLists.txt in every subdirectory
ADD_JISP_ARCHIVE(additional additional.jisp _core_SRCS)
ADD_JISP_ARCHIVE(icq5       icq5.jisp       _core_SRCS)
ADD_JISP_ARCHIVE(icq5.1     icq5.1.jisp     _core_SRCS)
ADD_JISP_ARCHIVE(icqlite    icqlite.jisp    _core_SRCS)
ADD_JISP_ARCHIVE(jisp       sim.jisp        _core_SRCS)
ADD_JISP_ARCHIVE(smiles     smiles.jisp     _core_SRCS)
ADD_JISP_ARCHIVE(GPL-Icons  GPL-Icons.jisp  _core_SRCS)

SIM_ADD_PLUGIN(_core)
