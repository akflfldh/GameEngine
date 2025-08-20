add_library(ReflectSystem SHARED

ReflectionSystem.cpp
ReflectionSystemImpl.cpp
ReflectionType.cpp

ReflectionClassInfo.h 
ReflectionPropertyInfo.h 
ReflectionSystem.h 
ReflectionSystemImpl.h 
ReflectSystemDllMacro.h
ReflectionMacro.h

 "ClassInfoImpl.h" "ClassInfoImpl.cpp")


target_include_directories(ReflectSystem PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_definitions(ReflectSystem PRIVATE REFLECT_SYSTEM_LIB)

target_link_libraries(ReflectSystem PUBLIC compiler_flags headerDir)