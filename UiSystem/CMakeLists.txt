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

 "UIResizeGizmoRenderableComponent.h" "UIResizeGizmoRenderableComponent.cpp" "UIImageComponent.h" "UIImageComponent.cpp" "UIResizeComponent.h" "UIResizeComponent.cpp" "UIMovableComponent.h" "UIMovableComponent.cpp" "UIButtonComponent.h" "UIButtonComponent.cpp" "UIButton.h" "UIButton.cpp")



target_compile_definitions(UiSystem PRIVATE UISYSTEM_LIB)


target_include_directories(UiSystem PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


target_link_libraries(UiSystem PUBLIC  compiler_flags headerDir CoreAsset CoreMath Logger ReflectSystem CoreDevice)





# (B) 외부 exe 또는 PATH에 있음:
set(CODEGEN_CMD "${CMAKE_CURRENT_SOURCE_DIR}/../CodeGenerator.exe")   # 경로 있으면 풀경로로 바꿔도 됨




# 2) 생성물 폴더 및 포함 (그대로 OK)
set(GEN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Generated)
file(MAKE_DIRECTORY ${GEN_DIR})
target_include_directories(UiSystem PRIVATE ${GEN_DIR})


# 리플렉션 대상 헤더들 찾기
file(GLOB_RECURSE REFLECT_INPUTS CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

# 빌드 폴더 안에 있는 파일은 제외
string(REPLACE "\\" "/" SRC_DIR_REGEX "${CMAKE_CURRENT_SOURCE_DIR}")
list(FILTER REFLECT_INPUTS EXCLUDE REGEX
    "${SRC_DIR_REGEX}/Generated/.*\\.generated\\.(h|cpp)$"
)
list(REMOVE_DUPLICATES REFLECT_INPUTS)

# 중복 제거
list(REMOVE_DUPLICATES REFLECT_INPUTS)
list(FILTER REFLECT_INPUTS EXCLUDE REGEX ".*/pch\\.h$")
list(FILTER REFLECT_INPUTS EXCLUDE REGEX ".*/framework\\.h$")

# 디버그용 출력
message(STATUS "Reflection inputs for UiSystem: ${REFLECT_INPUTS}")








# 4) 입력 1개당 규칙 생성하는 함수
function(add_reflect_for_input TARGET_NAME INPUT_PATH)
  # 소스 트리 기준 상대 경로와 디렉터리/베이스 분리
  file(RELATIVE_PATH REL "${CMAKE_CURRENT_SOURCE_DIR}" "${INPUT_PATH}")       # 예: Core/UI/UIType.h
  get_filename_component(BASE "${INPUT_PATH}" NAME_WE)                        # 예: UIType
  get_filename_component(REL_DIR "${REL}" DIRECTORY)                          # 예: Core/UI

  # 생성물은 GEN_DIR 하위에 원본 디렉터리 구조를 그대로 만듦(이름 충돌 방지)
  #set(OUT_DIR  "${GEN_DIR}/${REL_DIR}")                                       # 예: build/.../Generated/Core/UI
  set(OUT_DIR  "${CMAKE_CURRENT_SOURCE_DIR}/Generated")           
  file(MAKE_DIRECTORY "${OUT_DIR}")

  # 생성 파일 이름은 생성기의 규칙(베이스.generated.*)에 맞춤
  set(OUT_H    "${OUT_DIR}/${BASE}.generated.h")                               # UIType.generated.h
  set(OUT_CPP  "${OUT_DIR}/${BASE}.generated.cpp")                             # UIType.generated.cpp
  set(DEP      "${OUT_DIR}/${BASE}.d")


 # 절대 경로로 명시적 설정
  set(INCLUDE_FLAGS 
    -I${headerRootDirDef}
  )









  add_custom_command(
    # VS가 먼저 만들어야 하는 '바로 그 파일'을 OUTPUT으로 지정
    OUTPUT  "${OUT_CPP}"
    
    # 1) deps만 먼저 갱신 (항상 새로 씀)
    COMMAND ${CODEGEN_CMD}
             -p "${CMAKE_BINARY_DIR}" 
            --deps-only
            --outdir    "${OUT_DIR}"
            --depfile   "${DEP}"
            --deptarget "${OUT_CPP}"             # depfile 타겟 = OUTPUT과 '바이트 단위 동일'
            --basename   "${BASE}" 
            "${INPUT_PATH}"
              -- -x c++ -std=c++20 ${INCLUDE_FLAGS}

    # 2) 실제 생성 (생성기가 아무것도 안 만들 수도 있으므로, 마지막에 최소 보장)
    COMMAND ${CODEGEN_CMD}
            -p "${CMAKE_BINARY_DIR}" 
            --outdir "${OUT_DIR}"
            --basename   "${BASE}" 
            "${INPUT_PATH}"
            -- -x c++ -std=c++20 ${INCLUDE_FLAGS}

            
 # 3) cpp 없으면 빈 파일로 보장
    COMMAND ${CMAKE_COMMAND} -E touch "${OUT_CPP}"

    # 빌드 도구에게 관계 알려주기
    DEPFILE    "${DEP}"
    BYPRODUCTS "${OUT_H}"                 # OUT_CPP는 OUTPUT이니 BYPRODUCTS에서 빼기
    DEPENDS    "${INPUT_PATH}" 

    VERBATIM
    COMMAND_EXPAND_LISTS
  )

  # 생성물을 타겟에 포함
  target_sources(${TARGET_NAME} PRIVATE "${OUT_CPP}" "${OUT_H}")
endfunction()

# 5) 모든 입력 파일에 규칙 적용
foreach(IN_FILE IN LISTS REFLECT_INPUTS)
  add_reflect_for_input(UiSystem "${IN_FILE}")
endforeach()