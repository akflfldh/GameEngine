#pragma once

#include <RenderFrontend/RenderPass.h>
#include <RenderSystem/RenderType.h>

#include <vector>
namespace Render
{

struct MainConstnatData
{
    CoreMath::Matrix4X4 mViewProj;
    CoreMath::Vector3 mCameraPosWorld;
    int mLightNums = 0;
    CoreMath::Vector4 mAmbientLight;
};
// 불투명 렌더파이프라인의 렌더메인패스
class RenderPassMain : public IRenderPass
{
  public:
    RenderPassMain();
    ~RenderPassMain();

    virtual void AddToGraph(RenderPassGraph &renderPassGraph, const RenderPassSetUpData &passSetUpData) override;

    virtual void Execute(const RenderPassExecuteContext &renderPassExecuteContext) override;

    void SetPassConstantBufferResource(Render::BindingGpuResource bindingConstnatBuffer);
    void SetGlobalData(const Core::GlobalFrameData &globalFrameData, const RenderPassExecuteContext &executeContext);

  private:
    // virtual void SetUpPassState(IRenderSystem *renderSystem) override;

    std::vector<RenderItem> BuildRenderItem(const RenderPassExecuteContext &executeContext);

    bool BuildRenderItemMeshData(const Render::StaticMeshRenderCommnad &command, Render::RenderItem &renderItem);

    void BuildRenderItemBufferGpuResources(const Render::StaticMeshRenderCommnad &command,
                                           std::vector<BindingGpuResource> &bindingGpuResourceVector);
    void BuildRenderItemTexGpuResources(const MaterialRenderSnapshot &materialRenderSnapshot, MaterialID gpuMaterialID,
                                        std::vector<BindingGpuResource> &bindingGpuResourceVector);

  private:
    // 그냥 pass상수버퍼 만 참조하면된다.
    Render::BindingGpuResource mPassConstantBufferResource;

    Render::FrameContext mPassData;

    uint32_t mObjectBufferID;
    uint32_t mMaterialBufferID;
};
} // namespace Render
