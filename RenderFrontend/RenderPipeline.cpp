#include "RenderPipeline.h"
#include "RenderSystem/IRenderSystem.h"
Render::RenderPipeline::RenderPipeline() : mRenderSystem(IRenderSystem::GetInstance()) {}

Render::RenderPipeline::~RenderPipeline() {}

Render::IRenderSystem *Render::RenderPipeline::GetRenderSystem() const
{
    return mRenderSystem;
}
