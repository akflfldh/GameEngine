#pragma once

#include"Predeclare.h"
#include"UploadBuffer.h"
//#include"Utility.h"
#include"DescriptorHeapType.h"

#include"Buffer/UploadBufferDllExport.h"

namespace Quad
{


	//srv dsv도 가능한데 일단 srv만 생각하자
	class UPLOADBUFFER_API_LIB StructuredBuffer
	{
		friend class ResourceController;
		friend class StructuredBufferFactory;
	public:
		~StructuredBuffer() = default;

		void CopyData(UINT index, const void* srcData);
		ViewIndex GetViewIndex() const;
		void UploadData(const void* data);

		UINT GetElementSize()const;

		void ResetUploadCount();
	private:
		StructuredBuffer(Microsoft::WRL::ComPtr<ID3D12Device> device,UINT elementSize , UINT elementMaxNum);
		void SetViewIndex(ViewIndex index);

	

	private:
		UINT mElementSize;
		UINT mElementMaxNum;
		UINT mAlignSize;
		UploadBuffer* mUploadBuffer;
		ViewIndex mViewIndex;
		size_t mUploadCount;
	};


}

