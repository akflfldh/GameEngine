#pragma once

#include <RenderFrontend/RenderPass.h>
#include <string>
namespace Render
{

class BloomRenderPass : public IRenderPass
{

  public:
    struct BloomConstantData
    {
        int gWidth;
        int gHeight;
        float gThreshold = 1.0f;
    };

    enum class EBloomStage
    {

        eVertical = 0,
        eHorizontal
    };

    BloomRenderPass();
    virtual ~BloomRenderPass();

    virtual void AddToGraph(RenderPassGraph &renderPassGraph, const RenderPassSetUpData &passSetUpData) override;

    virtual void Execute(const RenderPassExecuteContext &renderPassExecuteContext) override;

    void SetInputTexSource(const std::string &nmae);

    void SetBloomStage(EBloomStage staga);

  private:
    // void SetGlobalData(const RenderPassExecuteContext &executeContext);
    // std::vector<RenderItem> BuildRenderItem(const RenderPassExecuteContext &executeContex);
    // bool BuildRenderItemMeshData(const Render::StaticMeshRenderCommnad &command, Render::RenderItem &renderItem);
    // void BuildRenderItemBufferGpuResources(const Render::StaticMeshRenderCommnad &command,
    //                                        std::vector<BindingGpuResource> &bindingGpuResourceVector);
    Render::MaterialID mGpuMaterialID;

    Render::FrameContext mPassData;

    std::string mInputTexName;
    std::string mOutputTexName;

    BloomConstantData mConstantData;

    EBloomStage mStage = EBloomStage::eHorizontal;
};
} // namespace Render
