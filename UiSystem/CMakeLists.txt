add_library(UiSystem SHARED
IUIComponent.h 
UIRenderableComponent.h 

UICanvas.h 
UIComponentFactory.h 
UIElement.h 
UIImage.h 
UIManager.h 
UIRectTransform.h 
UISystemDllMacro.h 
UIType.h 

IUIComponent.cpp
UICanvas.cpp 
UIComponentFactory.cpp
UIElement.cpp
UIImage.cpp
UIManager.cpp
UIRectTransform.cpp
UIRenderableComponent.cpp   
  
"UIResizeGizmoRenderableComponent.h" "UIResizeGizmoRenderableComponent.cpp" "UIImageComponent.h" "UIImageComponent.cpp" "UIResizeComponent.h" "UIResizeComponent.cpp" "UIMovableComponent.h" "UIMovableComponent.cpp" "UIButtonComponent.h" "UIButtonComponent.cpp" "UIButton.h" "UIButton.cpp" "UIElementPtr.h" "UIElementPtr.cpp" "UIComponentPtr.h" "UIComponentPtr.cpp" "UIElementTyprDef.h" "IViewContextInterface.h" "IUIRenderProxyManager.h" "IUIRenderProxyManager.cpp" "UITextComponent.h" "UITextComponent.cpp" "UILayoutComponent.h" "UILayoutComponent.cpp" "UIVerticalLayoutComponent.h" "UIVerticalLayoutComponent.cpp" "UIText.h" "UIText.cpp" "UIDoubleClickComponent.h" "UIDoubleClickComponent.cpp" "UITextInputComponent.h" "UITextInputComponent.cpp" "UIEditBox.h" "UIEditBox.cpp" "UIMultiRectComponent.h" "UIMultiRectComponent.cpp" "UITextButton.h" "UITextButton.cpp" "UIDragAndDropComponent.h" "UIDragAndDropComponent.cpp" "UIMouseWheelComponent.h" "UIMouseWheelComponent.cpp" "UIUtility.h" "UIUtility.cpp" "UIMouseDragComponent.h" "UIMouseDragComponent.cpp" "UIPopupManager.h" "UIPopupManager.cpp")
 
 

target_compile_definitions(UiSystem PRIVATE UISYSTEM_LIB)


target_include_directories(UiSystem PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


target_link_libraries(UiSystem PUBLIC  compiler_flags headerDir CoreAsset CoreMath Logger ReflectSystem CoreDevice CoreBase InputSystem CoreMath)


# (B) 외부 exe 또는 PATH에 있음:
set(CODEGEN_CMD "${CMAKE_CURRENT_SOURCE_DIR}/../CodeGenerator.exe")   # 경로 있으면 풀경로로 바꿔도 됨






# 2) 생성물 폴더 및 포함 (그대로 OK)
set(GEN_DIR ${CMAKE_CURRENT_BINARY_DIR}/Generated)
file(MAKE_DIRECTORY ${GEN_DIR})
target_include_directories(UiSystem PUBLIC ${GEN_DIR})


# --- 리플렉션 대상 헤더 검색 ---
file(GLOB_RECURSE ALL_HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
string(REPLACE "\\" "/" SRC_DIR_REGEX "${CMAKE_SOURCE_DIR}")
list(FILTER ALL_HEADERS EXCLUDE REGEX "${SRC_DIR_REGEX}/out/.*")
list(FILTER ALL_HEADERS EXCLUDE REGEX "${SRC_DIR_REGEX}/Generated/.*")
list(REMOVE_DUPLICATES ALL_HEADERS)

# --- REFLECT_CLASS 매크로가 있는 헤더만 선별 ---
set(REFLECT_TARGETS "")
foreach(HEADER_FILE IN LISTS ALL_HEADERS)
    file(READ "${HEADER_FILE}" FILE_CONTENT)
    string(REGEX MATCH "REFLECT_CLASS|REFLECT_STRUCT" IS_TARGET "${FILE_CONTENT}")
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
get_filename_component(OUT_GH_DIR "${OUT_GH}" DIRECTORY)


file(MAKE_DIRECTORY "${OUT_GH_DIR}")

    add_custom_command(
        OUTPUT      "${OUT_GPP}" "${OUT_GH}"
        COMMAND     ${CMAKE_COMMAND} -E touch "${OUT_GH}"

        
        COMMAND     ${CODEGEN_CMD} scan
                    --outputGpp   "${OUT_GPP}"
                    --outputGh    "${OUT_GH}"
                    --depfile     "${DEP_FILE}"
                    --deptarget   "${OUT_GPP}"
                    "${INPUT_PATH}"
                    -- -x c++ -std=c++20 --target=x86_64-pc-windows-msvc -D_DEBUG -I${CMAKE_SOURCE_DIR} -I${GEN_DIR} -I${CMAKE_CURRENT_SOURCE_DIR}
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

# --- 생성된 파일들을 빌드에 포함 ---
target_sources(UiSystem PRIVATE 
${ALL_GPP_FILES}
${ALL_GH_FILES})



