#pragma once

#include <RenderFrontend/RenderPass.h>
namespace Render
{
struct GrayScaleConstantPass
{
    float mGrayScale = 1.0f;
    // 기본적으로 UI들의 좌표는 가상의 스크린공간좌표이기에
    //  일단 view는 그냥 단위행렬이거나, 가상카메라가 전체가 움직이거나 스케일될때만
    //  Proj은 X,Y를 NDC공간으로
};

class GrayScaleRenderPass : public IRenderPass
{
  public:
    GrayScaleRenderPass();
    virtual ~GrayScaleRenderPass();

    void SetInputSource(const std::string &name);

    virtual void AddToGraph(RenderPassGraph &renderPassGraph, const RenderPassSetUpData &passSetUpData) override;

    virtual void Execute(const RenderPassExecuteContext &) override;

  private:
    void SetGlobalData(const Core::GlobalFrameData &globalFrameData, FrameContext &oFrameContext);

    std::string mInputSource;
    Render::MaterialID mPassGpuMaterialID;
};
} // namespace Render
