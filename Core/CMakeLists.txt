add_library(Core SHARED
Application.cpp 
GameTimer.cpp 
GlobalAppHelper.cpp 
GpuBufferContextSystem.cpp 
GpuBufferContextSystemImpl.cpp 
GpuSamplerSystem.cpp
 
IProgramDirector.cpp
ProjectConfig.cpp 
UIRenderItemBuilder.cpp 


Application.h 
GameTimer.h 
GlobalAppHelper.h
GpuBufferContextSystem.h
GpuBufferContextSystemImpl.h 
GpuSamplerSystem.h 
 
IProgramDirector.h 
MeshType.h 
ProjectConfig.h 
UIRenderItemBuilder.h 
CoreDllExport.h
 "ViewportController.h" "ViewportController.cpp" "CoreType.h"  "LogicalWindow.h" "LogicalWindow.cpp")


target_compile_definitions(Core PRIVATE CORE_EXPORT_LIB)

target_include_directories(Core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


target_link_libraries(Core PUBLIC compiler_flags headerDir Utility dxgi d3dcompiler 
CoreAsset Logger GpuResourceManager RenderSystem JsonParser BinaryReaderWriter
UiSystem SystemInitializer InputSystem)
