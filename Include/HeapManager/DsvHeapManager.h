#pragma once


#include"DescriptorHeapManager.h"

namespace Quad
{

	class HEAPMANAGER_API_LIB DsvHeapManager :public DescriptorHeapManager
	{
	public:
		DsvHeapManager() = default;
		~DsvHeapManager() = default;

		ViewIndex CreateDsv(Microsoft::WRL::ComPtr<ID3D12Resource> resource, D3D12_DEPTH_STENCIL_VIEW_DESC* dsvDesc);

	private:
		void InnerInit() override;

	};

}