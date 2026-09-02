#pragma once

#include <CoreAllocator/CoreAllocatorDllMacro.h>
#include <memory>

namespace CoreAlloc
{

class DefaultAllocator;

class CORE_ALLOC_API AllocatorManager
{
  public:
    static AllocatorManager *GetInstance();
    AllocatorManager();
    ~AllocatorManager();

    void *DefaultAllocate(size_t size, size_t alignment);
    void DefaultRelease(void *ptr);

  private:
    std::unique_ptr<DefaultAllocator> mDefaultAllocator;
};

} // namespace CoreAlloc