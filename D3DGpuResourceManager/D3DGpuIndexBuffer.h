#pragma once

#include"D3DGpuBuffer.h"
namespace D3DGRM
{

	class D3DGpuIndexBuffer:public D3DGpuBuffer
	{
	public:
		D3DGpuIndexBuffer(Microsoft::WRL::ComPtr<ID3D12Resource> bufffer,size_t indexNum, size_t indexSize);

		D3D12_INDEX_BUFFER_VIEW GetIndexBufferView() const;


	private:

		size_t mIndexNum;
		size_t mIndexSize;


	};

}
