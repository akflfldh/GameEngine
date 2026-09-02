#include "IGpuResource.h"
#include "D3DGpuResourceManager/IGpuResource.h"
#include "D3DGpuResourceManager/IGpuResourceManager.h"
#include "IGpuResource.h"
GRM::IGpuResource::IGpuResource() {}

void GRM::IGpuResource::SetCurrentResourceState(EResourceState state)
{
    if (state == EResourceState::ePresent)
    {
        int a = 2;
    }

    mCurrentResourceState = state;
}

GRM::IGpuResource::~IGpuResource() {}

GRM::EGpuResourceType GRM::IGpuResource::GetGpuResourceType() const
{
    return mGpuResourceType;
}

EResourceState GRM::IGpuResource::GetCurrentResourceState() const
{
    return mCurrentResourceState;
}

void GRM::IGpuResource::Release()
{
    auto manager = IGpuResourceManager::GetInstance();

    manager->Release(this);
}