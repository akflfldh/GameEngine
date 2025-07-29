#include "ID3DRenderPass.h"
#include"D3DMaterialManager.h"

D3DRender::ID3DRenderPass::ID3DRenderPass(std::shared_ptr<D3DRender::D3DMaterialManager> materialManager)
	:mMaterialManager(materialManager)
{
}

D3DRender::ID3DRenderPass::~ID3DRenderPass()
{




}
