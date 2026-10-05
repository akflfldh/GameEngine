add_library(NetWork SHARED
NetWorkSystem.h
NetWorkSystem.cpp
NetWorkDllMacro.h
 "INetWorkTransport.h" "INetWorkTransport.cpp" "WinNetWorkTransport.h" "WinNetWorkTransport.cpp" "NetWorkType.h")


target_compile_definitions(NetWork PRIVATE NETWORK_EXPORT_LIB)

target_link_libraries(NetWork PUBLIC compiler_flags headerDir)

target_include_directories(NetWork PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})



if(WIN32)
    target_link_libraries(NetWork  PRIVATE ws2_32)
endif()