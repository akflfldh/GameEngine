add_library(ImportModule STATIC

TextureImporter.h
 "FBXImporter.h" "FBXImporter.cpp" )

if(MSVC)
    target_sources(ImportModule PRIVATE 
    D3DTextureImporter.cpp 
    D3DHelper.cpp
    )
endif()

get_filename_component(PARENT_DIR "${CMAKE_CURRENT_SOURCE_DIR}" DIRECTORY)


set(FBXSDK_ROOT "" CACHE PATH "Path to FBXSDK Root")

set(FBXSDK_INCLUDE_DIR "${FBXSDK_ROOT}/include")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(FBXSDK_CONFIG_DIR "debug")
else()
    set(FBXSDK_CONFIG_DIR "release")
endif()    





set(FBXSDK_LIB_DIR "${FBXSDK_ROOT}/lib/x64/${FBXSDK_CONFIG_DIR}")



target_compile_definitions(ImportModule PRIVATE FBXSDK_SHARED)
target_include_directories(ImportModule PRIVATE ${FBXSDK_INCLUDE_DIR})
target_link_directories(ImportModule PRIVATE ${FBXSDK_LIB_DIR})
target_link_libraries(ImportModule PRIVATE ${FBXSDK_LIB_DIR}/libfbxsdk.lib wininet.lib)







add_library(DirectXTex STATIC IMPORTED)
set_target_properties(DirectXTex PROPERTIES 
    IMPORTED_LOCATION ${PARENT_DIR}/Lib/DirectXTex.lib
    INTERFACE_INCLUDE_DIRECTORIES ${PARENT_DIR}/Include/DirectTexX)


target_link_libraries(ImportModule PUBLIC compiler_flags headerDir DirectXTex Utility CoreAsset CoreBase )




target_include_directories(ImportModule PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${PARENT_DIR}/Include/DirectXTex  )



add_custom_command(TARGET 
ImportModule POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different 
"${FBXSDK_LIB_DIR}/libfbxsdk.dll"
"$<TARGET_FILE_DIR:EditorDirector>"
)