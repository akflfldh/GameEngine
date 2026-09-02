#pragma once

#include "RenderSystem/RenderType.h"

#include <dxgi.h>

#include <RenderSystem/IWindowRenderManager.h>
#include <assert.h>
#include <d3d12.h>
#include <iostream>
#include <memory>
#include <unordered_map>

#include <wrl.h>
#ifdef D3DX
#include "RenderSystem/RenderSystemDllMacro.h"
namespace D3DRender
{
class D3DWindowRenderData;

// window별 공통의데이터인 WindowRenderData를 관리하는 Manager클래스
class RENDER_SYSTEM_API D3DWindowRenderManager : public Render::IWindowRenderManager
{
  public:
    // 반드시 어느 한곳에서 생성후 호출할것
    static D3DWindowRenderManager *GetInstance();

    D3DWindowRenderManager(Microsoft::WRL::ComPtr<ID3D12Device> device, Microsoft::WRL::ComPtr<IDXGIFactory> factory,
                           Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue);
    ~D3DWindowRenderManager();

    bool RegisterWindow(const Render::CreationRenderChannelInfo &creationChannelInfo);

    std::shared_ptr<D3DWindowRenderData> GetWindowRenderData(HWND hwnd) const;

    int WindowResize(HWND hwnd);

    virtual GRM::GRMPtr GetCurrentSwapchainBackBuffer(void *windowHandle) override;
    virtual GRM::GRMPtr GetSwapchainBackBuffer(void *windowHandle, int index) override;
    virtual GRM::GRMPtr GetDepthStencilBuffer(void *windowHandle) override;

  private:
    static D3DWindowRenderManager *mInstance;

    Microsoft::WRL::ComPtr<ID3D12Device> mDevice;
    Microsoft::WRL::ComPtr<IDXGIFactory> mFactory;
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> mCommandQueue;

    std::unordered_map<HWND, std::shared_ptr<D3DWindowRenderData>> mWindowRenderDataTable;

    // TODOPO IWindowRenderMangaer를 만들자
    // 인터페이스 GRMPtr GetCurrentSwapChainBackbuffer() ;
};

} // namespace D3DRender

#endif
