PROJECT(sim)

IF(NOT WIN32)
    ADD_SUBDIRECTORY(png)
ENDIF(NOT WIN32)

######################
# target sim library #
######################

SET(sim_LIB_SRCS
    	buffer.cpp
		builtinlogger.cpp
		cfg.cpp
		clients/client.cpp
		clients/clientmanager.cpp
        clients/standardclientmanager.cpp
        commands/commandhub.cpp
        commands/menuaction.cpp
        commands/simtoolbar.cpp
        commands/uicommandcontextprovider.cpp
        commands/uicommandlist.cpp
		contacts/contact.cpp
        contacts/contactlist.cpp
		contacts/group.cpp
        contacts/imcontact.cpp
        contacts/imcontactid.cpp
        contacts/imgroup.cpp
		contacts/imstatus.cpp
		contacts/packettype.cpp
		contacts/protocol.cpp
		contacts/protocolmanager.cpp
        contacts/standardcontactlist.cpp
		contacts/userdata.cpp
		country.cpp
		events/actioncollectionevent.cpp
        events/contactevent.cpp
        events/eventdata.cpp
        events/eventhub.cpp
        events/ievent.cpp
        events/logevent.cpp
        events/menuitemcollectionevent.cpp
        events/nulleventdata.cpp
        events/standardevent.cpp
        events/widgetcollectionevent.cpp
        imagestorage/avatarstorage.cpp
        imagestorage/iconset.cpp
        imagestorage/imagestorage.cpp
        imagestorage/jispiconset.cpp
        imagestorage/standardavatarstorage.cpp
        imagestorage/standardimagestorage.cpp
		log.cpp
        messaging/genericmessage.cpp
        messaging/message.cpp
        messaging/messageoutpipe.cpp
        messaging/messagepipe.cpp
        messaging/messageprocessor.cpp
        messaging/standardmessageoutpipe.cpp
        messaging/standardmessagepipe.cpp
		misc.cpp
        network/asyncsocket.cpp
        network/tcpasyncsocket.cpp
		paths.cpp
		plugins.cpp
		profile/profile.cpp
		profile/profilemanager.cpp
		profile/standardprofilemanager.cpp
		propertyhub.cpp
		qzip/ioapi.c
		qzip/qzip.cpp
		qzip/unzip.c
        services.cpp
		simfs.cpp
        simgui/ballonmsg.cpp
        simgui/genericmessageeditor.cpp
		simgui/linklabel.cpp
        simgui/messageeditor.cpp
		simgui/preview.cpp
		simgui/qchildwidget.cpp
		simgui/qcolorbutton.cpp
		simgui/qkeybutton.cpp
        simgui/statuswidget.cpp
		translit.cpp
        xsl.cpp

        ../sqlite/sqlite3.c
	)

SET(sim_LIB_MESSAGE_SOURCES  ${sim_LIB_SRCS} PARENT_SCOPE)

SET(sim_LIB_HDRS
    	buffer.h
		builtinlogger.h
		cfg.h
		clients/client.h
		clients/clientmanager.h
        clients/standardclientmanager.h
        commands/commandhub.h
        commands/menuaction.h
        commands/simtoolbar.h
        commands/uicommandcontextprovider.h
        commands/uicommandlist.h
		contacts/contact.h
        contacts/contactlist.h
		contacts/group.h
        contacts/imcontact.h
        contacts/imcontactid.h
        contacts/imgroup.h
		contacts/imstatus.h
		contacts/packettype.h
		contacts/protocol.h
		contacts/protocolmanager.h
        contacts/standardcontactlist.h
		contacts/userdata.h
		country.h
		events/actioncollectionevent.h
        events/contactevent.h
        events/eventdata.h
        events/eventhub.h
        events/ievent.h
        events/logevent.h
        events/menuitemcollectionevent.h
        events/nulleventdata.h
        events/standardevent.h
        events/widgetcollectionevent.h
        imagestorage/avatarstorage.h
        imagestorage/iconset.h
        imagestorage/imagestorage.h
        imagestorage/jispiconset.h
        imagestorage/standardavatarstorage.h
        imagestorage/standardimagestorage.h
		log.h
        messaging/genericmessage.h
        messaging/message.h
        messaging/messageoutpipe.h
        messaging/messagepipe.h
        messaging/messageprocessor.h
        messaging/standardmessageoutpipe.h
        messaging/standardmessagepipe.h
		misc.h
        network/asyncsocket.h
        network/tcpasyncsocket.h
		paths.h
		plugins.h
		profile/profile.h
		profile/profilemanager.h
		profile/standardprofilemanager.h
		propertyhub.h
		qzip/ioapi.h
		qzip/qzip.h
		qzip/unzip.h
        services.h
		simapi.h
		simfs.h
		simgui/ballonmsg.h
        simgui/genericmessageeditor.h
		simgui/linklabel.h
        simgui/messageeditorfactory.h
        simgui/messageeditor.h
		simgui/preview.h
		simgui/qchildwidget.h
		simgui/qcolorbutton.h
		simgui/qkeybutton.h
        simgui/statuswidget.h
		singleton.h
		translit.h
        xsl.h

        ../sqlite/sqlite3.h
    )


IF(ENABLE_KDE4)
	SET(sim_LIB_SRCS ${sim_LIB_SRCS} simapp/kdesimapp.cpp)
	SET(sim_LIB_HDRS ${sim_LIB_HDRS} simapp/kdesimapp.h)
ELSE(ENABLE_KDE4)
	SET(sim_LIB_SRCS ${sim_LIB_SRCS} simapp/simapp.cpp aboutdata.cpp)
	SET(sim_LIB_HDRS ${sim_LIB_HDRS} simapp/simapp.h aboutdata.h)
ENDIF(ENABLE_KDE4)

# moc the sources
QT4_WRAP_CPP(MOC_SRCS ${sim_LIB_HDRS})
QT4_WRAP_UI(sim_ui_headers ${sim_ui})
SOURCE_GROUP(MOC-Files FILES ${MOC_SRCS})

# don't forget flex
#ADD_FLEX_FILES(sim_LIB_SRCS html.ll)

# our target
ADD_LIBRARY(simlib SHARED ${sim_LIB_SRCS} ${sim_LIB_HDRS} ${MOC_SRCS} ${sim_ui_headers})

IF(WIN32 AND NOT MINGW)
    SET_TARGET_PROPERTIES(simlib PROPERTIES VERSION 0.0.0 SOVERSION 0)
ELSE(WIN32 AND NOT MINGW)
    SET_TARGET_PROPERTIES(simlib PROPERTIES OUTPUT_NAME sim${SIM_FLAVOUR} VERSION 0.0.0 SOVERSION 0)
ENDIF(WIN32 AND NOT MINGW)

#some needed defines
SET_TARGET_PROPERTIES(simlib PROPERTIES DEFINE_SYMBOL SIMAPI_EXPORTS)
ADD_DEFINITIONS(-DNOUNCRYPT)

# additional include dirs - at least ${CMAKE_CURRENT_BINARY_DIR} is needed
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

TARGET_LINK_LIBRARIES(simlib)

if(NOT WIN32)
    TARGET_LINK_LIBRARIES(simlib pthread dl)
endif(NOT WIN32)

IF(ENABLE_KDE4)
  TARGET_LINK_LIBRARIES(simlib ${KDE4_KDECORE_LIBRARY} ${KDE4_KDEUI_LIBRARY} ${KDE4_KIO_LIBRARY})
ENDIF(ENABLE_KDE4)

# we need wsock32 on win32
if (WIN32)
  TARGET_LINK_LIBRARIES(simlib wsock32 wininet)
endif(WIN32)

# libxml, libxslt and iconv
INCLUDE_DIRECTORIES(${LIBXML2_INCLUDE_DIR} ${LIBXSLT_INCLUDE_DIR} ${LIBICONV_INCLUDE_DIR} ${ZLIB_INCLUDE_DIR})
TARGET_LINK_LIBRARIES(simlib ${QT_LIBRARIES} ${LIBXML2_LIBRARIES} ${LIBXSLT_LIBRARIES} ${ZLIB_LIBRARIES})

IF(WIN32)
  TARGET_LINK_LIBRARIES(simlib ${LIBICONV_LIBRARIES})
ENDIF(WIN32)

# set correct install path
INSTALL(TARGETS simlib LIBRARY DESTINATION ${LIBDIR} RUNTIME DESTINATION ${LIBDIR} BUNDLE DESTINATION ${LIBDIR})

IF(WIN32)
    SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE})
ENDIF(WIN32)

IF(APPLE)
    SET(LIBRARY_OUTPUT_PATH ${LIBDIR})
ENDIF(APPLE)

# unit tests
SET(sim_tests_srcs
	tests/gtest-qt.cpp
    tests/test.cpp
    tests/testbuffer.cpp
    tests/testclientmanager.cpp
    tests/testcommandhub.cpp
    tests/testcontact.cpp
    tests/testgroup.cpp
    tests/testmessagepipe.cpp
    tests/teststandardcontactlist.cpp
    tests/teststandardmessageoutpipe.cpp
    tests/testimcontactid.cpp
    tests/teststandardavatarstorage.cpp
    tests/teststandardprofilemanager.cpp
    tests/testwidgetcollectionevent.cpp
    
    tests/spies/widgetcollectioneventreceiver.cpp
    
    ${CMAKE_SOURCE_DIR}/testing/gtest/src/gtest-all.cc        
    ${CMAKE_SOURCE_DIR}/testing/src/gmock-all.cc
)


SET(sim_tests_hdrs
    tests/mocks/mockclient.h
    tests/stubs/stubclient.h
    tests/stubs/stubimagestorage.h
    tests/stubs/stubimcontact.h
    tests/stubs/stubimgroup.h
    tests/stubs/stubmessage.h
    tests/mocks/mockcontactlist.h
    tests/mocks/mockimcontact.h
    tests/mocks/mockasyncsocket.h
    tests/mocks/mockmessageoutpipe.h
    tests/mocks/mockmessageprocessor.h
    tests/mocks/mockprofilemanager.h
    tests/testuicommand.h
    tests/gtest-qt.h
    tests/testwidgetcollectionevent.h
    tests/spies/widgetcollectioneventreceiver.h
)

QT4_WRAP_CPP(testmocs ${sim_tests_hdrs})

ADD_EXECUTABLE(test_sim ${sim_tests_srcs} ${testmocs})
TARGET_LINK_LIBRARIES(test_sim simlib ${QT_LIBRARIES} ${sim_ui_headers})

IF(NOT DISABLE_AUTOTEST)
  ADD_CUSTOM_COMMAND(TARGET test_sim POST_BUILD COMMAND test_sim)
ENDIF(NOT DISABLE_AUTOTEST)

ADD_TEST(testSIM test_sim)

##################
# sim executable #
##################

# main program
SET(sim_EXE_SRCS
    sim.cpp
)

if(WIN32)
  if(MINGW)
    # resource compilation for mingw
    ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/sim_rc.o
                       COMMAND windres.exe -I${CMAKE_CURRENT_SOURCE_DIR}
                                           -i${CMAKE_CURRENT_SOURCE_DIR}/sim.rc
                                           -o ${CMAKE_CURRENT_BINARY_DIR}/sim_rc.o)
    SET(sim_EXE_SRCS ${sim_EXE_SRCS} ${CMAKE_CURRENT_BINARY_DIR}/sim_rc.o)
  else(MINGW)
    SET(sim_EXE_SRCS ${sim_EXE_SRCS} sim.rc)
  endif(MINGW)
endif(WIN32)

# i18n
COMPILE_PO_FILES(${CMAKE_SOURCE_DIR}/po sim_EXE_SRCS)

# sim executable
ADD_EXECUTABLE(${PROGNAME} WIN32 MACOSX_BUNDLE ${sim_EXE_SRCS})

IF(SIM_ENABLE_FPIE)
    SET_TARGET_PROPERTIES(${PROGNAME} PROPERTIES COMPILE_FLAGS "${SIM_CXX_FPIE_FLAGS}")
    SET_TARGET_PROPERTIES(${PROGNAME} PROPERTIES LINK_FLAGS "${SIM_PIE_LDFLAGS}")
ENDIF(SIM_ENABLE_FPIE)

# only needs simlib
TARGET_LINK_LIBRARIES(${PROGNAME} simlib ${QT_LIBRARIES} ${QT_QTMAIN_LIBRARY})

# set correct install path
INSTALL(TARGETS ${PROGNAME} RUNTIME DESTINATION ${BINDIR} BUNDLE DESTINATION ${BINDIR})

SET_TARGET_PROPERTIES(${PROGNAME} PROPERTIES OUTPUT_NAME ${PROGNAME}${SIM_FLAVOUR})

IF(WIN32)
    SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE})
ENDIF(WIN32)

IF(NOT WIN32 AND NOT APPLE)
    INSTALL(FILES sim.desktop DESTINATION ${XDG_APPS_DIR})
ENDIF(NOT WIN32 AND NOT APPLE)

# some needed libs for win32
if(WIN32)
  INSTALL(FILES ${CMAKE_SOURCE_DIR}/install_win32/iconv.dll    DESTINATION ${BINDIR})
  INSTALL(FILES ${CMAKE_SOURCE_DIR}/install_win32/libeay32.dll DESTINATION ${BINDIR})
  INSTALL(FILES ${CMAKE_SOURCE_DIR}/install_win32/libxml2.dll  DESTINATION ${BINDIR})
  INSTALL(FILES ${CMAKE_SOURCE_DIR}/install_win32/libxslt.dll  DESTINATION ${BINDIR})
  INSTALL(FILES ${CMAKE_SOURCE_DIR}/install_win32/ssleay32.dll DESTINATION ${BINDIR})
  INSTALL(FILES ${CMAKE_SOURCE_DIR}/install_win32/zlib1.dll    DESTINATION ${BINDIR})
endif(WIN32)
