#pragma once

#include"RenderType.h"
#include"IRenderSystem.h"
#ifdef D3DX


#include<Windows.h>
#include<unordered_map>

#include<memory>
#include<d3d12.h>
#include<wrl.h>
#include<dxgi.h>

namespace D3DRender
{
	class D3DRenderChannelSystem;
	class D3DWindowRenderManager;
	class D3DRenderSystem:public Render::IRenderSystem
	{
	public:
		D3DRenderSystem(Microsoft::WRL::ComPtr<IDXGIFactory> factory, Microsoft::WRL::ComPtr<ID3D12Device> device);
		~D3DRenderSystem();

		//return channel id 
		virtual int RegisterRenderChannel(const Render::CreationRenderChannelInfo& creationChannelInfo) override;


	private:
	
		std::unordered_map<HWND, std::vector<std::unique_ptr<D3DRenderChannelSystem>>> mRenderChannelSystemTable;
		Microsoft::WRL::ComPtr<IDXGIFactory> mFactory;
		Microsoft::WRL::ComPtr<ID3D12Device> mDevice;
		std::unique_ptr< D3DWindowRenderManager> mWindowRenderManager;


	};


}


#endif