cmake_minimum_required(VERSION 4.0)
project(Blitz3D CXX)

set(BUILD_EDITION "PRO" CACHE STRING "Choose the build edition")
set_property(CACHE BUILD_EDITION PROPERTY STRINGS PRO DEMO EDU)

if(BUILD_EDITION STREQUAL "PRO")
    add_compile_definitions(PRO)
elseif(BUILD_EDITION STREQUAL "DEMO")
    add_compile_definitions(DEMO)
elseif(BUILD_EDITION STREQUAL "EDU")
    add_compile_definitions(EDU)
else()
    message(FATAL_ERROR "Unknown edition: ${BUILD_EDITION}")
endif()

option(ENABLE_MEMDEBUG "Enable memory debugging" OFF)
if(ENABLE_MEMDEBUG)
    add_compile_definitions(MEMDEBUG)
endif()

message(STATUS "Building edition: ${BUILD_EDITION}")

# Important
add_subdirectory(config)
add_subdirectory(stdutil)

# Executables
add_subdirectory(asm_makeinsts)
add_subdirectory(bblaunch)
# add_subdirectory(bbruntime)
# add_subdirectory(bbruntime_dll)
# add_subdirectory(blitz)
# add_subdirectory(blitz3d)
# add_subdirectory(blitzide)
# add_subdirectory(compiler)
# add_subdirectory(debugger)
# add_subdirectory(gxruntime)
# add_subdirectory(innosetup)
add_subdirectory(linker)
add_subdirectory(linker_dll)
