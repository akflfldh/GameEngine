add_library(GpuResourceManager SHARED
IGpuResourceManager.h
IGpuResource.h
GRMPtr.h

GpuResourceDllMacro.h
IGpuResourceManager.cpp 
IGpuResource.cpp 
GRMPtr.cpp
 

GpuBufferContextSystem.cpp
GpuBufferContextSystem.h
GpuBufferContextSystemImpl.cpp
GpuBufferContextSystemImpl.h
GpuSamplerSystem.cpp 
GpuSamplerSystem.h 

)



get_filename_component(PARENT_DIR "${CMAKE_CURRENT_SOURCE_DIR}" DIRECTORY)



target_compile_definitions(GpuResourceManager PRIVATE GPURESOURCE_MANAGER_LIB)
target_include_directories(GpuResourceManager PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}" "${PARENT_DIR}/Include")

target_link_libraries(GpuResourceManager PUBLIC compiler_flags headerDir Logger CoreDevice JsonParser)




if(MSVC)

    target_sources(GpuResourceManager 
    PUBLIC 
    D3DGpuBuffer.h
    D3DGpuConstantBuffer.h
    D3DGpuDescriptorHeapManager.h 
    D3DGpuIndexBuffer.h 
    D3DGpuResource.h
    D3DGpuResourceManager.h 
    D3DGpuTexture.h 
    D3DGpuType.h 
    D3DGpuVertexBuffer.h 
    )

    target_sources(GpuResourceManager
    PRIVATE
    D3DGpuBuffer.cpp
    D3DGpuConstantBuffer.cpp
    D3DGpuDescriptorHeapManager.cpp
    D3DGpuIndexBuffer.cpp
    D3DGpuResource.cpp
    D3DGpuResourceManager.cpp 
    D3DGpuTexture.cpp
    D3DGpuType.cpp
    D3DGpuVertexBuffer.cpp
    )

endif()



