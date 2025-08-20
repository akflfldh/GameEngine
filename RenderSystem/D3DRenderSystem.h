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

    // return channel id
    virtual Render::RenderChannelID RegisterRenderChannel(
        const Render::CreationRenderChannelInfo &creationChannelInfo) override;

    // 가장 먼저호출할것
    virtual void StartWindow(Render::WindowHandle windowHandle) override;

    virtual void BeginFrame(Render::RenderChannelID channelID, const Render::FrameContext &frameContext) override;

    // renderItem제출 (여러번호출가능 EndFrame전까지)
    virtual void SubmitRenderItems(Render::RenderChannelID channelID,
                                   const std::vector<Render::RenderItem> &renderItemVector) override;

    // 최종실제 draw
    virtual void EndFrame(Render::RenderChannelID channelID) override;

    virtual void PresentWindow(Render::WindowHandle windowHandle) override;

    virtual void WindowResize(Render::WindowHandle windowHandle) override;

    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> GetCommandList() const;
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> GetCommandAllocator() const;

  private:
    void FlushCommandQueue();

    Render::InternalRenderItem ConvertInteranlRenderItem(const Render::RenderItem &renderItem) const;

  private:
    std::unordered_map<Render::RenderChannelID, std::unique_ptr<D3DRenderChannelSystem>> mRenderChannelSystemTable;
    Microsoft::WRL::ComPtr<IDXGIFactory> mFactory;
    Microsoft::WRL::ComPtr<ID3D12Device> mDevice;
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> mCommandQueue;
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mCommandAllocator;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList;
    std::unique_ptr<D3DWindowRenderManager> mWindowRenderManager;
    std::unique_ptr<D3DRenderPassManager> mRenderPassManager;
    std::shared_ptr<D3DMaterialManager> mMaterialManager;

    Render::RenderChannelID mNextChannelID;
    std::unordered_map<HWND, std::vector<unsigned long long>> mChannelWindowMapTable;

    D3DGRM::D3DGpuResourceManager *mGpuResourceManager;
};

} // namespace D3DRender

#endif