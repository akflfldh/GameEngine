add_library(Window STATIC
BaseWindow.cpp
BaseWindow.h "IWindowEventHandler.h")
target_include_directories(Window PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(Window PUBLIC  compiler_flags headerDir)
