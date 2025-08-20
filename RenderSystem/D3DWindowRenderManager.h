#pragma once

#include "RenderSystem/RenderType.h"

#include <dxgi.h>

#include <assert.h>
#include <iostream>
#include <memory>
#include <unordered_map>

#include <d3d12.h>

#include <wrl.h>
#ifdef D3DX
#include "RenderSystem/RenderSystemDllMacro.h"
namespace D3DRender
{
class D3DWindowRenderData;

// window별 공통의데이터인 WindowRenderData를 관리하는 Manager클래스
class RENDER_SYSTEM_API D3DWindowRenderManager
{
  public:
    // 반드시 어느 한곳에서 생성후 호출할것
    static D3DWindowRenderManager *GetInstance();

    D3DWindowRenderManager(Microsoft::WRL::ComPtr<ID3D12Device> device, Microsoft::WRL::ComPtr<IDXGIFactory> factory,
                           Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue);
    ~D3DWindowRenderManager();

    bool RegisterWindow(const Render::CreationRenderChannelInfo &creationChannelInfo);

    std::shared_ptr<D3DWindowRenderData> GetWindowRenderData(HWND hwnd) const;

    void WindowResize(HWND hwnd);

  private:
    static D3DWindowRenderManager *mInstance;

    Microsoft::WRL::ComPtr<ID3D12Device> mDevice;
    Microsoft::WRL::ComPtr<IDXGIFactory> mFactory;
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> mCommandQueue;

    std::unordered_map<HWND, std::shared_ptr<D3DWindowRenderData>> mWindowRenderDataTable;
};

} // namespace D3DRender

#endif
