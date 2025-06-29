#include "D3DGpuIndexBuffer.h"

D3DGRM::D3DGpuIndexBuffer::D3DGpuIndexBuffer(Microsoft::WRL::ComPtr<ID3D12Resource> buffer,size_t indexNum,size_t indexSize)
	:D3DGpuBuffer(buffer),mIndexNum(indexNum),mIndexSize(indexSize)
{
}



D3D12_INDEX_BUFFER_VIEW D3DGRM::D3DGpuIndexBuffer::GetIndexBufferView() const
{

	D3D12_INDEX_BUFFER_VIEW view;

	view.BufferLocation = GetResource()->GetGPUVirtualAddress();
	view.Format = DXGI_FORMAT_R32_UINT;
	view.SizeInBytes = mIndexSize*mIndexNum;

	return view;
}
