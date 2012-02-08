###############
# history library #
###############
SET(history_SRCS
	historyplugin.cpp
	historystorage.cpp
	sqlitehistorystorage.cpp
	
	historybrowser.cpp
	
	${CMAKE_SOURCE_DIR}/sqlite/sqlite3.c
)

SET(history_HDRS
	historyplugin.h
	historystorage.h
	sqlitehistorystorage.h
	
	historybrowser.h	
)

SET(history_UICS
	historybrowser.ui
)

SIM_ADD_PLUGIN(history)
SET_TARGET_PROPERTIES(history PROPERTIES DEFINE_SYMBOL HISTORY_EXPORTS)
target_link_libraries(history _core)

SET(history_test_headers
    tests/qt-gtest.h
)

QT4_WRAP_CPP(testmocs ${history_test_headers})

SET(history_test_sources
    tests/qt-gtest.cpp
    tests/test.cpp
    
	tests/testhistoryplugin.cpp
    
    ${CMAKE_SOURCE_DIR}/testing/gtest/src/gtest-all.cc        
    ${CMAKE_SOURCE_DIR}/testing/src/gmock-all.cc
)

ADD_EXECUTABLE(test_history ${history_test_sources} ${testmocs})
TARGET_LINK_LIBRARIES(test_history simlib ${QT_LIBRARIES} history _core)

ADD_CUSTOM_COMMAND(TARGET test_history POST_BUILD COMMAND test_history)
