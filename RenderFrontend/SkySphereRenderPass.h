#pragma once

#include <RenderFrontend/RenderPass.h>

namespace Render
{
struct SkySphereConstnatData
{
    CoreMath::Matrix4X4 mViewProj;
    CoreMath::Vector3 mCameraPosWorld;
    int mLightNums = 0;
    CoreMath::Vector4 mAmbientLight;
    CoreMath::Matrix4X4 mWorld;
};

class SkySphereRenderPass : public IRenderPass
{

  public:
    SkySphereRenderPass();
    virtual ~SkySphereRenderPass();

    virtual void AddToGraph(RenderPassGraph &renderPassGraph, const RenderPassSetUpData &passSetUpData) override;

    virtual void Execute(const RenderPassExecuteContext &) override;

  private:
    void SetGlobalData(const Core::GlobalFrameData &globalFrameData, const RenderPassExecuteContext &executeContext,
                       FrameContext &oPassData);
    std::vector<Render::RenderItem> BuildRenderItem(const RenderPassExecuteContext &executeContext);
    bool BuildRenderItemMeshData(const Render::SkySphereSnapshot &skySphereSnapshot, Render::RenderItem &renderItem);
    bool BuildRenderItemTexGpuResources(const SkySphereSnapshot &skySphereSnapshot,
                                        std::vector<BindingGpuResource> &bindingGpuResourceVector);
};

} // namespace Render