set(SRCS

EditorShaderImporter.h
EditorTextureImporter.h
EditorProjectManager.h

EditorDirector.h
ImGuiSystem.h
SerializedAssetContainer.h
SuperAssetBrowerController.h
SuperController.h
SuperFrameController.h



EditorDirector.cpp
EditorProjectManager.cpp
EditorShaderImporter.cpp
EditorTextureImporter.cpp
ImGuiSystem.cpp
SerializedAssetContainer.cpp
SuperAssetBrowerController.cpp
SuperController.cpp
SuperFrameController.cpp

 "EditorConfig.h" "EditorConfig.cpp" "FNameTableFileIOManager.h" "FNameTableFileIOManager.cpp" "EditorAssetImporterModule.h" "EditorAssetImporterModule.cpp")

if(MSVC)
    add_executable(EditorDirector WIN32
    ${SRCS})

endif()



set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/EditorDirector/ImGui)


target_sources(EditorDirector PRIVATE
${IMGUI_DIR}/imgui.cpp
${IMGUI_DIR}/imgui.h
${IMGUI_DIR}/imgui_demo.cpp
${IMGUI_DIR}/imgui_draw.cpp
${IMGUI_DIR}/imgui_impl_dx12.cpp
${IMGUI_DIR}/imgui_impl_dx12.h
${IMGUI_DIR}/imgui_impl_win32.cpp
${IMGUI_DIR}/imgui_impl_win32.h
${IMGUI_DIR}/imgui_internal.h
${IMGUI_DIR}/imgui_tables.cpp
${IMGUI_DIR}/imgui_widgets.cpp
${IMGUI_DIR}/imstb_rectpack.h
${IMGUI_DIR}/imstb_textedit.h
${IMGUI_DIR}/imstb_truetype.h
)


target_include_directories(EditorDirector PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}  ${CMAKE_CURRENT_SOURCE_DIR}/ImGui)

target_link_libraries(EditorDirector PUBLIC compiler_flags headerDir
Utility Core Window LogicalFileSystem JsonParser RenderSystem GpuResourceManager SystemInitializer CoreAsset
UiSystem ImportModule CoreBase RenderFrontend)