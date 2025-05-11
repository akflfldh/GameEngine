#include "HeapManager/SamplerHeapManager.h"

namespace Quad
{

	ViewIndex SamplerHeapManager::CreateSPV(D3D12_SAMPLER_DESC* samplerDesc)
	{
		ViewIndex currentViewIndex = GetAvailableViewFirstIndex();
		if (currentViewIndex == -1)
		{
			throw "descriptor heap이 가득참";
		}

		Microsoft::WRL::ComPtr<ID3D12Device> device = GetDevice();
		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = UseAvailableFirstView();
		device->CreateSampler(samplerDesc, cpuHandle);


		return currentViewIndex;

	}

	void SamplerHeapManager::InnerInit()
	{

		if (GetHeapType() != D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER)
			throw "디스크립터 힙타입이 다름";

	}

}