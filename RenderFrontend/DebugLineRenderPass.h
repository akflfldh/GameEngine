#pragma once

#include <RenderFrontend/RenderPass.h>

namespace Render
{

struct DebugLineConstantPassData
{
    CoreMath::Matrix4X4 mViewProj;
};

class DebugLineRenderPass : public IRenderPass
{
  public:
    DebugLineRenderPass();
    virtual ~DebugLineRenderPass();

    virtual void AddToGraph(RenderPassGraph &renderPassGraph, const RenderPassSetUpData &passSetUpData) override;

    virtual void Execute(const RenderPassExecuteContext &) override;

  private:
    void SetGlobalData(const Core::GlobalFrameData &globalFrameData, const RenderPassExecuteContext &executeContext,
                       FrameContext &oFrameContext);
    std::vector<RenderItem> BuildRenderItem(const RenderPassExecuteContext &executeContext);

    GRM::IGpuResource *GetVertexBuffer(size_t vertexNum);

  private:
    //  GRM::IGpuResource *mVertexBuffer = nullptr;
    GRM::GRMPtr mVertexBuffer;
    size_t mMaxVertexNum = 0;
    Render::MaterialID mGpuMaterialID;
};

} // namespace Render