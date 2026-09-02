#pragma once

#include "RenderSystem/IRenderSystem.h"
#ifdef D3DX

#include "RenderSystem/D3DWindowRenderManager.h"
#include "RenderSystem/RenderSystemDllMacro.h"
#include <Windows.h>
#include <d3d12.h>
#include <dxgi.h>
#include <memory>
#include <unordered_map>
#include <wrl.h>

namespace D3DGRM
{
class D3DGpuResourceManager;
}

namespace D3DRender
{

class D3DRenderChannelSystem;
class D3DWindowRenderManager;
class D3DRenderPassManager;
class D3DMaterialManager;
class RENDER_SYSTEM_API D3DRenderSystem : public Render::IRenderSystem
{
  public:
    D3DRenderSystem(Microsoft::WRL::ComPtr<IDXGIFactory> factory, Microsoft::WRL::ComPtr<ID3D12Device> device,
                    Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue,
                    D3DGRM::D3DGpuResourceManager *gpuResourceManager);
    ~D3DRenderSystem();

    D3DRenderSystem(const D3DRenderSystem &) = delete;
    D3DRenderSystem &operator=(const D3DRenderSystem &) = delete;

    // return channel id
    virtual void RegisterWindow(const Render::CreationRenderChannelInfo &creationChannelInfo) override;

    // 가장 먼저호출할것

    virtual void BeginFrame(Render::RenderChannelID channelID, const Render::FrameContext &frameContext) override;

    // renderItem제출 (여러번호출가능 EndFrame전까지)
    virtual void SubmitRenderItems(Render::RenderChannelID channelID,
                                   const std::vector<Render::RenderItem> &renderItemVector) override;

    // 최종실제 draw
    virtual void EndFrame(Render::RenderChannelID channelID) override;

    virtual void PresentWindow(Core::CommandContext *commandContext, Render::WindowHandle windowHandle,
                               int backBufferIndex) override;

    virtual int WindowResize(Render::WindowHandle windowHandle) override;

    // Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> GetCommandList() const;
    // Microsoft::WRL::ComPtr<ID3D12CommandAllocator> GetCommandAllocator() const;

#pragma region Refactoring

    virtual void StartFrame(Render::WindowHandle windowHandle) override;
    virtual void StartWindow(Core::CommandContext *commandContext, Render::WindowHandle windowHandle,
                             int backBufferIndex) override;

    //  virtual void StartView(Render::RenderChannelID channelID) override; // 이거 사용안함
    //    virtual void EndView(Render::RenderChannelID channelID) override;   // 이거 사용안함

    virtual void SetUpPassData(Core::CommandContext *commandContext, const Render::FrameContext &passData) override;
    virtual void SetUpStencilValue(Core::CommandContext *commandContext, uint32_t value);
    virtual void Draw(Core::CommandContext *commandContext,
                      const std::vector<Render::RenderItem> &renderItemList) override;

    virtual void DrawFullScreen(Core::CommandContext *commandContext, const Render::RenderItem &renderItem) override;

    virtual void ExecuteResourceBarriers(Core::CommandContext *commandContext,
                                         std::vector<Render::ResourceBarrier> &resourceBarriers) override;
    virtual void ExecuteResourceBarrier(const Render::ResourceBarrier &resourceBarrier) override;

    virtual void ClearRenderTarget(Core::CommandContext *commandContext, GRM::IGpuResource *renderTarget,
                                   float color[4], Render::RECT rect) override;

    // TODO depth value, Stencil Value 두개를 처리해야한다.
    virtual void ClearDepthStencil(Core::CommandContext *commandContext, GRM::IGpuResource *renderTarget, float value,
                                   Render::RECT rect) override;
    virtual void SetRenderTarget(Core::CommandContext *commandContext, GRM::IGpuResource *rendertarget,
                                 GRM::IGpuResource *depthStenci) override;

  private:
    // pso, rootsignature 비교후 바인딩
    void BindPSOIfNeeded(ID3D12GraphicsCommandList *commandList, const Render::RenderItem *beforeRenderItem,
                         const Render::RenderItem *currRenderItem, MaterialItem *beforeMaterialItem,
                         MaterialItem *currMaterialItem);

    void BindScissorRectIfNeeded(ID3D12GraphicsCommandList *commandList, const Render::RenderItem *beforeRenderItem,
                                 const Render::RenderItem *currRenderItem);

    void BindMeshBufferIfNeeded(ID3D12GraphicsCommandList *commandList, const Render::RenderItem *beforeRenderItem,
                                const Render::RenderItem *currRenderItem);

    // 무조건 새롭게 바인딩 수행
    void BindShaderResources(ID3D12GraphicsCommandList *commandList, const Render::RenderItem *currRenderItem);

    void DrawRenderItem(ID3D12GraphicsCommandList *commandList, const Render::RenderItem *currRenderItem);

    void BindGlobalShaderResource(ID3D12GraphicsCommandList *commandList);

    void SetUpRenderTarget(ID3D12GraphicsCommandList *commandList);

    // D3D12_RESOURCE_STATES ConvertToD3DResourceState(EResourceState resourceState);

    D3D12_RECT ConvertToD3DRect(const Render::RECT &rect);

#pragma endregion

  private:
    void FlushCommandQueue();

    Render::InternalRenderItem ConvertInteranlRenderItem(const Render::RenderItem &renderItem) const;

  private:
    std::unordered_map<Render::RenderChannelID, std::unique_ptr<D3DRenderChannelSystem>> mRenderChannelSystemTable;
    Microsoft::WRL::ComPtr<IDXGIFactory> mFactory;
    Microsoft::WRL::ComPtr<ID3D12Device> mDevice;
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> mCommandQueue;
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mCommandAllocator[3];
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList[3];

    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mCurrentCommandAllocator;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCurrentCommandList;

    std::unique_ptr<D3DWindowRenderManager> mWindowRenderManager;
    std::unique_ptr<D3DRenderPassManager> mRenderPassManager;
    std::shared_ptr<D3DMaterialManager> mMaterialManager;

    Render::RenderChannelID mNextChannelID;
    std::unordered_map<HWND, std::vector<unsigned long long>> mChannelWindowMapTable;

    D3DGRM::D3DGpuResourceManager *mGpuResourceManager;

    Render::WindowHandle mCurrWindowHandle;
    Render::FrameContext mCurrPassFrameContext;
};

} // namespace D3DRender

#endif