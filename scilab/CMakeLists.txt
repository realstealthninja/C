# If necessary, use the RELATIVE flag, otherwise each source file may be listed
# with full pathname. The RELATIVE flag makes it easier to extract an executable's name
# automatically.
include(GNUInstallDirs)
file( GLOB APP_SOURCES RELATIVE . *.c )
# file( GLOB APP_SOURCES ${CMAKE_SOURCE_DIR}/*.c )
# AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR} APP_SOURCES)
    # I used a simple string replace, to cut off .cpp.
add_library(forkwindows forkwindows.c forkwindows.h bool.h)

