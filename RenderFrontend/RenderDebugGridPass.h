#pragma once

#include <RenderFrontend/RenderPass.h>

namespace Render
{
struct DebugGridConstnatData
{
    CoreMath::Matrix4X4 mViewProj;
    CoreMath::Vector3 mCameraPosWorld;
};

class RenderDebugGridPass : public IRenderPass
{
  public:
    RenderDebugGridPass();
    ~RenderDebugGridPass();

    virtual void AddToGraph(RenderPassGraph &renderPassGraph, const RenderPassSetUpData &passSetUpData) override;

    virtual void Execute(const RenderPassExecuteContext &renderPassExecuteContext) override;

    void SetPassConstantBufferResource(Render::BindingGpuResource bindingConstnatBuffer);
    void SetGlobalData(const Core::GlobalFrameData &globalFrameData, const RenderPassExecuteContext &executeContext);

  private:
    //   std::vector<RenderItem> BuildRenderItem(const RenderPassExecuteContext &executeContext);

  private:
    Render::BindingGpuResource mPassConstantBufferResource;

    Render::FrameContext mPassData;
    uint32_t mPassMatID;
};

} // namespace Render
