#pragma once


#include"RenderType.h"


#ifdef D3DX

#include<unordered_map>
#include<memory>
#include<d3d12.h>
#include<dxgi.h>
#include<wrl.h>
namespace D3DRender
{
	class D3DWindowRenderData;

	//window별 공통의데이터인 WindowRenderData를 관리하는 Manager클래스
	class D3DWindowRenderManager
	{
	public:
		//반드시 어느 한곳에서 생성후 호출할것
		static D3DWindowRenderManager* GetInstance();

		D3DWindowRenderManager(Microsoft::WRL::ComPtr<ID3D12Device> device, Microsoft::WRL::ComPtr<IDXGIFactory> factory);
		~D3DWindowRenderManager();

		
		bool RegisterWindow(const Render::CreationRenderChannelInfo& creationChannelInfo);
	

	private:

		static D3DWindowRenderManager* mInstance;

		Microsoft::WRL::ComPtr<ID3D12Device> mDevice;
		Microsoft::WRL::ComPtr<IDXGIFactory> mFactory;

		std::unordered_map<HWND, std::unique_ptr<D3DWindowRenderData>> mWindowRenderDataTable;


	};

}


#endif
