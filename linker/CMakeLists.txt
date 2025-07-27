add_library(linker OBJECT linker.cpp)
target_include_directories(linker PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(linker PUBLIC stdutil)
