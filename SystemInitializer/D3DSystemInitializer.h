#pragma once



#define D3DX

#ifdef D3DX

#include"ISystemInitializer.h"
#include<d3d12.h>
#include<wrl.h>
#include<dxgi1_4.h>

#include"SystemInitializerDll.h"

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d12.lib")


namespace D3DGRM { class D3DGpuResourceManager; }
namespace D3DRender { class D3DRenderSystem; class D3DMaterialManager; }

namespace D3DSystemInitializer
{


	class SYSTEM_INITIALIZER_API D3DSystemInitializer:public SystemInitializer::ISystemInitializer
	{
	public:
		D3DSystemInitializer();
		virtual ~D3DSystemInitializer() = default;

		virtual void Initialize()override;


	private:
		Microsoft::WRL::ComPtr<IDXGIFactory4> mFactory;
		Microsoft::WRL::ComPtr<IDXGIAdapter> mAdapter;
		Microsoft::WRL::ComPtr<ID3D12Device> mDevice;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> mCommandQueue;

		class D3DGRM::D3DGpuResourceManager* mD3DGpuResourceManager;
		class D3DRender::D3DRenderSystem* mD3DRenderSystem;
	//	class D3DRender::D3DMaterialManager* mMaterialManager;
	};

}



#endif