#pragma once


#include"HeapManager/DescriptorHeapManager.h"

#include"HeapManager/HeapManagerDllExport.h"

namespace Quad
{
	class HEAPMANAGER_API_LIB CbvSrvUavHeapManager :public DescriptorHeapManager
	{
	public:
		CbvSrvUavHeapManager() = default;
		~CbvSrvUavHeapManager() = default;


		ViewIndex CreateCbv(D3D12_CONSTANT_BUFFER_VIEW_DESC* cbvDesc);	//ViewIndex == -1  실패
		ViewIndex CreateSrv(Microsoft::WRL::ComPtr<ID3D12Resource> resource, D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc);


		//ViewIndex CreateUav(D3D12_UNORDERED_ACCESS_VIEW_DESC  uavDesc);
		//bool CreateDescriptorUav();

		//Delete View 

	private:
		void InnerInit() override;


	private:


	};

}