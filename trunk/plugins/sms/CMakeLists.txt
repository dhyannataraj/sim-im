###############
# sms library #
###############

SET(sms_PLATFORMS COMMON)


SET(sms_SRCS_COMMON gsm_ta.cpp
                    serial.cpp
                    sms.cpp
                    smssetup.cpp)

SET(sms_HDRS_COMMON gsm_ta.h
                    serial.h
                    sms.h
                    smssetup.h)

SET(sms_UICS_COMMON smssetupbase.ui)

# Do not know where xpm/simcard.xpm is used... it is not mentioned anywhere exept
# Makefile.am
# So I desided not to add it to make dist filelist. If I am wrong, please correct the code
# N. Shaplov

# some needed include dirs
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/plugins/_core)

SIM_ADD_PLUGIN(sms)

ADD_DEPENDENCIES(sms _core)

