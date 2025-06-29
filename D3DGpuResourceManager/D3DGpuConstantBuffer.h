#pragma once


#include"D3DGpuBuffer.h"
#include<vector>
namespace D3DGRM
{
	//상수버퍼는 하나의 거대한 버퍼를  할당해서 연속된 작은 상수버퍼로 사용한다.

	class D3DGpuConstantBuffer:public D3DGpuBuffer
	{
	public:
		D3DGpuConstantBuffer(Microsoft::WRL::ComPtr<ID3D12Resource> buffer,size_t constantBufferNum, size_t constantBufferSize);
		~D3DGpuConstantBuffer() = default;
		

		void SetDescriptorHandleVector(const std::vector<D3DDescriptorHandle>& handleVector);
	
		size_t GetConstantBufferNum() const;

	private:
		std::vector<D3DDescriptorHandle> mDescriptorHandleVector;	//상수버퍼의 개수만큼존재한다.

		size_t mConstantBufferNum;
		size_t mConstantBufferSize;






	};

}
