#include "D3DGpuResource.h"

D3DGRM::D3DGpuResource::~D3DGpuResource()
{
}

D3DGRM::D3DGpuResource::D3DGpuResource(Microsoft::WRL::ComPtr<ID3D12Resource> resource)
	:mResource(resource)
{

}


Microsoft::WRL::ComPtr<ID3D12Resource> D3DGRM::D3DGpuResource::GetResource() const
{
	return mResource;

}

void D3DGRM::D3DGpuResource::SetDescriptorHandle(D3DDescriptorHandle handle)
{
	mDescriptorHandle = handle;
}
