#pragma once


#include"header.h"
#include"DescriptorHeapType.h"

#include"HeapManager/HeapManagerDllExport.h"


namespace Quad
{
	class HEAPMANAGER_API_LIB DescriptorHeapManager
	{
	public:
		DescriptorHeapManager();
		virtual ~DescriptorHeapManager() = 0;

		virtual void Initialize(Microsoft::WRL::ComPtr<ID3D12Device> device, UINT descriptorIncrementSize,
			D3D12_DESCRIPTOR_HEAP_TYPE type,
			D3D12_DESCRIPTOR_HEAP_FLAGS flags,
			UINT numDescriptors = VIEW_MAXNUM,
			UINT nodeMask = 0) final;

		virtual void Initialize(Microsoft::WRL::ComPtr<ID3D12Device> device, UINT mDescriptorIncrementSize,
			D3D12_DESCRIPTOR_HEAP_DESC heapDesc) final;

		D3D12_CPU_DESCRIPTOR_HANDLE GetCpuDescriptorHandle(ViewIndex viewIndex) const;
		D3D12_GPU_DESCRIPTOR_HANDLE GetGpuDescriptorHandle(ViewIndex viewIndex) const;

		void ReleaseDescriptor(ViewIndex viewIndex);
		UINT GetDescriptorIncrementSize() const;

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetDescriptorHeap() const;


	protected:
		Microsoft::WRL::ComPtr<ID3D12Device> GetDevice() const;
		ViewIndex GetAvailableViewFirstIndex()const;
		D3D12_CPU_DESCRIPTOR_HANDLE UseAvailableFirstView();
		D3D12_DESCRIPTOR_HEAP_TYPE GetHeapType() const;

	private:
		virtual void InnerInit() = 0;

		Microsoft::WRL::ComPtr<ID3D12Device> mDevice;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDescriptorHeap;
		UINT mDescriptorIncrementSize;


		struct FlagElement
		{
			bool mUsage = false;
			ViewIndex mNextIndex = -1;
		};
		std::vector<FlagElement> mDescriptorFlagVector;
		ViewIndex mAvailableViewFirstIndex = 0;
		ViewIndex mNotAvailableViewFirstIndex = -1;
	};

}