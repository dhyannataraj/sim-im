###############
# sms library #
###############
SET(sms_SRCS
        gsm_ta.cpp
        serial.cpp
        sms.cpp
        smssetup.cpp
)

SET(sms_HDRS
        gsm_ta.h
        serial.h
        sms.h
        smssetup.h
)

SET(sms_UICS
        smssetupbase.ui
)

# some needed include dirs
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/plugins/_core)

SIM_ADD_PLUGIN(sms)

ADD_DEPENDENCIES(sms _core)

# install some sounds
INSTALL(FILES sounds/phone.wav DESTINATION ${SIM_SOUND_DIR})
