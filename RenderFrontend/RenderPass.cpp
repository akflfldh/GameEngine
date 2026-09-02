#include "RenderPass.h"
#include <D3DGpuResourceManager/GpuBufferContextSystem.h>
#include <D3DGpuResourceManager/GpuSamplerSystem.h>
#include <D3DGpuResourceManager/IGpuResourceManager.h>
#include <RenderFrontend/AssetResolver.h>
#include <RenderFrontend/RenderUploadManager.h>
#include <RenderSystem/IMaterialManager.h>
Render::IRenderPass::IRenderPass()
    : mAssetResolver(AssetResolver::GetInstance()), mGpuMaterialManager(Render::IMaterialManager::GetInstance()),
      mGpuSamplerSystem(GRM::GpuSamplerSystem::GetInstance()),
      mGpuBufferContextSystem(GRM::GpuBufferContextSystem::GetInstance()),
      mGpuResourceManager(GRM::IGpuResourceManager::GetInstance()),
      mRenderUploadManager(Render::RenderUploadManager::GetInstance())
{
}

Render::IRenderPass::~IRenderPass() {}

const std::string &Render::IRenderPass::GetName() const
{
    return mName;
}

void Render::IRenderPass::SetOutputTarget(const std::string &name)
{
    mOutputTargetName = name;
}
void Render::IRenderPass::SetOutputDepthStencil(const std::string &name)
{

    mOutputDepthStencilName = name;
}
uint32_t Render::IRenderPass::GetBufferID() const
{
    return mBufferID;
}
Render::RECT Render::IRenderPass::GetViewport() const
{

    return mViewport;
}

bool Render::IRenderPass::GetUseDefaultViewport() const
{
    return mUseDefaultViewport;
}

void Render::IRenderPass::SetClearRenderTarget(bool clear)
{
    mClearRenderTarget = clear;
}

void Render::IRenderPass::SetPassName(const std::string &name)
{
    mName = name;
}

void Render::IRenderPass::SetBufferID(uint32_t bufferiD)
{
    mBufferID = bufferiD;
}

void Render::IRenderPass::SetViewport(const Render::RECT &viewport)
{

    mViewport = viewport;
}
