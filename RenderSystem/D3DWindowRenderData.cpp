#include "D3DWindowRenderData.h"

#include<IGpuResourceManager.h>

namespace D3DX
{



	D3DWindowRenderData::D3DWindowRenderData(D3DWindowRenderManager* windowRenderManager,Microsoft::WRL::ComPtr<ID3D12Device> device ,Microsoft::WRL::ComPtr<IDXGIFactory> factory,const Render::CreationRenderChannelInfo & creationInfo)
		:mWindowRenderManager(windowRenderManager), mDevice(device),mFactory(factory), mWindowHandle(creationInfo.mWindowHandle),mBackBufferForamt(DXGI_FORMAT_R8G8B8A8_UNORM)
	{
		mGpuResourceManager = GRM::IGpuResourceManager::GetInstance();

		CreateSwapChain(creationInfo);
	
		//default back buffer, - 2개
		//default depth stencil buffer -1개



	}

	D3DWindowRenderData::~D3DWindowRenderData()
	{


	}

	void D3DWindowRenderData::ResizeWindow()
	{
		RECT clientSize;
		GetClientRect(mWindowHandle, &clientSize);

		ResizeBackBuffer(clientSize.right, clientSize.bottom);



	}

	void D3DWindowRenderData::CreateSwapChain(const Render::CreationRenderChannelInfo& creationInfo)
	{


		HWND windowHandle = creationInfo.mWindowHandle;
		RECT windowSize;	//창의 클라이언트 크기
		GetClientRect(windowHandle, &windowSize);

		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		swapChainDesc.BufferDesc.Width = windowSize.right;
		swapChainDesc.BufferDesc.Height = windowSize.bottom;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 60;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 1;
		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 2;
		swapChainDesc.OutputWindow = creationInfo.mWindowHandle;
		swapChainDesc.Windowed = true;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;      //direct12에서는 flip만사용가능. 그리고 후면버퍼에서 멀티샘플링불가능
		swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;


		HRESULT hresult  = mFactory->CreateSwapChain(mDevice.Get(), &swapChainDesc, mSwapChain.GetAddressOf());
		mSwapChain->GetBuffer(0, IID_PPV_ARGS(mBackBuffer[0].GetAddressOf()));
		mSwapChain->GetBuffer(0, IID_PPV_ARGS(mBackBuffer[1].GetAddressOf()));


		ResizeWindow();

	}
	void D3DWindowRenderData::ResizeBackBuffer(UINT clientWidth, UINT clientHeight)
	{
		
		for(size_t bufferIndex= 0; bufferIndex<2; ++bufferIndex)
		{
			mGpuResourceManager->ReleaseSwapChainBackBufferView(mBackBuffer[bufferIndex].Get());
			mBackBuffer[bufferIndex].Reset();
		}


		HRESULT reulst = mSwapChain->ResizeBuffers(2, clientWidth, clientHeight, mBackBufferForamt, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);

		mSwapChain->GetBuffer(0, IID_PPV_ARGS(mBackBuffer[0].GetAddressOf()));
		mSwapChain->GetBuffer(1, IID_PPV_ARGS(mBackBuffer[1].GetAddressOf()));

		mGpuResourceManager->RegisterSwapChainBackBufferView(mBackBuffer[0].Get());
		mGpuResourceManager->RegisterSwapChainBackBufferView(mBackBuffer[1].Get());


	}
	void D3DWindowRenderData::CreateDepthStencilBuffer(UINT clientWidth, UINT clientHeight)
	{



		//mGpuResourceManager->Create2DTextureResource


	}
}