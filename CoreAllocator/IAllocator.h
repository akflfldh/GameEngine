

#include <CoreAllocator/CoreAllocatorDllMacro.h>

namespace CoreAlloc
{
class CORE_ALLOC_API IAllocator
{
  public:
    IAllocator();
    virtual ~IAllocator() = 0;

    virtual void *Allocate(size_t size, size_t alignment) = 0;
    virtual void Release(void *ptr) = 0;

  private:
};

} // namespace CoreAlloc