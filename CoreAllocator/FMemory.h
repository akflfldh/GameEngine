#pragma once

#include <CoreAllocator/CoreAllocatorDllMacro.h>

namespace CoreAlloc
{

// Allocator들의 사용에대한 입구를 하나로
//  또한 각 allcator  정보를 종합하여 메모리 사용량등을 파악하거나 할수있을거같다.
class IAllocator;
class CORE_ALLOC_API FMemory
{

  public:
    static FMemory *GetInstance();
    FMemory();
    ~FMemory();

    // Default Allocator 사용 메서드
    void *Allocate(size_t size, size_t alignment = 16);
    void Release(void *ptr);

    // 나머지 Allocator는 직접 받아서 사용
    // PoolAllocator GetPoolAllocator()

  private:
    IAllocator *mDefaultAllocator;
};

} // namespace CoreAlloc