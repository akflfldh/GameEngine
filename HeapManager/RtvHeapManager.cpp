#include "HeapManager/RtvHeapManager.h"


namespace Quad
{
	ViewIndex RtvHeapManager::CreateRtv(Microsoft::WRL::ComPtr<ID3D12Resource> resource,
		D3D12_RENDER_TARGET_VIEW_DESC* rtvDesc)
	{
		ViewIndex currentViewIndex = GetAvailableViewFirstIndex();
		if (currentViewIndex == -1)
		{
			throw "descriptor heap이 가득참";
		}

		Microsoft::WRL::ComPtr<ID3D12Device> device = GetDevice();
		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = UseAvailableFirstView();
		device->CreateRenderTargetView(resource.Get(), rtvDesc, cpuHandle);


		return currentViewIndex;
	}

	void RtvHeapManager::InnerInit()
	{

		if (GetHeapType() != D3D12_DESCRIPTOR_HEAP_TYPE_RTV)
			throw "디스크립터 힙타입이 다름";
	}

}