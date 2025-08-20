#include "D3DGpuResourceManager/D3DGpuBuffer.h"

D3DGRM::D3DGpuBuffer::D3DGpuBuffer(Microsoft::WRL::ComPtr<ID3D12Resource> resource) : D3DGpuResource(resource)
{
    mGpuResourceType = GRM::EGpuResourceType::eBuffer;
}

D3DGRM::D3DGpuBuffer::~D3DGpuBuffer() {}
