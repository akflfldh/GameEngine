#pragma once

#include <RenderFrontend/RenderFrontendType.h>
#include <RenderSystem/RenderType.h>
#include <string>
#include <vector>
namespace Core
{
struct GlobalFrameData;
}

namespace GRM
{
class GpuSamplerSystem;
class GpuBufferContextSystem;
class IGpuResourceManager;
} // namespace GRM

namespace Render
{
class IRenderSystem;
class RenderPassGraph;
class AssetResolver;
class IMaterialManager; // gpuMaterialManager
class RenderUploadManager;
struct RenderPassSetUpData
{
    uint32_t mWindowWidth;
    uint32_t mWindowHeight;

    float mBackBufferClearColor[4];
};

class IRenderPass
{
  public:
    IRenderPass();
    virtual ~IRenderPass() = 0;

    const std::string &GetName() const;

    void SetOutputTarget(const std::string &name);
    void SetOutputDepthStencil(const std::string &name);

    virtual void AddToGraph(RenderPassGraph &renderPassGraph, const RenderPassSetUpData &passSetUpData) = 0;

    virtual void Execute(const RenderPassExecuteContext &) = 0;

    // void SetGlobalData(const Core::GlobalFrameData &globalFrameData)
    uint32_t GetBufferID() const;

    Render::RECT GetViewport() const;

    bool GetUseDefaultViewport() const;

    void SetClearRenderTarget(bool clear);

    void SetPassName(const std::string &name);

  protected:
    void SetBufferID(uint32_t bufferiD);

    // 반드시 패스들이 호출해서 설정해야한다.
    void SetViewport(const Render::RECT &viewport);

    // draw직전 전역 데이터 설정(ex passbuffer binding,viewport)
    // virtual void SetUpPassState(IRenderSystem *renderSystem) = 0;
    AssetResolver *mAssetResolver;
    IMaterialManager *mGpuMaterialManager;
    GRM::GpuSamplerSystem *mGpuSamplerSystem;
    GRM::GpuBufferContextSystem *mGpuBufferContextSystem;
    GRM::IGpuResourceManager *mGpuResourceManager;

    std::string mOutputTargetName;
    std::string mOutputDepthStencilName;
    bool mUseDefaultViewport = true;

    bool mClearRenderTarget = false;

    RenderUploadManager *mRenderUploadManager;

  private:
    std::string mName;
    uint32_t mBufferID; // PASS 상수버퍼 ID

    Render::RECT mViewport;
};

} // namespace Render