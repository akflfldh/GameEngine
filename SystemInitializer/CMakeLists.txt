add_library(SystemInitializer SHARED

ISystemInitializer.h
SystemInitializerDll.h
ISystemInitializer.cpp
)



if(MSVC)
    target_sources(SystemInitializer PUBLIC
        D3DSystemInitializer.h
    )
    target_sources(SystemInitializer PRIVATE
          D3DSystemInitializer.cpp
    )

endif()


target_include_directories(SystemInitializer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_definitions(SystemInitializer PRIVATE SYSTEM_INITIALIZER_LIB)

target_link_libraries(SystemInitializer PUBLIC compiler_flags headerDir GpuResourceManager RenderSystem CoreDevice d3d12 dxgi)