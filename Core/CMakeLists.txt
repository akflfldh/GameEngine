add_library(Core SHARED
Application.cpp 
 
GlobalAppHelper.cpp 

 
IProgramDirector.cpp
ProjectConfig.cpp 

 

Application.h 
 
GlobalAppHelper.h 
 
   
IProgramDirector.h
MeshType.h  
ProjectConfig.h 
 
CoreDllExport.h 
"ViewportController.h" "ViewportController.cpp" "CoreType.h"  "LogicalWindow.h" "LogicalWindow.cpp"  "Transform.h" "Transform.cpp" "Component.h" "Component.cpp" "Object.h" "Object.cpp" "ObjectImpl.h" "ObjectImpl.cpp" "Entity.h" "Entity.cpp"  "SceneComponent.h" "SceneComponent.cpp"   "QuadVector.h" "QuadVector.cpp" "QuadVectorImpl.h" "QuadVectorImpl.cpp"  "Map.h" "Map.cpp"  "World.h" "World.cpp" "MapFactory.h" "MapFactory.cpp" "StaticMeshComponent.h" "StaticMeshComponent.cpp" "MapLoader.h" "MapLoader.cpp" "MapMetaDataHeader.h"
"MapMetaDataHeader.cpp" "MapStorer.h" "MapStorer.cpp" "CameraComponent.h" "CameraComponent.cpp" "CameraObject.h" "CameraObject.cpp" "StaticMeshObject.h" "StaticMeshObject.cpp" "IRenderProxyManager.h" "IRenderProxyManager.cpp" "MultiCallbackSystem.h" "MultiCallbackSystem.cpp" "ObjectManager.h" "ObjectManager.cpp" "ObjectTypeDef.h" "ObjectPtr.h" "ObjectPtr.cpp" "CallbackSystemDef.h" "ObjectController.h" "ObjectController.cpp" "SuperController.h" "SuperController.cpp" "SceneManager.h" "SceneManager.cpp" "IEngineMode.h" "IEngineMode.cpp" "IHittable.h" "IDrawableOutline.h" "IDrawableOutline.cpp" "RenderIDManager.h" "RenderIDManager.cpp" "MaterialSystem.h" "MaterialSystem.cpp" "LightComponent.h" "LightComponent.cpp" "LightObject.h" "LightObject.cpp" "Prefab.h" "Prefab.cpp" "PrefabFactory.h" "PrefabFactory.cpp" "PrefabLoader.h" "PrefabLoader.cpp" "PrefabMetaDataHeader.cpp" "PrefabMetaDataHeader.h" "PrefabStorer.h" "PrefabStorer.cpp" "WorkSpace.h" "WorkSpace.cpp" "IInterface.h" "IInterface.cpp" "RuntimeServices.h" "GameMode.h" "GameMode.cpp" "PlayerController.h" "PlayerController.cpp" "PlayerStart.h" "PlayerStart.cpp" "BillboardComponent.h" "BillboardComponent.cpp" "DebugDraw.h" "DebugDraw.cpp" "ColliderComponent.h" "ColliderComponent.cpp" "PhysicsBridgeSystem.h" "PhysicsBridgeSystem.cpp" "IPhysicsBodyComponent.h" "IPhysicsBodyComponent.cpp" "RigidBodyComponent.h" "RigidBodyComponent.cpp" "IPhysicsShapeProvider.h" "IPhysicsShapeProvider.cpp" "CorePhysicsType.h" "ControllableEntity.h" "ControllableEntity.cpp" "Character.h" "Character.cpp" "CharacterMovementComponent.h" "CharacterMovementComponent.cpp")


target_compile_definitions(Core PRIVATE CORE_EXPORT_LIB)

target_include_directories(Core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


target_link_libraries(Core PUBLIC compiler_flags headerDir Utility 
CoreAsset Logger JsonParser BinaryReaderWriter
UiSystem SystemInitializer InputSystem CoreBase ReflectSystem Window CoreMath Physics)






# (B) 외부 exe 또는 PATH에 있음:
set(CODEGEN_CMD "${CMAKE_CURRENT_SOURCE_DIR}/../CodeGenerator.exe")   # 경로 있으면 풀경로로 바꿔도 됨


# 2) 생성물 폴더 및 포함 (그대로 OK)
set(GEN_DIR ${CMAKE_CURRENT_BINARY_DIR}/Generated)
file(MAKE_DIRECTORY ${GEN_DIR})
target_include_directories(Core PUBLIC ${GEN_DIR})




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


add_custom_target(codegen DEPENDS ${ALL_GPP_FILES} ${ALL_GH_FILES})
add_dependencies(Core codegen)
# 3) 생성된 소스/헤더를 Core 빌드에 포함
target_sources(Core PRIVATE ${ALL_GPP_FILES} ${ALL_GH_FILES})

# 4) (이미 있으나 재확인) 포함 경로에 GEN_DIR 추가
target_include_directories(Core PUBLIC ${GEN_DIR})