#################
# remote plugin #
#################

SET(remote_PLATFORMS COMMON)

SET(remote_SRCS_COMMON remote.cpp
                       remotecfg.cpp)

SET(remote_HDRS_COMMON remote.h
                       remotecfg.h)

SET(remote_UICS_COMMON remotecfgbase.ui)

SIM_ADD_PLUGIN(remote)

#########################
# next target - simctrl #
#########################

PROJECT(remote_ctrl)

ADD_EXECUTABLE(simctrl simctrl.cpp)
ADD_TO_DIST_TARGET("${CMAKE_CURRENT_SOURCE_DIR}/simctrl.cpp")

IF(SIM_ENABLE_FPIE)
    SET_TARGET_PROPERTIES(simctrl PROPERTIES COMPILE_FLAGS "${SIM_CXX_FPIE_FLAGS}")
    SET_TARGET_PROPERTIES(simctrl PROPERTIES LINK_FLAGS "${SIM_PIE_LDFLAGS}")
ENDIF(SIM_ENABLE_FPIE)
TARGET_LINK_LIBRARIES(simctrl simlib)
INSTALL(TARGETS simctrl RUNTIME DESTINATION ${BINDIR})
IF(NOT WIN32)
    INSTALL(FILES simctrl.desktop DESTINATION ${SERVICES_INSTALL_DIR})
ENDIF(NOT WIN32)
ADD_TO_DIST_TARGET("${CMAKE_CURRENT_SOURCE_DIR}/simctrl.desktop")

IF(0)
#doesn't work yet
########################
# next target - simext #
########################

ADD_EXECUTABLE(simext simext.idl SIM_ext.cpp simext.cpp simext.rc)
INSTALL(TARGETS simext RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX})

########################
# next target - simipc #
########################

ADD_EXECUTABLE(simipc simipc.cpp)
INSTALL(TARGETS simipc RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX})

###########################
# next target - simremote #
###########################

ADD_EXECUTABLE(simremote simremote.idl sim_remote.cpp simControl.cpp simremote.rc)
INSTALL(TARGETS simremote RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX})

ENDIF(0)
