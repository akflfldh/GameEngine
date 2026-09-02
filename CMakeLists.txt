cmake_minimum_required(VERSION 3.25)

project(GameEngine VERSION 1.0 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(compiler_flags INTERFACE)

if(MSVC)
	add_compile_options(/utf-8)
	add_compile_definitions(_WINDOWS D3DX)
	add_compile_options(/Zc:preprocessor)
 	set(CMAKE_MSVC_RUNTIME_LIBRARY
        "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
endif()

target_compile_features(compiler_flags INTERFACE cxx_std_20)


option(GAMEENGINE_DEVELOPMENT_BUILD "Use the source directory as EditorRoot" OFF)



set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/$<CONFIG>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/$<CONFIG>)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/$<CONFIG>)


add_subdirectory(glm)

set(headerRootDirDef ${CMAKE_SOURCE_DIR})
add_library(headerDir INTERFACE)
target_include_directories(headerDir INTERFACE  ${headerRootDirDef})
target_link_libraries(headerDir INTERFACE glm::glm)
target_compile_definitions(headerDir INTERFACE  
GLM_ENABLE_EXPERIMENTAL
GLM_FORCE_LEFT_HANDED
GLM_FORCE_DEPTH_ZERO_TO_ONE)






add_subdirectory(CoreAllocator)
add_subdirectory(Utility)
add_subdirectory(Window)
add_subdirectory(Logger)
add_subdirectory(PhysicalFileSystem)
add_subdirectory(BinaryReaderWriter)
add_subdirectory(CoreBase)
add_subdirectory(CoreDevice)
add_subdirectory(D3DGpuResourceManager)
add_subdirectory(RenderSystem)
add_subdirectory(CoreAsset)
add_subdirectory(LogicalFileSystem)
add_subdirectory(CoreMath)
add_subdirectory(Physics)
add_subdirectory(JsonParserWrapping)
add_subdirectory(InputSystem)
add_subdirectory(SystemInitializer)
add_subdirectory(ReflectSystem)
add_subdirectory(UiSystem)
add_subdirectory(ImportModule)
add_subdirectory(RenderFrontend)
add_subdirectory(Core)
add_subdirectory(EditorDirector)

option(GAMEENGINE_BUILD_PHYSICS_TESTS "Build the standalone Physics test executable" OFF)

if(GAMEENGINE_BUILD_PHYSICS_TESTS)
    add_subdirectory(PhysicsTests)
endif()
