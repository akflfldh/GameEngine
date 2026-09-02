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
   
 "EditorConfig.h" "EditorConfig.cpp" "FNameTableFileIOManager.h" "FNameTableFileIOManager.cpp" "EditorAssetImporterModule.h" "EditorAssetImporterModule.cpp" "EditorAssetManager.h" "EditorAssetManager.cpp")
     
if(MSVC)   
   add_executable(EditorDirector WIN32
    ${SRCS} "EditorSceneController.h" "EditorSceneController.cpp" "EditorFontImporter.h" "EditorFontImporter.cpp" "UIScrollBox.h" "UIScrollBox.cpp" "UIScrollControlComponent.h" "UIScrollControlComponent.cpp" "ObjectHierarchyPanel.h" "ObjectHierarchyPanel.cpp" "UIAssetBrowser.h" "UIAssetBrowser.cpp" "UISplitterPanel.h" "UISplitterPanel.cpp" "UISplitterComponent.h" "UISplitterComponent.cpp" "UITreeNode.h" "UITreeNode.cpp" "UIDirectoryTree.h" "UIDirectoryTree.cpp" "UIGridLayoutComponent.h" "UIGridLayoutComponent.cpp" "UIFileItem.h" "UIFileItem.cpp" "GlobalOverlayManager.h" "GlobalOverlayManager.cpp" "EditorUtility.h" "EditorUtility.cpp" "UIEditorDebugHUD.h" "UIEditorDebugHUD.cpp" "EditorSceneManager.h" "EditorSceneManager.cpp" "EditorMode.h" "EditorMode.cpp" "PropertyPanel.h" "PropertyPanel.cpp" "ObjectHierarchyItem.h" "ObjectHierarchyItem.cpp" "UIDragSourceComponent.h" "UIDragSourceComponent.cpp" "GlobalOverlayType.h" "UISelectableComponent.h" "UISelectableComponent.cpp" "UIDropTargetComponent.h" "UIDropTargetComponent.cpp" "UIReflectPanel.h" "UIReflectPanel.cpp" "UIReflectSinglePrimitivePanel.h" "UIReflectSinglePrimitivePanel.cpp" "UIReflectVector3Panel.h" "UIReflectVector3Panel.cpp" "UIBoolPanel.h" "UIBoolPanel.cpp" "UIReflectBoolPanel.h" "UIReflectBoolPanel.cpp" "UIFoldoutPanel.h" "UIFoldoutPanel.cpp" "UIReflectVectorPanel.h" "UIReflectVectorPanel.cpp" "UIReflectPanelFactory.h" "UIReflectPanelFactory.cpp" "IPropertyBindable.h" "IPropertyBindable.cpp" "EditorSelectionManager.h" "EditorSelectionManager.cpp" "TransformGizmo.h" "TransformGizmo.cpp" "GizmoMeshComponent.h" "GizmoMeshComponent.cpp" "ProjectGenerator.h" "ProjectGenerator.cpp" "UIProjectBrowser.h" "UIProjectBrowser.cpp" "EditorProjectBrowserManager.h" "EditorProjectBrowserManager.cpp" "UIProjectItem.h" "UIProjectItem.cpp" "UIHierarchyItem.h" "UIHierarchyItem.cpp" "ClassListUIScrollPanel.h" "ClassListUIScrollPanel.cpp" "TaskUIController.h" "TaskUIController.cpp" "PrefabGenerationManager.h" "PrefabGenerationManager.cpp"   "PrefabWorkSpaceManager.h" "PrefabWorkSpaceManager.cpp" "ComponentScrollPanel.h" "ComponentScrollPanel.cpp" "PrefabWorkSpaceData.h" "PrefabWorkSpaceData.cpp" "StaticMeshComponentUIReflectPanel.h" "StaticMeshComponentUIReflectPanel.cpp" "EditorInspectorUtility.h" "EditorInspectorUtility.cpp" "EditorPlayMode.h" "EditorPlayMode.cpp" "EditorEditMode.h" "EditorEditMode.cpp" "MapPlaySettingPanel.cpp" "MapPlaySettingPanel.h" "UIDropdown.h" "UIDropdown.cpp" "UISearchSelectBox.h" "UISearchSelectBox.cpp" "CameraComponentVisualizer.h" "CameraComponentVisualizer.cpp" "EditorCameraVisualizerObject.h" "EditorCameraVisualizerObject.cpp" "EditorDeleteCommand.h" "EditorDeleteCommand.cpp" "LightComponentVisualizer.h" "LightComponentVisualizer.cpp" "EditorLightVisualizerObject.h" "EditorLightVisualizerObject.cpp" "EditorVisualizerManager.h" "EditorVisualizerManager.cpp" "IEditorVisualizer.h" "IEditorVisualizer.cpp" "PlayerStartVisualizer.h" "PlayerStartVisualizer.cpp" "EditorPlayerStartVisualizerObject.h" "EditorPlayerStartVisualizerObject.cpp" "EditorHitProxyComponent.h" "EditorHitProxyComponent.cpp" "IEditorVisualizerObject.h" "IEditorVisualizerObject.cpp" "EditorDebugDraw.h" "EditorDebugDraw.cpp" "ImportCommonHeader.h" "ClassGenerator.h" "ClassGenerator.cpp" "EditorAssetImporterManager.h" "EditorAssetImporterManager.cpp" "IEditorTaskManager.h" "IEditorTaskManager.cpp" "ClassGenerationManager.h" "ClassGenerationManager.cpp" "EditorAssetImportFinalizer.h" "EditorAssetImportFinalizer.cpp" "MaterialWorkSpaceManager.h" "MaterialWorkSpaceManager.cpp" "MaterialEditUIController.h" "MaterialEditUIController.cpp" "UIAssetSlotPanel.h" "UIAssetSlotPanel.cpp" "UIAssetSlotListPanel.h" "UIAssetSlotListPanel.cpp" "UIReflectFloatPanel.h" "UIReflectFloatPanel.cpp" "MapSettingUIController.h" "MapSettingUIController.cpp" "DefaultEditorInspectorManager.h" "DefaultEditorInspectorManager.cpp" "IInspector.h" "IInspector.cpp" "DefaultPropertyInspector.h" "DefaultPropertyInspector.cpp" "UIReflectTextureSlotPanel.h" "UIReflectTextureSlotPanel.cpp" "EditorMain.cpp")

endif() 

if(GAMEENGINE_DEVELOPMENT_BUILD)
    set(GAMEENGINE_DEVELOPMENT_ROOT "${CMAKE_SOURCE_DIR}")
else()
    set(GAMEENGINE_DEVELOPMENT_ROOT "")
endif()



set(EDITOR_CONFIG_GENERATED_DIR "${CMAKE_CURRENT_BINARY_DIR}/GeneratedConfig")

file(MAKE_DIRECTORY "${EDITOR_CONFIG_GENERATED_DIR}")

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/EditorBuildConfig.h.in"
                "${EDITOR_CONFIG_GENERATED_DIR}/EditorBuildConfig.h" @ONLY)




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

target_include_directories(EditorDirector PRIVATE "${EDITOR_CONFIG_GENERATED_DIR}")
target_include_directories(EditorDirector PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}  ${CMAKE_CURRENT_SOURCE_DIR}/ImGui)

target_link_libraries(EditorDirector PUBLIC compiler_flags headerDir
Utility Core Window LogicalFileSystem JsonParser RenderSystem GpuResourceManager SystemInitializer CoreAsset
UiSystem ImportModule CoreBase RenderFrontend)





# (B) 외부 exe 또는 PATH에 있음:
set(CODEGEN_CMD "${CMAKE_CURRENT_SOURCE_DIR}/../CodeGenerator.exe")   # 경로 있으면 풀경로로 바꿔도 됨
set(UISYSTEM_GEN_DIR "${CMAKE_BINARY_DIR}/UiSystem/Generated")

# 2) 생성물 폴더 및 포함 (그대로 OK)
set(GEN_DIR ${CMAKE_CURRENT_BINARY_DIR}/Generated)
file(MAKE_DIRECTORY ${GEN_DIR})
target_include_directories(EditorDirector PUBLIC ${GEN_DIR})




# --- 리플렉션 대상 헤더 검색 ---
file(GLOB_RECURSE ALL_HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
string(REPLACE "\\" "/" SRC_DIR_REGEX "${CMAKE_SOURCE_DIR}")
list(FILTER ALL_HEADERS EXCLUDE REGEX "${SRC_DIR_REGEX}/out/.*")
list(FILTER ALL_HEADERS EXCLUDE REGEX "${GEN_DIR}/.*")
list(REMOVE_DUPLICATES ALL_HEADERS)

# --- REFLECT_CLASS 매크로가 있는 헤더만 선별 ---
set(REFLECT_TARGETS "")
foreach(HEADER_FILE IN LISTS ALL_HEADERS)
    file(READ "${HEADER_FILE}" FILE_CONTENT)
    string(REGEX MATCH "REFLECT_CLASS" IS_TARGET "${FILE_CONTENT}")
    if(IS_TARGET)
        list(APPEND REFLECT_TARGETS "${HEADER_FILE}")
    endif()
endforeach()

# --- 코드 생성 함수 ---
set(ALL_GPP_FILES "")
set(ALL_GH_FILES "") 






function(add_analysis_step INPUT_PATH)


        
    get_filename_component(FILE_BASENAME ${INPUT_PATH} NAME_WE)
    file(RELATIVE_PATH REL_PATH "${CMAKE_SOURCE_DIR}" "${INPUT_PATH}")
    string(REPLACE "/" "_" SAFE_BASE_NAME "${REL_PATH}")
    string(REPLACE "." "_" SAFE_BASE_NAME "${SAFE_BASE_NAME}")
    set(DEP_FILE "${GEN_DIR}/${SAFE_BASE_NAME}.d")
    set(OUT_GPP "${GEN_DIR}/${FILE_BASENAME}.generated.cpp")
    set(OUT_GH "${GEN_DIR}/${FILE_BASENAME}.generated.h") 

    # 하위 폴더 생성 보장 (중간 경로 포함)
    # ① 파일이 없을 경우 더미 파일 생성

    add_custom_command(
         OUTPUT      "${OUT_GPP}" "${OUT_GH}"
        COMMAND     ${CMAKE_COMMAND} -E touch "${OUT_GH}"

        COMMAND     ${CODEGEN_CMD} scan
                    --outputGpp   "${OUT_GPP}"
                    --outputGh    "${OUT_GH}"
                    --depfile     "${DEP_FILE}"
                    --deptarget   "${OUT_GPP}"
                    "${INPUT_PATH}"
                    -- -x c++ -std=c++20 --target=x86_64-pc-windows-msvc -D_DEBUG -I${CMAKE_SOURCE_DIR} -I${GEN_DIR} -I${CMAKE_CURRENT_SOURCE_DIR} -I${CMAKE_BINARY_DIR}/Core/Generated -I${UISYSTEM_GEN_DIR}
        DEPENDS     "${INPUT_PATH}"
        DEPFILE     "${DEP_FILE}"
        COMMENT     "Analyzing ${INPUT_PATH}"
        VERBATIM
    )
     list(APPEND ALL_GPP_FILES "${OUT_GPP}")
    list(APPEND ALL_GH_FILES "${OUT_GH}")

    set(ALL_GPP_FILES ${ALL_GPP_FILES} PARENT_SCOPE)
    set(ALL_GH_FILES ${ALL_GH_FILES} PARENT_SCOPE)
endfunction()

# --- 선별된 헤더에 대해 코드 생성 실행 ---
foreach(IN_FILE IN LISTS REFLECT_TARGETS)
    add_analysis_step("${IN_FILE}")
endforeach()


#add_custom_target(codegen DEPENDS ${ALL_GPP_FILES} ${ALL_GH_FILES})
add_dependencies(EditorDirector codegen)
# 3) 생성된 소스/헤더를 Core 빌드에 포함
target_sources(EditorDirector PRIVATE ${ALL_GPP_FILES} ${ALL_GH_FILES})

# 4) (이미 있으나 재확인) 포함 경로에 GEN_DIR 추가
target_include_directories(EditorDirector PUBLIC ${GEN_DIR})