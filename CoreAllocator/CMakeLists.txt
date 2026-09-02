add_library(CoreAllocator SHARED 

    IAllocator.h
    IAllocator.cpp

 "CoreAllocatorDllMacro.h" "FMemory.h" "FMemory.cpp" "DefaultAllocator.h" "DefaultAllocator.cpp" "AllocatorManager.h" "AllocatorManager.cpp")


target_compile_definitions(CoreAllocator PRIVATE CORE_ALLOC_LIB)

target_link_libraries(CoreAllocator PUBLIC compiler_flags headerDir)





