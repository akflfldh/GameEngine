add_library(RenderFrontend SHARED

RenderFrontendDllMarco.h
UIRenderItemBuilder.cpp
UIRenderItemBuilder.h
AssetResolver.h
AssetResolver.cpp
 "ObjectRenderItemBuilder.h" "ObjectRenderItemBuilder.cpp" "RenderProxy.h" "RenderPipeline.h" "RenderPipeline.cpp" "RenderPass.h" "RenderPass.cpp" "RenderPassMain.h" "RenderPassMain.cpp" "RenderPipelineOpaque.h" "RenderPipelineOpaque.cpp" "RenderPipelineManager.h" "RenderPipelineManager.cpp" "RenderFrontendType.h" "RenderPassGraph.h" "RenderPassGraph.cpp" "RenderPipelineUI.h" "RenderPipelineUI.cpp" "RenderPassUI.h" "RenderPassUI.cpp" "RenderThread.h" "RenderThread.cpp"   "GrayScaleRenderPass.h" "GrayScaleRenderPass.cpp" "RenderOutlinePass.h" "RenderOutlinePass.cpp" "RenderEditOverlayPass.h" "RenderEditOverlayPass.cpp" "ShaderData.h" "RenderUploadManager.h" "RenderUploadManager.cpp" "RenderDebugGridPass.h" "RenderDebugGridPass.cpp" "BillboardRenderPass.h" "BillboardRenderPass.cpp" "DebugLineRenderPass.h" "DebugLineRenderPass.cpp" "RenderMaterialResolver.h" "RenderMaterialResolver.cpp" "SkySphereRenderPass.h" "SkySphereRenderPass.cpp")


add_compile_definitions(RenderFrontend PRIVATE RENDER_FRONTEND_LIB)

target_link_libraries(RenderFrontend PUBLIC compiler_flags headerDir RenderSystem GpuResourceManager CoreAsset RenderSystem UiSystem Core CoreMath)