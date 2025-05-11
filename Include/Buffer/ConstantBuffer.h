#pragma once


#include"Predeclare.h"
#include"header.h"
#include"DescriptorHeapType.h"
#include"Buffer/UploadBufferDllExport.h"

namespace Quad
{

	class UPLOADBUFFER_API_LIB ConstantBuffer
	{
		friend class ShaderResourceFactory;
		friend class ResourceController;
		friend class ConstantBufferManager;
	public:
		
		~ConstantBuffer();

		ViewIndex GetViewIndex(int elementIndex) const;
		UINT GetElementSize() const;
		UINT GetElementMaxNum()const;

		void ResetUpLoadCount();
		ViewIndex UploadData(const void * data,UINT dataSize);
		UINT GetCurrentUploadCount();
	private:
		//상수버퍼를 만들수있는 팩토리가있어야할거같다 여러가지 설정과 인자들이 필요하니
		ConstantBuffer(Microsoft::WRL::ComPtr<ID3D12Device> device, UINT elementSize, UINT elementNum);

	private:
		//이 예약어가 어떤크기인지 어떤데이터인지 엔진은 다 알고있다.
		//그것을 처리할 팩토리나 ,시스템이 필요할거다.
		int mElementSize;		//4바이트단위로의 정렬을 고려한 사이즈 (정렬한)
		int mElementMaxNum;		//개수
		UploadBuffer* mUploadBuffer;		
		std::vector<ViewIndex> mViewIndexVector;
		UINT mUploadCount;


	};
}

