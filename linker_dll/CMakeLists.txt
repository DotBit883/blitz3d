add_library(linker_dll SHARED linker_dll.cpp)
set_property(TARGET linker PROPERTY POSITION_INDEPENDENT_CODE ON)
target_link_libraries(linker_dll PUBLIC linker)
