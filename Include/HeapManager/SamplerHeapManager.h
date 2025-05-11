#pragma once

#include"HeapManager/DescriptorHeapManager.h"


#include"HeapManager/HeapManagerDllExport.h"
namespace Quad
{

	class HEAPMANAGER_API_LIB	SamplerHeapManager :public DescriptorHeapManager
	{
	public:
		SamplerHeapManager() = default;
		~SamplerHeapManager() = default;

		ViewIndex CreateSPV(D3D12_SAMPLER_DESC* samplerDesc);

	private:
		void InnerInit() override;



	};

}
