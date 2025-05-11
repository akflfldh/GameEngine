#pragma once
#include"header.h"
#include"Predeclare.h"
#include"Utility/Utility.h"


#include"Buffer/UploadBufferDllExport.h"

namespace Quad
{
	class UPLOADBUFFER_API_LIB	UploadBuffer
	{
		friend class ResourceController;
		friend class ShaderResourceFactory;
		friend class StructuredBufferFactory;
	public:
		UploadBuffer(Microsoft::WRL::ComPtr<ID3D12Device>device, UINT elementSize, UINT elementNum, bool isConstants);
		~UploadBuffer();

		void CopyData(UINT index, const void* srcData, UINT elementSize) const;

		D3D12_GPU_VIRTUAL_ADDRESS GetGpuVirtualAddress() const;
		UINT64 GetElementBufferSize() const;
		Microsoft::WRL::ComPtr<ID3D12Resource> GetResource() const;

	private:


		Microsoft::WRL::ComPtr<ID3D12Resource> mUploadBuffer;
		BYTE* mPmapped;
		UINT64 mElementSize;
		UINT64 mElementBufferSize;



	};

}