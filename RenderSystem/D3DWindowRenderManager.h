#pragma once

#include<dxgi.h>
#include"RenderType.h"

#include<assert.h>
#include<iostream>
#include<unordered_map>
#include<memory>

#include<d3d12.h>

#include<wrl.h>
#ifdef D3DX
#include"RenderSystemDllMacro.h"
namespace D3DRender
{
	class D3DWindowRenderData;

	//window별 공통의데이터인 WindowRenderData를 관리하는 Manager클래스
	class RENDER_SYSTEM_API D3DWindowRenderManager
	{
	public:
		//반드시 어느 한곳에서 생성후 호출할것
		static D3DWindowRenderManager* GetInstance();

		D3DWindowRenderManager(Microsoft::WRL::ComPtr<ID3D12Device> device, Microsoft::WRL::ComPtr<IDXGIFactory> factory);
		~D3DWindowRenderManager();

		
		bool RegisterWindow(const Render::CreationRenderChannelInfo& creationChannelInfo);
		
		std::shared_ptr<D3DWindowRenderData> GetWindowRenderData(HWND hwnd) const;
	private:

		static D3DWindowRenderManager* mInstance;

		Microsoft::WRL::ComPtr<ID3D12Device> mDevice;
		Microsoft::WRL::ComPtr<IDXGIFactory> mFactory;

		std::unordered_map<HWND, std::shared_ptr<D3DWindowRenderData>> mWindowRenderDataTable;


	};

}


#endif
