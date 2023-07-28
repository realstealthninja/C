include(GNUInstallDirs)
file(GLOB APP_SOURCES RELATIVE . *.c)

include_directories("${CMAKE_CURRENT_BINARY_DIR}")
add_library(forkwindows SHARED forkwindows.c forkwindows.h)
generate_export_header(forkwindows
    BASE_NAME forkwindows
    EXPORT_MACRO_NAME FORK_WINDOWS
    EXPORT_FILE_NAME forkwindows.h
    STATIC_DEFINE SHARED_EXPORTS_BUILT_AS_STATIC)
