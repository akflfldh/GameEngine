#pragma once

#include <RenderFrontend/RenderPass.h>
#include <RenderSystem/RenderType.h>

namespace Render
{
struct OutlineConstantData
{
    CoreMath::Matrix4X4 mViewProj;
    //  float mOutlineColor[4];
};

class RenderOutlinePass : public IRenderPass
{
  public:
    RenderOutlinePass();
    virtual ~RenderOutlinePass();

    virtual void AddToGraph(RenderPassGraph &renderPassGraph, const RenderPassSetUpData &passSetUpData) override;

    virtual void Execute(const RenderPassExecuteContext &) override;
    void SetPassConstantBufferResource(Render::BindingGpuResource bindingConstnatBuffer);
    void SetGlobalData(const Core::GlobalFrameData &globalFrameData);

  private:
    std::vector<RenderItem> BuildRenderItem(const RenderPassExecuteContext &executeContext);

    void ChangeMaterial(std::vector<RenderItem> &renderItemVec);
    void BuildRenderItemBufferGpuResources(const Render::StaticMeshOutlineRenderCommand &command,
                                           std::vector<BindingGpuResource> &bindingGpuResourceVector);

  private:
    Render::BindingGpuResource mPassConstantBufferResource;
    Render::FrameContext mPassData;
    Render::MaterialID mStaticMeshStencilMaterialID;
    Render::MaterialID mStaticMeshOutlineMaterialID;

    uint32_t mObjectBufferID;
};

} // namespace Render