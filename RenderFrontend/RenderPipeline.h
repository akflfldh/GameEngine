#pragma once
#include <vector>

#include <Core/IRenderProxyManager.h>
namespace Core
{
struct GlobalFrameData;
}

namespace Render
{
class RenderPass;
class IRenderSystem;
class RenderPassGraph;

class RenderPipeline
{
  public:
    RenderPipeline();
    virtual ~RenderPipeline() = 0;

    //virtual void Execute(const std::vector<Core::RenderProxy *> &renderProxyList) = 0;
    virtual void SetGlobalData(const Core::GlobalFrameData &globalFrameData) = 0;

    virtual void RegisterAllPass(RenderPassGraph *renderPassGraph) = 0;



  protected:
    IRenderSystem *GetRenderSystem() const;

  private:
    IRenderSystem *mRenderSystem;
};

} // namespace Render
