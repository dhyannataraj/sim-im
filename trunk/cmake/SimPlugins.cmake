# Searching and enabling plugins
MACRO(SIM_ADD_PLUGIN _name)
    PROJECT(${_name})
    
    SET(_srcs ${${_name}_SRCS})
    SET(_hdrs ${${_name}_HDRS})
    SET(_uics ${${_name}_UICS})

    KDE3_AUTOMOC(${_srcs})
    
    QT3_ADD_UI_FILES(_srcs ${_uics})

    ADD_LIBRARY(${_name} SHARED ${_srcs} ${_hdrs})

    INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

    TARGET_LINK_LIBRARIES(${_name} simlib)

    SET_TARGET_PROPERTIES(${_name} PROPERTIES PREFIX "")

    INSTALL(TARGETS ${_name} LIBRARY DESTINATION ${SIM_PLUGIN_DIR} RUNTIME  DESTINATION ${SIM_PLUGIN_DIR})
ENDMACRO(SIM_ADD_PLUGIN)

MACRO(SIM_FIND_PLUGINS sim_plugin_dir)
    FILE(GLOB plugins_dir ${sim_plugin_dir}/* )
    FOREACH(_cur_dir ${plugins_dir})
        FILE(GLOB cmakefile ${_cur_dir}/CMakeLists.txt)
        IF(cmakefile)
            GET_FILENAME_COMPONENT(plugin ${_cur_dir} NAME_WE)
            
            STRING(TOUPPER ${plugin} uc_plugin)
            OPTION(USE_PLUGIN_${uc_plugin}  "Use plugin ${plugin}" ON)

            # all plugins starting with _ are *not* optional!
            IF(${uc_plugin} MATCHES "^_.*$")
                ## prepend
                SET(SIM_PLUGINS ${plugin} ${SIM_PLUGINS})
                MARK_AS_ADVANCED(USE_${uc_plugin})
            ELSE(${uc_plugin} MATCHES "^_.*$")
                ## append
                SET(SIM_PLUGINS ${SIM_PLUGINS} ${plugin})
            ENDIF(${uc_plugin} MATCHES "^_.*$")
            

        ENDIF(cmakefile)
    ENDFOREACH(_cur_dir)
ENDMACRO(SIM_FIND_PLUGINS sim_plugin_dir)

MACRO(SIM_INCLUDE_PLUGINS)
    FOREACH(plugin ${SIM_PLUGINS})
        STRING(TOUPPER ${plugin} uc_plugin)
        IF(USE_PLUGIN_${uc_plugin})
            MESSAGE(STATUS "using plugin " ${plugin})
            ADD_SUBDIRECTORY(plugins/${plugin})
        ELSE(USE_PLUGIN_${uc_plugin})
            MESSAGE(STATUS "skipping plugin " ${plugin})
        ENDIF(USE_PLUGIN_${uc_plugin})
    ENDFOREACH(plugin ${SIM_PLUGINS})
ENDMACRO(SIM_INCLUDE_PLUGINS)
