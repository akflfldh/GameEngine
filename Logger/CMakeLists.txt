add_library(Logger SHARED
Logger.h
Logger.cpp
LoggerImpl.cpp
LoggerImpl.h
)
# 이 타깃을 컴파일할 때만 export 모드가 되도록
target_compile_definitions(Logger PRIVATE QUAD_LOG_EXPORT)


target_include_directories(Logger PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Include"  "${CMAKE_CURRENT_SOURCE_DIR}")

get_filename_component(PARENT_DIR "${CMAKE_CURRENT_SOURCE_DIR}" DIRECTORY)

add_library(CallbackLib SHARED IMPORTED)


set_target_properties(CallbackLib PROPERTIES
IMPORTED_IMPLIB_DEBUG "${PARENT_DIR}/QuadCallbackSystemDLL/QuadCallbackSystem.lib"
IMPORTED_IMPLIB_RELEASE "${PARENT_DIR}/QuadCallbackSystemDLL/QuadCallbackSystem.lib"
IMPORTED_LOCATION "${PARENT_DIR}/QuadCallbackSystemDLL/QuadCallbackSystem.dll")


target_link_libraries(Logger PUBLIC compiler_flags headerDir PRIVATE CallbackLib)


# DLL을 bin 폴더로 자동 복사
add_custom_command(TARGET Logger POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${PARENT_DIR}/QuadCallbackSystemDLL/QuadCallbackSystem.dll"
        "${CMAKE_BINARY_DIR}/bin/$<CONFIG>/"
    COMMENT "Copying CallbackLib DLL to output directory"
)