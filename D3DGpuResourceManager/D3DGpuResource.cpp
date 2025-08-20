#include "D3DGpuResourceManager/D3DGpuResource.h"

D3DGRM::D3DGpuResource::~D3DGpuResource() {}

D3DGRM::D3DGpuResource::D3DGpuResource(Microsoft::WRL::ComPtr<ID3D12Resource> resource) : mResource(resource) {}

Microsoft::WRL::ComPtr<ID3D12Resource> D3DGRM::D3DGpuResource::GetResource() const
{
    return mResource;
}

void D3DGRM::D3DGpuResource::SetDescriptorHandle(D3DGRM::ED3DResourceDescriptorType type,
                                                 const D3DDescriptorHandle descriptorHandle)
{
    mDescriptorHandleTable[type] = descriptorHandle;
}

bool D3DGRM::D3DGpuResource::GetDescriptorHandle(D3DGRM::ED3DResourceDescriptorType type,
                                                 D3DDescriptorHandle &descriptorHandle)
{

    auto it = mDescriptorHandleTable.find(type);

    if (it != mDescriptorHandleTable.end())
    {
        descriptorHandle = it->second;
        return true;
    }
    else
        return false;
}

void D3DGRM::D3DGpuResource::SetResourceState(D3D12_RESOURCE_STATES state)
{

    mResourceState = state;
}

D3D12_RESOURCE_STATES D3DGRM::D3DGpuResource::GetResourceState() const
{
    return mResourceState;
}

void D3DGRM::D3DGpuResource::RemoveDescriptorHandle(D3DGRM::ED3DResourceDescriptorType type)
{

    mDescriptorHandleTable.erase(type);
}

void D3DGRM::D3DGpuResource::RemoveDescriptorHandleAll()
{
    mDescriptorHandleTable.clear();
}
