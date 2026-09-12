#pragma once
#include <CoreMath/CoreMath.h>
#include <RenderFrontend/RenderPass.h>
#include <vector>
namespace Render
{
struct ShadowPassBuffer
{
    CoreMath::Matrix4X4 mViewProj;
};

class ShadowRenderPass : public IRenderPass
{

  public:
    ShadowRenderPass();
    virtual ~ShadowRenderPass();

    virtual void AddToGraph(RenderPassGraph &renderPassGraph, const RenderPassSetUpData &passSetUpData) override;

    virtual void Execute(const RenderPassExecuteContext &renderPassExecuteContext) override;

  private:
    void SetGlobalData(const RenderPassExecuteContext &executeContext);
    std::vector<RenderItem> BuildRenderItem(const RenderPassExecuteContext &executeContex);
    bool BuildRenderItemMeshData(const Render::StaticMeshRenderCommnad &command, Render::RenderItem &renderItem);
    void BuildRenderItemBufferGpuResources(const Render::StaticMeshRenderCommnad &command,
                                           std::vector<BindingGpuResource> &bindingGpuResourceVector);
    Render::MaterialID mGpuMaterialID;

    Render::FrameContext mPassData;
};

} // namespace Render