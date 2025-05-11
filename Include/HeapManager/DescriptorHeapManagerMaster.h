#pragma once


#include"HeapManager/DescriptorHeapManager.h"
#include"HeapManager/CbvSrvUavHeapManager.h"
#include"HeapManager/DsvHeapManager.h"
#include"HeapManager/RtvHeapManager.h"
#include"HeapManager/SamplerHeapManager.h"

#include"HeapManager/HeapManagerDllExport.h"
namespace Quad
{
	class HEAPMANAGER_API_LIB DescriptorHeapManagerMaster
	{
	public:
		DescriptorHeapManagerMaster();
		~DescriptorHeapManagerMaster() = default;

		void Initialize(CbvSrvUavHeapManager* cbvSrvUavDescriptorHeapManager,
			DsvHeapManager* dsvDescriptorHeapManager,
			RtvHeapManager* rtvDescriptorHeapManager,
			SamplerHeapManager* spvDescriptorHeapManager);

		ViewIndex CreateCbv(D3D12_CONSTANT_BUFFER_VIEW_DESC* cbvDesc);	//ViewIndex == -1  실패
		ViewIndex CreateSrv(Microsoft::WRL::ComPtr<ID3D12Resource> resource, D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc);
		ViewIndex CreateDsv(Microsoft::WRL::ComPtr<ID3D12Resource> resource, D3D12_DEPTH_STENCIL_VIEW_DESC* dsvDesc);
		ViewIndex CreateRtv(Microsoft::WRL::ComPtr<ID3D12Resource> resource,
			D3D12_RENDER_TARGET_VIEW_DESC* rtvDesc);
		ViewIndex CreateSPV(D3D12_SAMPLER_DESC* samplerDesc);



		D3D12_CPU_DESCRIPTOR_HANDLE GetCbvSrvUavCpuDescriptorHandle(ViewIndex viewIndex) const;
		D3D12_CPU_DESCRIPTOR_HANDLE GetDsvCpuDescriptorHandle(ViewIndex viewIndex) const;
		D3D12_CPU_DESCRIPTOR_HANDLE GetRtvCpuDescriptorHandle(ViewIndex viewIndex) const;
		D3D12_CPU_DESCRIPTOR_HANDLE GetSpvCpuDescriptorHandle(ViewIndex viewIndex) const;


		D3D12_GPU_DESCRIPTOR_HANDLE GetCbvSrvUavGpuDescriptorHandle(ViewIndex viewIndex)const;
		D3D12_GPU_DESCRIPTOR_HANDLE GetDsvGpuDescriptorHandle(ViewIndex viewIndex)const;
		D3D12_GPU_DESCRIPTOR_HANDLE GetRtvGpuDescriptorHandle(ViewIndex viewIndex)const;
		D3D12_GPU_DESCRIPTOR_HANDLE GetSpvGpuDescriptorHandle(ViewIndex viewIndex)const;

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetCbvSrvUavDescriptorHeap() const;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetDsvDescriptorHeap() const;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetRtvDescriptorHeap() const;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetSpvDescriptorHeap() const;


		void ReleaseCbvSrvUav(ViewIndex viewIndex);
		void ReleaseDsv(ViewIndex viewIndex);
		void ReleaseRtv(ViewIndex viewIndex);
		void ReleaseSampler(ViewIndex viewIndex);

	private:
		CbvSrvUavHeapManager* mCbvSrvUavDescriptorHeapManager;
		DsvHeapManager* mDsvDescriptorHeapManager;
		RtvHeapManager* mRtvDescriptorHeapManager;
		SamplerHeapManager* mSpvDescriptorHeapManager;



	};

}
