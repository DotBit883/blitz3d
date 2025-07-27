cmake_minimum_required(VERSION 3.16)
project(Blitz3D)

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
add_subdirectory(compiler)
# add_subdirectory(debugger)
# add_subdirectory(gxruntime)
add_subdirectory(linker)
add_subdirectory(linker_dll)
