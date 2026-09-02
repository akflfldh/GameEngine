#pragma once

#include <CoreMath/CoreMath.h>
#include <RenderFrontend/RenderPass.h>
namespace Render
{

struct BillboardPassConstantData
{
    CoreMath::Matrix4X4 mViewProj;
    CoreMath::Vector3 mCameraPosWorld;
};

class BillboardRenderPass : public IRenderPass
{
  public:
    BillboardRenderPass();
    virtual ~BillboardRenderPass();

    virtual void AddToGraph(RenderPassGraph &renderPassGraph, const RenderPassSetUpData &passSetUpData) override;

    virtual void Execute(const RenderPassExecuteContext &renderPassExecuteContext) override;

  private:
    void SetGlobalData(const Core::GlobalFrameData &globalFrameData, const RenderPassExecuteContext &executeContext);

    std::vector<Render::RenderItem> BuildRenderItem(const RenderPassExecuteContext &executeContext);

    void BuildObjectStructuredBuffer(RenderItem &renderItem, const std::vector<BillboardRenderCommand> &cmdList,
                                     const std::vector<size_t> &cmdIndexList, uint32_t &objectStructuredBufferOffset);

    void BuildRenderItemTexGpuResource(RenderItem &renderItem, CoreAsset::Texture *texture);

  private:
    Render::FrameContext mPassData;
    Render::MaterialID mGpuMaterialID;
};

} // namespace Render