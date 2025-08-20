add_library(CoreMath STATIC
CoreMath.cpp
CoreMath.h


)



target_link_libraries(CoreMath PUBLIC compiler_flags headerDir)

target_include_directories(CoreMath PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
