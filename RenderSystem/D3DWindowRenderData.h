#pragma once

#include"RenderType.h"

#ifdef D3DX


#include<d3d12.h>
#include<wrl.h>
#include<dxgi.h>
namespace GRM { class IGpuResourceManager; }

namespace D3DX
{

	class D3DWindowRenderManager;
	class D3DWindowRenderData
	{
	public:
		D3DWindowRenderData(D3DWindowRenderManager * windowRenderManager ,Microsoft::WRL::ComPtr<ID3D12Device> device, Microsoft::WRL::ComPtr<IDXGIFactory> factory,
			const Render::CreationRenderChannelInfo& creationInfo);
		~D3DWindowRenderData();

		void ResizeWindow();
	private:
		void CreateSwapChain(const Render::CreationRenderChannelInfo& creationInfo);
		void ResizeBackBuffer(UINT clientWidth, UINT clientHeight);
		void CreateDepthStencilBuffer(UINT clientWidth, UINT clientHeight);


	private:
		D3DWindowRenderManager* mWindowRenderManager;

		Microsoft::WRL::ComPtr<ID3D12Device> mDevice;
		Microsoft::WRL::ComPtr<IDXGIFactory> mFactory;
		Microsoft::WRL::ComPtr<IDXGISwapChain> mSwapChain;
		
		Microsoft::WRL::ComPtr<ID3D12Resource> mBackBuffer[2];

		HWND mWindowHandle;
		DXGI_FORMAT mBackBufferForamt;

		GRM::IGpuResourceManager* mGpuResourceManager;

	};

	



}



#endif