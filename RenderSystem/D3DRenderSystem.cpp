#include "D3DRenderSystem.h"

#include"D3DRenderChannelSystem.h"

#include"D3DWindowRenderManager.h"
D3DRender::D3DRenderSystem::D3DRenderSystem(Microsoft::WRL::ComPtr<IDXGIFactory> factory, Microsoft::WRL::ComPtr<ID3D12Device> device)
	:mFactory(factory),mDevice(device)
{
	mWindowRenderManager = std::make_unique< D3DWindowRenderManager>(mDevice, mFactory);




}

D3DRender::D3DRenderSystem::~D3DRenderSystem()
{
}

int D3DRender::D3DRenderSystem::RegisterRenderChannel(const Render::CreationRenderChannelInfo& creationChannelInfo)
{

	mRenderChannelSystemTable[creationChannelInfo.mWindowHandle].push_back(std::make_unique<D3DRenderChannelSystem>(creationChannelInfo));
	
	bool result =mWindowRenderManager->RegisterWindow(creationChannelInfo);
	

	return 0;
}
