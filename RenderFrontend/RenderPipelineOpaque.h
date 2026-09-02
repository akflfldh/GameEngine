#pragma once

#include <RenderFrontend/RenderPassMain.h>
#include <RenderFrontend/RenderPipeline.h>

namespace Render
{

class RenderPipelineOpaque : public RenderPipeline
{
  public:
    RenderPipelineOpaque();
    ~RenderPipelineOpaque();

   // void Execute(const std::vector<Core::RenderProxy *> &renderProxyList) override;

    virtual void SetGlobalData(const Core::GlobalFrameData &globalFrameData) override;

    virtual void RegisterAllPass(RenderPassGraph *renderPassGraph
                               ) override;

  private:
    std::vector<RenderItem> GetMainPassRenderItem(const std::vector<Core::RenderProxy *> &mRenderProxyList);
    void BuildStaticRenderItem(std::vector<Render::RenderItem> &oRenderItemList,
                               Core::StaticMeshRenderProxy *staticMeshRenderProxy);

  private:
    RenderPassMain mMainPass;
};

} // namespace Render