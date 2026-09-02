add_library(CoreBase SHARED
CoreBaseDllMacro.h
FName.cpp
FName.h

 "FNameTable.h" "FNameTable.cpp" "FNameTableImpl.h" "FNameTableImpl.cpp" "FString.h" "FString.cpp" "Arch.h" "Arch.cpp" "BinaryArch.h" "BinaryArch.cpp" "FBuffer.h" "FBuffer.cpp" "TextArch.h" "TextArch.cpp" "FVector.h" "FVector.cpp" "Blitable_Trait.h" "TypeDescriptor.h" "TypeDescriptor.cpp" "BaseClass.h" "BaseClass.cpp" "CallbackSystem.h" "CallbackSystem.cpp" "CoreBaseType.h" "CoreAssert.h" "CoreAssert.cpp" "CoreStringld.h" "MainTimer.h" "MainTimer.cpp" "TimerManager.h" "TimerManager.cpp" "AsyncThread.h" "AsyncThread.cpp" "AsyncThreadPool.h" "AsyncThreadPool.cpp" "PrograssContext.h" "PrograssContext.cpp" "ThreadSafeQueue.h" "ThreadSafeQueue.cpp" "CollisionPhysicsType.h")




target_compile_definitions(CoreBase PRIVATE COREBASE_EXPORT_LIB)

target_include_directories(CoreBase PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


target_link_libraries(CoreBase PUBLIC compiler_flags headerDir CoreAllocator BinaryReaderWriter)

