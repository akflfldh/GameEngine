#pragma once

#include <CoreMath/CoreMath.h>
#include <RenderFrontend/RenderPass.h>
namespace Render
{

struct ConstantPass
{
    // glm::mat4 mViewProj;
    CoreMath::Matrix4X4 mViewProj;
    // 기본적으로 UI들의 좌표는 가상의 스크린공간좌표이기에
    //  일단 view는 그냥 단위행렬이거나, 가상카메라가 전체가 움직이거나 스케일될때만
    //  Proj은 X,Y를 NDC공간으로
    float mScreenWidth;
    float mScreenHeight;
};

struct ProxyContext
{
    CoreAsset::AssetID mMat;
    SRECT mScissorRect;
    bool bUseScissorRect = false;

    bool operator==(const ProxyContext &proxy) const
    {
        if (mMat != proxy.mMat)
            return false;
        if (mScissorRect != proxy.mScissorRect)
            return false;

        if (bUseScissorRect != proxy.bUseScissorRect)
            return false;

        return true;
    }
};

class RenderPassUI : public IRenderPass
{
  public:
    RenderPassUI();

    virtual void AddToGraph(RenderPassGraph &renderPassGraph, const RenderPassSetUpData &passSetUpData) override;

    virtual void Execute(const RenderPassExecuteContext &) override;

  private:
    void SetGlobalData(const Core::GlobalFrameData &globalFrameData, FrameContext &oFrameContext);
    std::vector<RenderItem> BuildRenderItem(const RenderPassExecuteContext &renderPassExecuteContext);

    SRECT ConvertWorldToScreenRect(const SRECT &rect, const Core::GlobalFrameData &globalFrameData);
    void SetUpRenderItemShaderResource(const RenderPassExecuteContext &renderPassExecuteContext, RenderItem &renderItem,
                                       const UIRenderCommand &renderCommand);

    void BuildRenderItemTexGpuResources(const MaterialRenderSnapshot &materialRenderSnapshot,

                                        std::vector<BindingGpuResource> &bindingGpuResourceVector);

    Render::ScissorRect mGlobalScissorRect;
};

} // namespace Render

namespace std
{

template <> struct hash<Render::ProxyContext>
{
    size_t operator()(const Render::ProxyContext &proxyContext) const
    {
        return static_cast<const uint32_t>(proxyContext.mMat + proxyContext.mScissorRect.mLeft +
                                           proxyContext.mScissorRect.mRight + proxyContext.mScissorRect.mTop +
                                           proxyContext.mScissorRect.mBottom);
    }
};

} // namespace std