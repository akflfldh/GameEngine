#include "HeapManager/CbvSrvUavHeapManager.h"


namespace Quad
{

	ViewIndex CbvSrvUavHeapManager::CreateCbv(D3D12_CONSTANT_BUFFER_VIEW_DESC* cbvDesc)
	{

		ViewIndex currentViewIndex = GetAvailableViewFirstIndex();
		if (currentViewIndex == -1)
		{
			throw "descriptor heap이 가득참";
			//return -1;
		}

		Microsoft::WRL::ComPtr<ID3D12Device> device = GetDevice();
		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = UseAvailableFirstView();


		device->CreateConstantBufferView(cbvDesc, cpuHandle);

		return currentViewIndex;
	}

	ViewIndex CbvSrvUavHeapManager::CreateSrv(Microsoft::WRL::ComPtr<ID3D12Resource> resource,
		D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc)
	{

		ViewIndex currentViewIndex = GetAvailableViewFirstIndex();
		if (currentViewIndex == -1)
		{
			throw "descriptor heap이 가득참";
			//return -1;
		}
		Microsoft::WRL::ComPtr<ID3D12Device> device = GetDevice();
		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = UseAvailableFirstView();
		device->CreateShaderResourceView(resource.Get(), srvDesc, cpuHandle);


		return currentViewIndex;
	}

	//ViewIndex CbvSrvUavHeapManager::CreateUav(D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc)
	//{
	//
	//	ViewIndex currentViewIndex = GetAvailableViewFirstIndex();
	//	if (currentViewIndex == -1)
	//	{
	//		throw "descriptor heap이 가득참";
	//	}
	//
	//	Microsoft::WRL::ComPtr<ID3D12Device> device = GetDevice();
	//	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = UseAvailableFirstView();
	//	device->CreateUnorderedAccessView(resource.Get(), &srvDesc, cpuHandle);
	//
	//
	//}


	void CbvSrvUavHeapManager::InnerInit()
	{
		if (GetHeapType() != D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
			throw "디스크립터 힙타입이 다름";

		return;
	}

}