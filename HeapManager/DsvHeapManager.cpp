#include "HeapManager/DsvHeapManager.h"
namespace Quad
{

	ViewIndex DsvHeapManager::CreateDsv(Microsoft::WRL::ComPtr<ID3D12Resource> resource,
		D3D12_DEPTH_STENCIL_VIEW_DESC* dsvDesc)
	{
		ViewIndex currentViewIndex = GetAvailableViewFirstIndex();
		if (currentViewIndex == -1)
		{
			throw "descriptor heap이 가득참";
			//return -1;
		}

		Microsoft::WRL::ComPtr<ID3D12Device> device = GetDevice();

		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = UseAvailableFirstView();
		device->CreateDepthStencilView(resource.Get(), dsvDesc, cpuHandle);

		return currentViewIndex;

	}

	void DsvHeapManager::InnerInit()
	{
		if (GetHeapType() != D3D12_DESCRIPTOR_HEAP_TYPE_DSV)
			throw "디스크립터 힙타입이 다름";

	}


}