##################
# jabber library #
##################
SET(jabber_PLATFORMS COMMON)

SET(jabber_SRCS_COMMON discoinfo.cpp
                       httppoll.cpp
                       infoproxy.cpp
                       jabber.cpp
                       jabber_auth.cpp
                       jabber_rosters.cpp
                       jabber_ssl.cpp
                       jabberaboutinfo.cpp
                       jabberadd.cpp
                       jabberbrowser.cpp
                       jabberbuffer.cpp
                       jabberclient.cpp
                       jabberconfig.cpp
                       jabberfiletransfer.cpp
                       jabberhomeinfo.cpp
                       jabberinfo.cpp
                       jabbermessage.cpp
                       jabberpicture.cpp
                       jabbersearch.cpp
                       jabberworkinfo.cpp
                       jidadvsearch.cpp
                       jidsearch.cpp)

SET(jabber_HDRS_COMMON discoinfo.h
                       infoproxy.h
                       jabber.h
                       jabber_events.h
                       jabber_ssl.h
                       jabberaboutinfo.h
                       jabberadd.h
                       jabberbrowser.h
                       jabberbuffer.h
                       jabberclient.h
                       jabberconfig.h
                       jabberhomeinfo.h
                       jabberinfo.h
                       jabbermessage.h
                       jabberpicture.h
                       jabbersearch.h
                       jabberworkinfo.h
                       jidadvsearch.h
                       jidsearch.h)

SET(jabber_UICS_COMMON discoinfobase.ui
                       infoproxybase.ui
                       jabberaboutinfobase.ui
                       jabberaddbase.ui
                       jabberconfigbase.ui
                       jabberhomeinfobase.ui
                       jabberinfobase.ui
                       jabberpicturebase.ui
                       jabberworkinfobase.ui
                       jidadvsearchbase.ui
                       jidsearchbase.ui)

SIM_ADD_PLUGIN(jabber)

