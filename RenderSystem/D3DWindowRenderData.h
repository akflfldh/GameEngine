#pragma once

#include "RenderSystem/RenderType.h"

#ifdef D3DX

#include "RenderSystem/RenderSystemDllMacro.h"
#include <CommonHeader/GpuTypes.h>
#include <D3DGpuResourceManager/GRMPtr.h>
#include <d3d12.h>
#include <dxgi.h>
#include <dxgi1_4.h>
#include <wrl.h>
namespace GRM
{
class IGpuResourceManager;
class GRMPtr;
} // namespace GRM
namespace D3DGRM
{
class D3DGpuTexture;
class D3DGpuResourceManager;
} // namespace D3DGRM

namespace D3DRender
{

class D3DWindowRenderManager;
class RENDER_SYSTEM_API D3DWindowRenderData
{
  public:
    D3DWindowRenderData(D3DWindowRenderManager *windowRenderManager, Microsoft::WRL::ComPtr<ID3D12Device> device,
                        Microsoft::WRL::ComPtr<IDXGIFactory> factory,
                        Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue,
                        const Render::CreationRenderChannelInfo &creationInfo);
    ~D3DWindowRenderData();

    int ResizeWindow();

    // 스왑체인,후면/깊이버퍼
    GRM::GRMPtr GetBackBuffer(int index) const; // index : 0 ,1
    GRM::GRMPtr GetDepthStencilBuffer() const;

    GRM::GRMPtr GetCurrentBackBuffer() const;

    int GetCurrentBackBufferIndex() const;
    void IncrementBackBufferIndex();
    Microsoft::WRL::ComPtr<IDXGISwapChain> GetSwapChain() const;

    // 펜스
    Microsoft::WRL::ComPtr<ID3D12Fence> GetFence() const;
    size_t GetCurrentFenceValue() const;
    void IncrementFenceValue();
    HANDLE GetFenceEventHandle();

  private:
    void CreateSwapChain(const Render::CreationRenderChannelInfo &creationInfo);
    // backbuffer index return
    int ResizeBackBuffer(UINT clientWidth, UINT clientHeight);
    void ResizeDepthStencilBuffer(UINT clientWidth, UINT clientHeight);
    void CreateDepthStencilBuffer(const Render::CreationRenderChannelInfo &creationInfo);

  private:
    D3DWindowRenderManager *mWindowRenderManager;

    Microsoft::WRL::ComPtr<ID3D12Device> mDevice;
    Microsoft::WRL::ComPtr<IDXGIFactory> mFactory;
    Microsoft::WRL::ComPtr<IDXGISwapChain3> mSwapChain;
    Microsoft::WRL::ComPtr<ID3D12Fence> mFence;
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> mCommandQueue;
    // Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mCommandAllocator;
    // Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList;
    size_t mCurrentFenceValue;

    // Microsoft::WRL::ComPtr<ID3D12Resource> mBackBuffer[2];

    HWND mWindowHandle;
    DXGI_FORMAT mBackBufferForamt;

    D3DGRM::D3DGpuResourceManager *mGpuResourceManager;

    GRM::GRMPtr mBackBuffer[2];
    GRM::GRMPtr mDepthStencilBuffer;

    GRM::TextureDesc mDepthStencilDesc;

    int mCurrentBackBufferIndex;
    HANDLE mFenceEventHandle;
};

} // namespace D3DRender

#endif