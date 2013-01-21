##################
# jabber library #
##################
SET(jabber_SRCS
#        discoinfo.cpp
#        httppoll.cpp
#        infoproxy.cpp
        jabber.cpp
#        jabber_auth.cpp
#        jabber_rosters.cpp
#        jabber_ssl.cpp
#        jabberaboutinfo.cpp
#        jabberadd.cpp
#        jabberbrowser.cpp
#        jabberbuffer.cpp
        jabberclient.cpp
        jabbercontact.cpp
        jabbergroup.cpp
#        jabberconfig.cpp
#        jabberfiletransfer.cpp
#        jabberhomeinfo.cpp
#        jabberinfo.cpp
#        jabbermessage.cpp
#        jabberpicture.cpp
#        jabbersearch.cpp
        jabberstatus.cpp
        jabberstatuswidget.cpp
#        jabberworkinfo.cpp
#        jidadvsearch.cpp
#        jidsearch.cpp
        protocol/inputstreamdispatcher.cpp
        protocol/iqcontroller.cpp
        protocol/iqrequest.cpp
        protocol/jabberauthenticationcontroller.cpp
        protocol/taghandler.cpp
        protocol/xmlelement.cpp
        network/standardjabbersocket.cpp
        ui/jabberloginwidget.cpp
)

SET(jabber_HDRS
#        discoinfo.h
#        infoproxy.h
        jabber.h
        jabber_api.h
#        jabber_events.h
#        jabber_ssl.h
#        jabberaboutinfo.h
#        jabberadd.h
#        jabberbrowser.h
#        jabberbuffer.h
        jabberclient.h
        jabbercontact.h
        jabbergroup.h
#        jabberconfig.h
#        jabberhomeinfo.h
#        jabberinfo.h
#        jabbermessage.h
#        jabberpicture.h
#        jabbersearch.h
        jabberstatus.h
        jabberstatuswidget.h
#        jabberworkinfo.h
#        jidadvsearch.h
#        jidsearch.h
        protocol/inputstreamdispatcher.h
        protocol/iqcontroller.h
        protocol/iqrequest.h
        protocol/jabberauthenticationcontroller.h
        protocol/taghandler.h
        protocol/xmlelement.h
        network/jabbersocket.h
        network/standardjabbersocket.h
        ui/jabberloginwidget.h
)

SET(jabber_UICS
#        discoinfobase.ui
#        infoproxybase.ui
#        jabberaboutinfobase.ui
#        jabberaddbase.ui
#        jabberconfigbase.ui
#        jabberhomeinfobase.ui
#        jabberinfobase.ui
#        jabberpicturebase.ui
#        jabberworkinfobase.ui
#        jidadvsearchbase.ui
#        jidsearchbase.ui
        ui/jabberloginwidget.ui
)

set (Boost_USE_STATIC_LIBS ON)
set (Boost_USE_MULTITHREADED ON)

find_package(Boost 1.49 REQUIRED system)

include_directories(${Boost_INCLUDE_DIRS})

SIM_ADD_PLUGIN(jabber)
SET_TARGET_PROPERTIES(jabber PROPERTIES DEFINE_SYMBOL JABBER_EXPORTS)

SET(jabber_test_sources
    tests/test.cpp
    tests/testjabbercontact.cpp
    tests/testjabberclient.cpp
    tests/testjabberstatus.cpp
    tests/testjabbergroup.cpp
    tests/testinputstreamdispatcher.cpp
    tests/testjabberauthenticationcontroller.cpp
    
    ${CMAKE_SOURCE_DIR}/testing/gtest/src/gtest-all.cc        
    ${CMAKE_SOURCE_DIR}/testing/src/gmock-all.cc
)

ADD_EXECUTABLE(test_jabber ${jabber_test_sources})
TARGET_LINK_LIBRARIES(test_jabber simlib ${QT_LIBRARIES} jabber)

ADD_CUSTOM_COMMAND(TARGET test_jabber POST_BUILD COMMAND test_jabber)

ADD_TEST(testJabber test_jabber)
