add_library(PhysicalFileSystem SHARED
PhysicalFileSystem.cpp
PhysicalFileSystem.h
PhysicalFileSystemDLLMacro.h
WindowHandleRef.cpp
WindowHandleRef.h
"StandardPhysicalFileSystem.cpp"
"StandardPhysicalFileSystem.h"
)


target_compile_definitions(PhysicalFileSystem PRIVATE PHYSICAL_FILESYSTEM_EXPORT)

if(MSVC)
    add_compile_definitions(_WINDOWS)
endif()

target_include_directories(PhysicalFileSystem PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})



target_link_libraries(PhysicalFileSystem PUBLIC compiler_flags headerDir Utility Logger)




