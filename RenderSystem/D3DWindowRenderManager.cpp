#include "pch.h"

#include "RenderSystem/D3DWindowRenderData.h"
#include "RenderSystem/D3DWindowRenderManager.h"

D3DRender::D3DWindowRenderManager *D3DRender::D3DWindowRenderManager::mInstance = nullptr;

D3DRender::D3DWindowRenderManager::D3DWindowRenderManager(Microsoft::WRL::ComPtr<ID3D12Device> device,
                                                          Microsoft::WRL::ComPtr<IDXGIFactory> factory,
                                                          Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue)
    : mDevice(device), mFactory(factory), mCommandQueue(commandQueue)
{
    assert(mInstance == nullptr);
    std::cerr << "D3DWindowRenderManager 초과생성\n";
    mInstance = this;
}

D3DRender::D3DWindowRenderManager::~D3DWindowRenderManager() {}

D3DRender::D3DWindowRenderManager *D3DRender::D3DWindowRenderManager::GetInstance()
{
    assert(mInstance != nullptr);

    return mInstance;
}

bool D3DRender::D3DWindowRenderManager::RegisterWindow(const Render::CreationRenderChannelInfo &creationChannelInfo)
{
    HWND windowHandle = (HWND)creationChannelInfo.mWindowHandle;
    auto it = mWindowRenderDataTable.find(windowHandle);

    if (it != mWindowRenderDataTable.end())
    {

        // 이미있다는 로그
        return true;
    }

    mWindowRenderDataTable[windowHandle] =
        std::make_shared<D3DWindowRenderData>(this, mDevice, mFactory, mCommandQueue, creationChannelInfo);

    return true;
}

std::shared_ptr<D3DRender::D3DWindowRenderData> D3DRender::D3DWindowRenderManager::GetWindowRenderData(HWND hwnd) const
{
    auto it = mWindowRenderDataTable.find(hwnd);

    return it != mWindowRenderDataTable.cend() ? it->second : nullptr;
}

int D3DRender::D3DWindowRenderManager::WindowResize(HWND hwnd)
{

    std::shared_ptr<D3DRender::D3DWindowRenderData> windowData = GetWindowRenderData(hwnd);
    if (windowData == nullptr)
        return 0;

    return windowData->ResizeWindow();
}

GRM::GRMPtr D3DRender::D3DWindowRenderManager::GetCurrentSwapchainBackBuffer(void *windowHandle)
{
    HWND hwnd = (HWND)windowHandle;

    auto windowRenderData = GetWindowRenderData(hwnd);
    return windowRenderData->GetCurrentBackBuffer();
}

GRM::GRMPtr D3DRender::D3DWindowRenderManager::GetSwapchainBackBuffer(void *windowHandle, int index)
{

    HWND hwnd = (HWND)windowHandle;

    auto windowRenderData = GetWindowRenderData(hwnd);
    return windowRenderData->GetBackBuffer(index);
}

GRM::GRMPtr D3DRender::D3DWindowRenderManager::GetDepthStencilBuffer(void *windowHandle)
{

    HWND hwnd = (HWND)windowHandle;

    auto windowRenderData = GetWindowRenderData(hwnd);
    return windowRenderData->GetDepthStencilBuffer();
}
