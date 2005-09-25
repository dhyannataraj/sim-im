######################################################################
# created by Christian Ehrlicher
######################################################################

TEMPLATE     = app
TARGET       = sim
DESTDIR      = ../bin
DEPENDPATH  += . 
INCLUDEPATH += .
PRECOMPILED_HEADER = simapi_pch.h

include(../sim.pri)

win32 {
    # add win32 folder
    DEPENDPATH += win32 win32\libxml win32\libxslt win32\openssl
    INCLUDEPATH += win32 win32\libxml win32\libxslt win32\openssl
    # defines for ltdl.c - why do we need this crap? QLibrary is better imho
    DEFINES += LTDL_SHLIB_EXT=\".dll\" LTDL_OBJDIR=\".\" HAVE_STDIO_H HAVE_STDLIB_H HAVE_STRING_H
    #others
    DEFINES += SIMAPI_EXPORTS
   
    QMAKE_LIBS += win32\libeay32.lib win32\libxml.lib win32\libxslt.lib

}

# Input
HEADERS += aboutdata.h \
           ballonmsg.h \
           buffer.h \
           cjk_variants.h \
           compatqtversion.h \
           datepicker.h \
           editfile.h \
           exec.h \
           fetch.h \
           fontedit.h \
           html.h \
           icons.h \
           intedit.h \
           johab_hangul.h \
           kdeisversion.h \
           linklabel.h \
           listview.h \
           ltdl.h \
           preview.h \
           qchildwidget.h \
           qcolorbutton.h \
           qkeybutton.h \
           qzip.h \
           sax.h \
           simapi.h \
           socket.h \
           sockfactory.h \
           stl.h \
           textshow.h \
           toolbtn.h \
           translit.h \
           unzip.h \
           xsl.h
SOURCES += aboutdata.cpp \
           ballonmsg.cpp \
           buffer.cpp \
           cfg.cpp \
           cmddef.cpp \
           contacts.cpp \
           country.cpp \
           datepicker.cpp \
           editfile.cpp \
           exec.cpp \
           fetch.cpp \
           fontedit.cpp \
           icons.cpp \
           intedit.cpp \
           linklabel.cpp \
           listview.cpp \
           log.cpp \
           ltdl.c \
           message.cpp \
           plugins.cpp \
           preview.cpp \
           qchildwidget.cpp \
           qcolorbutton.cpp \
           qkeybutton.cpp \
           qzip.cpp \
           sax.cpp \
           sim.cpp \
           simapi.cpp \
           socket.cpp \
           sockfactory.cpp \
           sslclient.cpp \
           textshow.cpp \
           toolbtn.cpp \
           translit.cpp \
           unquot.cpp \
           unzip.c \
           xsl.cpp

LEXSOURCES  = html.ll