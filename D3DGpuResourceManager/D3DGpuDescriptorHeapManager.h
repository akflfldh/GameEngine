#pragma once


#include<d3d12.h>
#include<wrl.h>
#include<vector>

#include"D3DGpuType.h"
#include"GpuResourceDllMacro.h"
namespace D3DGRM
{
	




	class GPURESOURCE_MANAGER_API D3DGpuDescriptorHeapManager
	{
	public:
		D3DGpuDescriptorHeapManager(Microsoft::WRL::ComPtr<ID3D12Device> device, D3D12_DESCRIPTOR_HEAP_TYPE type, size_t descriptorNum,
			D3D12_DESCRIPTOR_HEAP_FLAGS  heapFlag = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
		~D3DGpuDescriptorHeapManager();
	
	
		D3DDescriptorHandle Allocate();
		void Free(D3DDescriptorHandle handle);

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetDescriptorHeap() const;
			 
	private:
		Microsoft::WRL::ComPtr<ID3D12Device> mDevice;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDescriptorHeap;

		D3D12_DESCRIPTOR_HEAP_FLAGS  mHeapFlag;


		size_t mDescriptorIncrementSize;
		size_t  mDescriptorNextOffset;	//free offset vector가 비어있을경우 사용(할때마다 ++1증가)
		std::vector<size_t> mFreeDecriptorOffsetVector;		//마지막에있는 offset을 꺼내쓰고, 반환된 오프셋은 다시 마지막에 푸시
	};

}
