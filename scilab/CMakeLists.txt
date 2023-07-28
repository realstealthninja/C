# If necessary, use the RELATIVE flag, otherwise each source file may be listed
# with full pathname. The RELATIVE flag makes it easier to extract an executable's name
# automatically.
include(GNUInstallDirs)
file(GLOB APP_SOURCES RELATIVE . *.c)
# file( GLOB APP_SOURCES ${CMAKE_SOURCE_DIR}/*.c )
# AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR} APP_SOURCES)
# I used a simple string replace, to cut off .cpp.
add_library(forkwindows SHARED forkwindows.h forkwindows.c bool.h)
set_target_properties(forkwindows PROPERTIES PUBLIC_HEADER forkwindows.h)
install(TARGETS forkwindows
        LIBRARY DESTINATION ${CMAKE_LIBRARY_PATH}
        PUBLIC_HEADER DESTINATION ${CMAKE_INCLUDE_PATH}/scilib)
install(FILES forkwindows.h bool.h
        DESTINATION ${CMAKE_INCLUDE_PATH}/scilib)
