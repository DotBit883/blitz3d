add_library(stdutil OBJECT stdutil.cpp stdutil.h)
target_include_directories(stdutil PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(stdutil PUBLIC config)
