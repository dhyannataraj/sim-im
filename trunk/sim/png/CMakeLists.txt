IF(NOT WIN32)
  INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )
  INCLUDE(KDE4InstallIcons)
  KDE4_INSTALL_ICONS( ${CMAKE_INSTALL_PREFIX}/share/icons )
ENDIF(NOT WIN32)

FILE(GLOB _icons *.png)
FOREACH(_file ${_icons})
  ADD_TO_DIST_TARGET("${_file}")
ENDFOREACH(_file)
ADD_TO_DIST_TARGET("${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt")
