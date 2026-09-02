#include "AllocatorManager.h"
#include <CoreAllocator/DefaultAllocator.h>
CoreAlloc::AllocatorManager *CoreAlloc::AllocatorManager::GetInstance()
{
    static AllocatorManager instance;
    return &instance;
}

CoreAlloc::AllocatorManager::AllocatorManager() : mDefaultAllocator(std::make_unique<DefaultAllocator>()) {}

CoreAlloc::AllocatorManager::~AllocatorManager() {}

void *CoreAlloc::AllocatorManager::DefaultAllocate(size_t size, size_t alignment)
{
    return mDefaultAllocator->Allocate(size, alignment);
}

void CoreAlloc::AllocatorManager::DefaultRelease(void *ptr)
{

    mDefaultAllocator->Release(ptr);
}
