#include "HeapManager/DescriptorHeapManager.h"
namespace Quad
{

	DescriptorHeapManager::~DescriptorHeapManager()
	{

	}


	DescriptorHeapManager::DescriptorHeapManager() :mDescriptorFlagVector(VIEW_MAXNUM)
	{
		for (size_t i = 0; i < mDescriptorFlagVector.size() - 1; ++i)
		{
			mDescriptorFlagVector[i].mNextIndex = i + 1;
		}



	}

	void DescriptorHeapManager::Initialize(Microsoft::WRL::ComPtr<ID3D12Device> device,
		UINT descriptorIncrementSize,
		D3D12_DESCRIPTOR_HEAP_TYPE type,
		D3D12_DESCRIPTOR_HEAP_FLAGS flags,
		UINT numDescriptors,
		UINT nodeMask)
	{

		D3D12_DESCRIPTOR_HEAP_DESC descriptorHeap;
		descriptorHeap.Flags = flags;
		descriptorHeap.Type = type;
		descriptorHeap.NumDescriptors = numDescriptors;
		descriptorHeap.NodeMask = nodeMask;

		Initialize(device, descriptorIncrementSize, descriptorHeap);


	}

	void DescriptorHeapManager::Initialize(Microsoft::WRL::ComPtr<ID3D12Device> device,
		UINT descriptorIncrementSize, D3D12_DESCRIPTOR_HEAP_DESC heapDesc)
	{
		if (heapDesc.NumDescriptors > VIEW_MAXNUM)
			throw "디스크립터힙 생성실패  (개수 초과)";

		mDevice = device;
		mDescriptorIncrementSize = descriptorIncrementSize;
		D3D12_DESCRIPTOR_HEAP_DESC heapDsc = heapDesc;

		ThrowIfFailed(mDevice->CreateDescriptorHeap(&heapDsc, IID_PPV_ARGS(mDescriptorHeap.GetAddressOf())));


		InnerInit();

	}

	Microsoft::WRL::ComPtr<ID3D12Device> DescriptorHeapManager::GetDevice() const
	{
		return mDevice;
	}

	ViewIndex DescriptorHeapManager::GetAvailableViewFirstIndex() const
	{

		return mAvailableViewFirstIndex;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeapManager::UseAvailableFirstView()
	{
		ViewIndex availableFirstIndex = GetAvailableViewFirstIndex();
		D3D12_CPU_DESCRIPTOR_HANDLE handle = mDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

		handle.ptr += mDescriptorIncrementSize * availableFirstIndex;


		ViewIndex newNotAvailableViewIndex = availableFirstIndex;
		mAvailableViewFirstIndex = mDescriptorFlagVector[newNotAvailableViewIndex].mNextIndex;
		mDescriptorFlagVector[newNotAvailableViewIndex].mNextIndex = mNotAvailableViewFirstIndex;
		mNotAvailableViewFirstIndex = newNotAvailableViewIndex;


		return handle;
	}

	D3D12_DESCRIPTOR_HEAP_TYPE DescriptorHeapManager::GetHeapType() const
	{
		return mDescriptorHeap->GetDesc().Type;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeapManager::GetCpuDescriptorHandle(ViewIndex viewIndex) const
	{
		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = mDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		cpuHandle.ptr += mDescriptorIncrementSize * viewIndex;
		return cpuHandle;
	}

	D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeapManager::GetGpuDescriptorHandle(ViewIndex viewIndex) const
	{

		D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = mDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
		gpuHandle.ptr += mDescriptorIncrementSize * viewIndex;
		return gpuHandle;
	}

	void DescriptorHeapManager::ReleaseDescriptor(ViewIndex viewIndex)
	{

		ViewIndex currIndex = mNotAvailableViewFirstIndex;
		ViewIndex preIndex = mNotAvailableViewFirstIndex;
		while (currIndex != -1)
		{
			if (currIndex == viewIndex)
			{
				mDescriptorFlagVector[preIndex].mNextIndex = mDescriptorFlagVector[currIndex].mNextIndex;
				if (currIndex == mNotAvailableViewFirstIndex)
				{
					mNotAvailableViewFirstIndex = mDescriptorFlagVector[currIndex].mNextIndex;
				}
				mDescriptorFlagVector[currIndex].mNextIndex = mAvailableViewFirstIndex;
				mAvailableViewFirstIndex = currIndex;
				break;
			}

			preIndex = currIndex;
			currIndex = mDescriptorFlagVector[currIndex].mNextIndex;
		}

	}

	UINT DescriptorHeapManager::GetDescriptorIncrementSize() const
	{
		return mDescriptorIncrementSize;
	}

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> DescriptorHeapManager::GetDescriptorHeap() const
	{
		return mDescriptorHeap;
	}


}
