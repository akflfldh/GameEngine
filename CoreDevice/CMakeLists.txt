add_library(CoreDevice SHARED
CoreDevice.h 
CoreDeviceDllMacro.h 
CoreDevice.cpp
)


target_compile_definitions(CoreDevice PRIVATE CORE_DEVICE_LIB)


if(MSVC)
    target_sources(CoreDevice PUBLIC 
    D3DCoreDevice.h
    PRIVATE
    D3DCoreDevice.cpp)

target_link_libraries(CoreDevice PUBLIC 
d3d12
dxgi )


endif()

add_library(pix SHARED IMPORTED)

set_target_properties(pix PROPERTIES
IMPORTED_IMPLIB_DEBUG "${CMAKE_SOURCE_DIR}/Pix/WinPixEventRuntime.lib"
IMPORTED_IMPLIB_RELEASE "${CMAKE_SOURCE_DIR}/Pix/WinPixEventRuntime.lib"
IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/Pix/WinPixEventRuntime.dll")

target_include_directories(CoreDevice PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_definitions(CoreDevice PRIVATE CORE_DEVICE_LIB)

target_link_libraries(CoreDevice PUBLIC compiler_flags headerDir pix )


