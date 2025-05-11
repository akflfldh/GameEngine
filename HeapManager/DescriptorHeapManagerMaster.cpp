#include "HeapManager/DescriptorHeapManagerMaster.h"

namespace Quad
{
	DescriptorHeapManagerMaster::DescriptorHeapManagerMaster()
		:mCbvSrvUavDescriptorHeapManager(nullptr), mDsvDescriptorHeapManager(nullptr),
		mRtvDescriptorHeapManager(nullptr), mSpvDescriptorHeapManager(nullptr)
	{
	}

	void DescriptorHeapManagerMaster::Initialize(CbvSrvUavHeapManager* cbvSrvUavDescriptorHeapManager,
		DsvHeapManager* dsvDescriptorHeapManager,
		RtvHeapManager* rtvDescriptorHeapManager,
		SamplerHeapManager* spvDescriptorHeapManager)
	{
		mCbvSrvUavDescriptorHeapManager = cbvSrvUavDescriptorHeapManager;
		mDsvDescriptorHeapManager = dsvDescriptorHeapManager;
		mRtvDescriptorHeapManager = rtvDescriptorHeapManager;
		mSpvDescriptorHeapManager = spvDescriptorHeapManager;

	}

	ViewIndex DescriptorHeapManagerMaster::CreateCbv(D3D12_CONSTANT_BUFFER_VIEW_DESC* cbvDesc)
	{
		return mCbvSrvUavDescriptorHeapManager->CreateCbv(cbvDesc);
	}

	ViewIndex DescriptorHeapManagerMaster::CreateSrv(Microsoft::WRL::ComPtr<ID3D12Resource> resource,
		D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc)
	{
		return mCbvSrvUavDescriptorHeapManager->CreateSrv(resource, srvDesc);
	}

	ViewIndex DescriptorHeapManagerMaster::CreateDsv(Microsoft::WRL::ComPtr<ID3D12Resource> resource,
		D3D12_DEPTH_STENCIL_VIEW_DESC* dsvDesc)
	{
		return mDsvDescriptorHeapManager->CreateDsv(resource, dsvDesc);
	}

	ViewIndex DescriptorHeapManagerMaster::CreateRtv(Microsoft::WRL::ComPtr<ID3D12Resource> resource,
		D3D12_RENDER_TARGET_VIEW_DESC* rtvDesc)
	{
		return mRtvDescriptorHeapManager->CreateRtv(resource, rtvDesc);
	}

	ViewIndex DescriptorHeapManagerMaster::CreateSPV(D3D12_SAMPLER_DESC* samplerDesc)
	{
		return mSpvDescriptorHeapManager->CreateSPV(samplerDesc);
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeapManagerMaster::GetCbvSrvUavCpuDescriptorHandle(ViewIndex viewIndex) const
	{
		return mCbvSrvUavDescriptorHeapManager->GetCpuDescriptorHandle(viewIndex);
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeapManagerMaster::GetDsvCpuDescriptorHandle(ViewIndex viewIndex) const
	{
		return mDsvDescriptorHeapManager->GetCpuDescriptorHandle(viewIndex);
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeapManagerMaster::GetRtvCpuDescriptorHandle(ViewIndex viewIndex) const
	{
		return mRtvDescriptorHeapManager->GetCpuDescriptorHandle(viewIndex);
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeapManagerMaster::GetSpvCpuDescriptorHandle(ViewIndex viewIndex) const
	{
		return mSpvDescriptorHeapManager->GetCpuDescriptorHandle(viewIndex);
	}

	D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeapManagerMaster::GetCbvSrvUavGpuDescriptorHandle(ViewIndex viewIndex) const
	{
		return mCbvSrvUavDescriptorHeapManager->GetGpuDescriptorHandle(viewIndex);
	}

	D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeapManagerMaster::GetDsvGpuDescriptorHandle(ViewIndex viewIndex) const
	{
		return mDsvDescriptorHeapManager->GetGpuDescriptorHandle(viewIndex);
	}

	D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeapManagerMaster::GetRtvGpuDescriptorHandle(ViewIndex viewIndex) const
	{
		return mRtvDescriptorHeapManager->GetGpuDescriptorHandle(viewIndex);
	}

	D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeapManagerMaster::GetSpvGpuDescriptorHandle(ViewIndex viewIndex) const
	{
		return mSpvDescriptorHeapManager->GetGpuDescriptorHandle(viewIndex);
	}

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> DescriptorHeapManagerMaster::GetCbvSrvUavDescriptorHeap() const
	{
		return mCbvSrvUavDescriptorHeapManager->GetDescriptorHeap();
	}

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> DescriptorHeapManagerMaster::GetDsvDescriptorHeap() const
	{
		return mDsvDescriptorHeapManager->GetDescriptorHeap();
	}

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> DescriptorHeapManagerMaster::GetRtvDescriptorHeap() const
	{
		return mRtvDescriptorHeapManager->GetDescriptorHeap();
	}

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> DescriptorHeapManagerMaster::GetSpvDescriptorHeap() const
	{
		return mSpvDescriptorHeapManager->GetDescriptorHeap();
	}

	void DescriptorHeapManagerMaster::ReleaseCbvSrvUav(ViewIndex viewIndex)
	{
		mCbvSrvUavDescriptorHeapManager->ReleaseDescriptor(viewIndex);
	}

	void DescriptorHeapManagerMaster::ReleaseDsv(ViewIndex viewIndex)
	{
		mDsvDescriptorHeapManager->ReleaseDescriptor(viewIndex);
	}

	void DescriptorHeapManagerMaster::ReleaseRtv(ViewIndex viewIndex)
	{
		mRtvDescriptorHeapManager->ReleaseDescriptor(viewIndex);
	}

	void DescriptorHeapManagerMaster::ReleaseSampler(ViewIndex viewIndex)
	{
		mSpvDescriptorHeapManager->ReleaseDescriptor(viewIndex);
	}

}