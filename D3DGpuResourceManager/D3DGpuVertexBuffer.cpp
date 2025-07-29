#include "D3DGpuVertexBuffer.h"

D3DGRM::D3DGpuVertexBuffer::D3DGpuVertexBuffer(Microsoft::WRL::ComPtr<ID3D12Resource> buffer, size_t vertexNum, size_t vertexSize)
	:D3DGpuBuffer(buffer), mVertexNum(vertexNum),mVertexSize(vertexSize)
{
}



D3D12_VERTEX_BUFFER_VIEW D3DGRM::D3DGpuVertexBuffer::GetVertexBufferView() const
{

	D3D12_VERTEX_BUFFER_VIEW view;
	view.BufferLocation = GetResource()->GetGPUVirtualAddress();
	view.SizeInBytes = (UINT)(mVertexNum * mVertexSize);
	view.StrideInBytes = (UINT)mVertexSize;
	
	return view;
}