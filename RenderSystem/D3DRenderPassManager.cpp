#include "D3DRenderPassManager.h"

#include"D3DOpaqueRenderPass.h"
#include"D3DWindowRenderData.h"
#include"D3DMaterialManager.h"
#include<D3DGpuTexture.h>


D3DRender::D3DRenderPassManager::D3DRenderPassManager(Microsoft::WRL::ComPtr<ID3D12Device> device, D3DGRM::D3DGpuResourceManager* gpuResourceManager, std::shared_ptr<D3DRender::D3DMaterialManager> materialManager)
	:mDevice(device),mGpuResourceManager(gpuResourceManager),mMaterialManager(materialManager)
{
}

D3DRender::D3DRenderPassManager::~D3DRenderPassManager()
{
}


//일반적인 ui의 경우와 
//3d경우 두개에대해서만 파이프라인을만들면되고 

//특정 렌더패스를 끄고 키면된다.
//

std::vector<std::unique_ptr<D3DRender::ID3DRenderPass>> D3DRender::D3DRenderPassManager::CreateDefaultRenderPipeline(
	std::shared_ptr<D3DRender::D3DWindowRenderData> windowRenderData)
{

	std::vector<std::unique_ptr<ID3DRenderPass>> renderPassSet;


	
	D3DGRM::D3DGpuTexture* backBuffer[2];
	backBuffer[0]= static_cast<D3DGRM::D3DGpuTexture*>(windowRenderData->GetBackBuffer(0).getResource());
	backBuffer[1]= static_cast<D3DGRM::D3DGpuTexture*>(windowRenderData->GetBackBuffer(1).getResource());
	D3DGRM::D3DGpuTexture* depthStencilBuffer = static_cast<D3DGRM::D3DGpuTexture*>(windowRenderData->GetDepthStencilBuffer().getResource());

	//일단 지금은 기본 렌더패스 하나만 들어있다.
	
	std::unique_ptr<D3DOpaqueRenderPass> opaqueRenderPass = std::make_unique<D3DOpaqueRenderPass>(mDevice,mGpuResourceManager,
		mMaterialManager,backBuffer, depthStencilBuffer);


	renderPassSet.push_back(std::move(opaqueRenderPass));


	return renderPassSet;





}
