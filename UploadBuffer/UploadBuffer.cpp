#include "Buffer/UploadBuffer.h"

namespace Quad
{
	UploadBuffer::UploadBuffer(Microsoft::WRL::ComPtr<ID3D12Device> device, UINT elementSize, UINT elementNum, bool isConstants)
	{

		//?ш린議곗젙
		//size 
		mElementBufferSize = mElementSize = elementSize;
		if (isConstants)
		{
			mElementBufferSize = (elementSize + 255) & (~255);
		}

		UINT64 size = mElementBufferSize * elementNum;

		D3D12_RESOURCE_DESC uploadBufferDesc;
		uploadBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		uploadBufferDesc.Alignment = 0;	//64kb;
		uploadBufferDesc.Width = size;
		uploadBufferDesc.Height = 1;
		uploadBufferDesc.DepthOrArraySize = 1;
		uploadBufferDesc.MipLevels = 1;
		uploadBufferDesc.SampleDesc.Count = 1;
		uploadBufferDesc.SampleDesc.Quality = 0;
		uploadBufferDesc.Format = DXGI_FORMAT_UNKNOWN;
		uploadBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		uploadBufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_UPLOAD);
		ThrowIfFailed(device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &uploadBufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr, IID_PPV_ARGS(&mUploadBuffer)));

		D3D12_RANGE read_range{ 0,0 };
		ThrowIfFailed(mUploadBuffer->Map(0, &read_range, (void**)&mPmapped));
	}

	UploadBuffer::~UploadBuffer()
	{
		mUploadBuffer->Unmap(0, nullptr);

	}

	void UploadBuffer::CopyData(UINT index, const void* srcData,UINT elementSize) const
	{
		memcpy(&mPmapped[index * mElementBufferSize], srcData, elementSize);
	}

	D3D12_GPU_VIRTUAL_ADDRESS UploadBuffer::GetGpuVirtualAddress() const
	{
		return mUploadBuffer->GetGPUVirtualAddress();
	}

	UINT64 UploadBuffer::GetElementBufferSize() const
	{
		return mElementBufferSize;
	}

	Microsoft::WRL::ComPtr<ID3D12Resource> UploadBuffer::GetResource() const
	{
		return mUploadBuffer;
	}

}