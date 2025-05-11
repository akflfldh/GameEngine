#pragma once

#include"DescriptorHeapManager.h"
namespace Quad
{
	class HEAPMANAGER_API_LIB RtvHeapManager :public DescriptorHeapManager
	{
	public:
		RtvHeapManager() = default;
		~RtvHeapManager() = default;

		ViewIndex CreateRtv(Microsoft::WRL::ComPtr<ID3D12Resource> resource,
			D3D12_RENDER_TARGET_VIEW_DESC* rtvDesc);


	private:
		void InnerInit() override;


	};

}