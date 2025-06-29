#pragma once

#include"D3DGpuBuffer.h"

namespace D3DGRM
{

	class D3DGpuVertexBuffer:public D3DGpuBuffer
	{
	public:
		D3DGpuVertexBuffer(Microsoft::WRL::ComPtr<ID3D12Resource> buffer,size_t vertexNum, size_t vertexSize);
		//vertexbuffer함수

		D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() const;

	private:

		size_t mVertexNum;
		size_t mVertexSize;


	};
}

