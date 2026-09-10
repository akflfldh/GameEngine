set(GAME_DIRECTOR_SOURCES
    GameDirector.cpp
      GameDirector.h
      GameWindowController.cpp
      GameWindowController.h
)


if(MSVC)
    add_executable(GameDirector WIN32 ${GAME_DIRECTOR_SOURCES} "GameRuntimeMode.h" "GameRuntimeMode.cpp" "GameRuntimeConfig.h" "GameRuntimeConfig.cpp" "GameMain.cpp")

target_include_directories(GameDirector  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})


target_compile_definitions(GameDirector PRIVATE
    GAME_DEBUG_PROJECT_PATH="${GAME_DEBUG_PROJECT_PATH}"
)





target_link_libraries(GameDirector PRIVATE 
compiler_flags
headerDir
Core
  CoreAsset
  CoreBase
  CoreMath
  Window
  InputSystem
  UiSystem
  RenderSystem
  RenderFrontend
  GpuResourceManager
  SystemInitializer
  LogicalFileSystem
  Logger
  Utility
)

set(GAME_RUNTIME_TEMPLATE_DIR
      "$<TARGET_FILE_DIR:GameDirector>/GameRuntimeTemplate"
  )


set(RUNTIME_DEPENDENCY_SCRIPT
    "${CMAKE_SOURCE_DIR}/CMake/CollectRuntimeDependencies.cmake")



add_custom_command(TARGET GameDirector POST_BUILD 
                    COMMAND ${CMAKE_COMMAND} -E make_directory "${GAME_RUNTIME_TEMPLATE_DIR}"
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:GameDirector>" 
                    "$<TARGET_RUNTIME_DLLS:GameDirector>"
                    "${GAME_RUNTIME_TEMPLATE_DIR}"

                    COMMAND ${CMAKE_COMMAND} 
                    "-DGAME_OUTPUT_DIR=${GAME_RUNTIME_TEMPLATE_DIR}"
                    "-DRUNTIME_SEARCH_DIR=$<TARGET_FILE_DIR:GameDirector>"
                    -P "${RUNTIME_DEPENDENCY_SCRIPT}"


                    COMMAND_EXPAND_LISTS
                    VERBATIM
)


endif()



