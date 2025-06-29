#include "D3DGpuDescriptorHeapManager.h"

D3DGRM::D3DGpuDescriptorHeapManager::D3DGpuDescriptorHeapManager(Microsoft::WRL::ComPtr<ID3D12Device> device, D3D12_DESCRIPTOR_HEAP_TYPE type,
	size_t descriptorNum, D3D12_DESCRIPTOR_HEAP_FLAGS  heapFlag )
{

	mDevice = device;
	mHeapFlag = heapFlag;
	mDescriptorIncrementSize =	mDevice->GetDescriptorHandleIncrementSize(type);
	

	D3D12_DESCRIPTOR_HEAP_DESC heapDesc;
	heapDesc.Type = type;
	heapDesc.NodeMask = 0;
	heapDesc.NumDescriptors = descriptorNum;
	heapDesc.Flags = mHeapFlag;

	mDevice->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(mDescriptorHeap.GetAddressOf()));


	mDescriptorNextOffset = 0;
	mFreeDecriptorOffsetVector.reserve(descriptorNum);


}

D3DGRM::D3DGpuDescriptorHeapManager::~D3DGpuDescriptorHeapManager()
{


}


D3DGRM::D3DDescriptorHandle D3DGRM::D3DGpuDescriptorHeapManager::Allocate()
{

	size_t offset = mDescriptorNextOffset;
	if (mFreeDecriptorOffsetVector.size() != 0)
	{
		offset = mFreeDecriptorOffsetVector.back();
		mFreeDecriptorOffsetVector.pop_back();
	}
	else
		mDescriptorNextOffset++;

	D3DDescriptorHandle handle;
	handle.mCpuDescriptorHandle.ptr = mDescriptorHeap->GetCPUDescriptorHandleForHeapStart().ptr + offset * mDescriptorIncrementSize;	


	if (mHeapFlag == D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE)
	{
		//visible일경우에만 gpuDescriptorHandle이 유효하다.
		handle.mGpuDescriptorHandle.ptr = mDescriptorHeap->GetGPUDescriptorHandleForHeapStart().ptr + offset * mDescriptorIncrementSize;

	}
	else
		handle.mGpuDescriptorHandle.ptr = 0;


	return handle;

}

void D3DGRM::D3DGpuDescriptorHeapManager::Free(D3DDescriptorHandle handle)
{

	handle.mCpuDescriptorHandle.ptr -= mDescriptorHeap->GetCPUDescriptorHandleForHeapStart().ptr;
	size_t offset  = handle.mCpuDescriptorHandle.ptr / mDescriptorIncrementSize;

	mFreeDecriptorOffsetVector.push_back(offset);


}
