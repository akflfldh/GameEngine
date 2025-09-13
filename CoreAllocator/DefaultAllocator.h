#pragma once

#include <CoreAllocator/CoreAllocatorDllMacro.h>
#include <CoreAllocator/IAllocator.h>
namespace CoreAlloc
{

class CORE_ALLOC_API DefaultAllocator : public IAllocator
{
  public:
    DefaultAllocator();
    virtual ~DefaultAllocator();

    virtual void *Allocate(size_t size, size_t alignment) override;
    virtual void Release(void *ptr) override;

  private:
};

} // namespace CoreAlloc