add_library(ReflectSystem SHARED

ReflectionSystem.cpp
"ReflectionClassInfo.cpp"

ReflectionClassInfo.h 
ReflectionPropertyInfo.h 
ReflectionSystem.h 
ReflectSystemDllMacro.h
ReflectionMacro.h  )


target_include_directories(ReflectSystem PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_definitions(ReflectSystem PRIVATE REFLECT_SYSTEM_LIB)

target_link_libraries(ReflectSystem PUBLIC compiler_flags headerDir CoreBase Utility )