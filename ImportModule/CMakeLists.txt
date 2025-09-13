add_library(ImportModule STATIC

TextureImporter.h
)

if(MSVC)
    target_sources(ImportModule PRIVATE 
    D3DTextureImporter.cpp 
    D3DHelper.cpp
    )
endif()

get_filename_component(PARENT_DIR "${CMAKE_CURRENT_SOURCE_DIR}" DIRECTORY)



add_library(DirectXTex STATIC IMPORTED)
set_target_properties(DirectXTex PROPERTIES 
    IMPORTED_LOCATION ${PARENT_DIR}/Lib/DirectXTex.lib
    INTERFACE_INCLUDE_DIRECTORIES ${PARENT_DIR}/Include/DirectTexX)


target_link_libraries(ImportModule PUBLIC compiler_flags headerDir DirectXTex Utility CoreAsset CoreBase)




target_include_directories(ImportModule PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${PARENT_DIR}/Include/DirectXTex )

