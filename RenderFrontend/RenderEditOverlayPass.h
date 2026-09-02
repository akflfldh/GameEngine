#pragma once

#include <RenderFrontend/RenderPass.h>

namespace Render
{
struct EditorOverlayPassData
{
    CoreMath::Matrix4X4 mViewProj;
};

class RenderEditOverlayPass : public Render::IRenderPass
{
  public:
    RenderEditOverlayPass();
    ~RenderEditOverlayPass();

    virtual void AddToGraph(RenderPassGraph &renderPassGraph, const RenderPassSetUpData &passSetUpData) override;

    virtual void Execute(const RenderPassExecuteContext &renderPassExecuteContext) override;

    void SetPassConstantBufferResource(Render::BindingGpuResource bindingConstnatBuffer);
    void SetGlobalData(const Core::GlobalFrameData &globalFrameData);

  private:
    std::vector<RenderItem> BuildRenderItem(const RenderPassExecuteContext &executeContext);

    void BuildRenderItemMeshData(const Render::StaticMeshRenderCommnad &command, Render::RenderItem &renderItem);

    void BuildRenderItemBufferGpuResources(const Render::StaticMeshRenderCommnad &command,
                                           std::vector<BindingGpuResource> &bindingGpuResourceVector);
    void BuildRenderItemTexGpuResources(const MaterialRenderSnapshot &materialRenderSnapshot,
                                        std::vector<BindingGpuResource> &bindingGpuResourceVector);

  private:
    // 그냥 pass상수버퍼 만 참조하면된다.
    Render::BindingGpuResource mPassConstantBufferResource;

    Render::FrameContext mPassData;

    uint32_t mObjectBufferID;
};

} // namespace Render