cmake_minimum_required(VERSION 3.25)

project(GameEngine VERSION 1.0 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(compiler_flags INTERFACE)

if(MSVC)
	add_compile_options(/utf-8)
	add_compile_definitions(_WINDOWS D3DX)
endif()



target_compile_features(compiler_flags INTERFACE cxx_std_20)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/$<CONFIG>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/$<CONFIG>)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/$<CONFIG>)



set(headerRootDirDef ${CMAKE_SOURCE_DIR})
add_library(headerDir INTERFACE)
target_include_directories(headerDir INTERFACE  ${headerRootDirDef})







add_subdirectory(Utility)
add_subdirectory(Window)
add_subdirectory(Logger)
add_subdirectory(PhysicalFileSystem)
add_subdirectory(BinaryReaderWriter)
add_subdirectory(CoreDevice)
add_subdirectory(D3DGpuResourceManager)
add_subdirectory(RenderSystem)
add_subdirectory(CoreAsset)
add_subdirectory(LogicalFileSystem)
add_subdirectory(CoreMath)
add_subdirectory(JsonParserWrapping)
add_subdirectory(InputSystem)
add_subdirectory(SystemInitializer)
add_subdirectory(ReflectSystem)
add_subdirectory(UiSystem)
add_subdirectory(ImportModule)
add_subdirectory(Core)
add_subdirectory(EditorDirector)