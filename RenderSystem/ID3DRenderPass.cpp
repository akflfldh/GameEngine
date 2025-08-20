#include "pch.h"

#include "RenderSystem/D3DMaterialManager.h"
#include "RenderSystem/ID3DRenderPass.h"

D3DRender::ID3DRenderPass::ID3DRenderPass(std::shared_ptr<D3DRender::D3DMaterialManager> materialManager)
    : mMaterialManager(materialManager)
{
}

D3DRender::ID3DRenderPass::~ID3DRenderPass() {}
