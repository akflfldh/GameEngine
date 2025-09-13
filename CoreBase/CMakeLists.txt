add_library(CoreBase SHARED
CoreBaseDllMacro.h
FName.cpp
FName.h

 "FNameTable.h" "FNameTable.cpp" "FNameTableImpl.h" "FNameTableImpl.cpp" "FString.h" "FString.cpp" "Arch.h" "Arch.cpp" "BinaryArch.h" "BinaryArch.cpp" "FBuffer.h" "FBuffer.cpp" "TextArch.h" "TextArch.cpp" "FVector.h" "FVector.cpp")




target_compile_definitions(CoreBase PRIVATE COREBASE_EXPORT_LIB)

target_include_directories(CoreBase PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


target_link_libraries(CoreBase PUBLIC compiler_flags headerDir CoreAllocator BinaryReaderWriter)

