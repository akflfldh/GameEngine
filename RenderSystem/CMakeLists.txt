add_library(RenderSystem SHARED
IRenderSystem.h 
RenderType.h
RenderSystemInternalHelper.h 
RenderSystemDllMacro.h 
MaterialType.h 
IMaterialManager.h 
IMaterialManager.cpp 
IRenderSystem.cpp 
RenderSystemInternalHelper.cpp 
 "IWindowRenderManager.h" "IWindowRenderManager.cpp")


if(MSVC)

    target_sources(RenderSystem PUBLIC
    ID3DRenderPass.h 
    D3DWindowRenderManager.h 
    D3DWindowRenderData.h 
    D3DShaderReflectSystem.h 
    D3DRenderType.h 
    D3DRenderSystem.h 
    D3DRenderPassManager.h 
    D3DRenderChannelSystem.h 
    D3DOpaqueRenderPass.h 
    D3DMaterialManager.h 
    )

    target_sources(RenderSystem PRIVATE
    ID3DRenderPass.cpp
    D3DWindowRenderManager.cpp 
    D3DWindowRenderData.cpp
    D3DShaderReflectSystem.cpp 
    D3DRenderSystem.cpp
    D3DRenderType.cpp
    D3DRenderPassManager.cpp
    D3DRenderChannelSystem.cpp 
    D3DOpaqueRenderPass.cpp
    D3DMaterialManager.cpp
    )



endif()

#add_library(pix SHARED IMPORTED)

#set_target_properties(pix PROPERTIES
#IMPORTED_IMPLIB_DEBUG "${CMAKE_SOURCE_DIR}/Pix/WinPixEventRuntime.lib"
#IMPORTED_IMPLIB_RELEASE "${CMAKE_SOURCE_DIR}/Pix/WinPixEventRuntime.lib"
#IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/Pix/WinPixEventRuntime.dll")



target_compile_definitions(RenderSystem PRIVATE RENDER_SYSTEM_LIB)

target_link_libraries(RenderSystem PUBLIC compiler_flags headerDir GpuResourceManager Logger d3d12 d3dcompiler CoreBase)

target_include_directories(RenderSystem PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})











