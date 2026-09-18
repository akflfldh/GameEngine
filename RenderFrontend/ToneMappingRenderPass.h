#pragma once

#include <RenderFrontend/RenderPass.h>
namespace Render
{
struct ToneMappingConstant
{
    float gExposure = 1.0f;
};

class ToneMappingRenderPass : public IRenderPass
{
  public:
    ToneMappingRenderPass();
    virtual ~ToneMappingRenderPass();

    void SetInputSource(const std::string &name);
    void SetInputSourceTwo(const std::string &name);

    virtual void AddToGraph(RenderPassGraph &renderPassGraph, const RenderPassSetUpData &passSetUpData) override;

    virtual void Execute(const RenderPassExecuteContext &) override;

  private:
    void SetGlobalData(const RenderPassExecuteContext &executeContext, FrameContext &oFrameContext);

    std::string mInputSource;
    std::string mInputSourceTwo;
    Render::MaterialID mPassGpuMaterialID;
};

} // namespace Render