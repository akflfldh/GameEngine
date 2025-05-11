#pragma once

#include"Predeclare.h"
#include"header.h"
#include"HeapManager/DescriptorHeapManagerMaster.h"
#include"Core/CoreDllExport.h"
namespace Quad
{
	class CORE_API_LIB StructuredBufferFactory
	{

	public:
		StructuredBufferFactory() = default;
		~StructuredBufferFactory() = default;

		void Initialize(Microsoft::WRL::ComPtr<ID3D12Device> device, DescriptorHeapManagerMaster * descriptorHeapManagerMaster);
		StructuredBuffer* CreateStructuredBuffer(UINT elementSize, UINT elementMaxNum);

	private:
		Microsoft::WRL::ComPtr<ID3D12Device> mDevice;
		DescriptorHeapManagerMaster* mDescriptorHeapManagerMaster;

	};

}

