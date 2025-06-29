#include "D3DWindowRenderManager.h"

#include"D3DWindowRenderData.h"
#include<assert.h>

#include<iostream>
D3DRender::D3DWindowRenderManager* D3DRender::D3DWindowRenderManager::mInstance =nullptr;

D3DRender::D3DWindowRenderManager::D3DWindowRenderManager(Microsoft::WRL::ComPtr<ID3D12Device> device, Microsoft::WRL::ComPtr<IDXGIFactory> factory)
	:mDevice(device),mFactory(factory)
{
	assert(mInstance == nullptr);
	std::cerr << "D3DWindowRenderManager 초과생성\n";
	mInstance = this;

}

D3DRender::D3DWindowRenderManager::~D3DWindowRenderManager()
{
}

D3DRender::D3DWindowRenderManager* D3DRender::D3DWindowRenderManager::GetInstance()
{
	assert(mInstance != nullptr);
	std::cerr << "D3DWindowRenderManager가 생성되지 않았습니다.\n";

	return mInstance;

}


bool D3DRender::D3DWindowRenderManager::RegisterWindow(const Render::CreationRenderChannelInfo & creationChannelInfo)
{
	HWND windowHandle = creationChannelInfo.mWindowHandle;
	auto it = mWindowRenderDataTable.find(windowHandle);

	if (it != mWindowRenderDataTable.end())
	{

		//이미있다는 로그
		return true;
	}

	mWindowRenderDataTable[windowHandle]=  std::make_unique<D3DWindowRenderData>(this,mDevice,mFactory, creationChannelInfo);


}
