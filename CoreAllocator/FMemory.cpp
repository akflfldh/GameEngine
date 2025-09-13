#include "FMemory.h"
#include "CoreAllocator/DefaultAllocator.h"
CoreAlloc::FMemory *CoreAlloc::FMemory::GetInstance()
{
    static FMemory instance;
    return &instance;
}

CoreAlloc::FMemory::FMemory()
{
    mDefaultAllocator = new DefaultAllocator;
}

CoreAlloc::FMemory::~FMemory()
{
    delete mDefaultAllocator;
}

void *CoreAlloc::FMemory::Allocate(size_t size, size_t alignment)
{
    return mDefaultAllocator->Allocate(size, alignment);
}

void CoreAlloc::FMemory::Release(void *ptr)
{
    mDefaultAllocator->Release(ptr);
}