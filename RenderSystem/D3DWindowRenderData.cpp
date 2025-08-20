#include "pch.h"

#include "RenderSystem/D3DWindowRenderData.h"
#include <CoreDevice/D3DCoreDevice.h>
#include <D3DGpuResourceManager/D3DGpuResourceManager.h>
#include <D3DGpuResourceManager/D3DGpuTexture.h>
#include <d3dx12.h>
namespace D3DRender
{

D3DWindowRenderData::D3DWindowRenderData(D3DWindowRenderManager *windowRenderManager,
                                         Microsoft::WRL::ComPtr<ID3D12Device> device,
                                         Microsoft::WRL::ComPtr<IDXGIFactory> factory,
                                         Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue,
                                         const Render::CreationRenderChannelInfo &creationInfo)
    : mWindowRenderManager(windowRenderManager), mDevice(device), mFactory(factory), mCommandQueue(commandQueue),
      mWindowHandle(creationInfo.mWindowHandle), mBackBufferForamt(DXGI_FORMAT_R8G8B8A8_UNORM),
      mCurrentBackBufferIndex(0), mCurrentFenceValue(0)
{
    mGpuResourceManager = static_cast<D3DGRM::D3DGpuResourceManager *>(GRM::IGpuResourceManager::GetInstance());
    HRESULT result = mDevice->CreateFence(mCurrentFenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFence));

    Core::D3DCoreDevice *d3dCoreDevice = static_cast<Core::D3DCoreDevice *>(Core::CoreDevice::GetInstance());
    mDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&mCommandAllocator));
    mDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mCommandAllocator.Get(), nullptr,
                               IID_PPV_ARGS(&mCommandList));

    CreateSwapChain(creationInfo);
    // CreateDepthStencilBuffer(creationInfo);
    // default back buffer, - 2개
    // default depth stencil buffer -1개
}

D3DWindowRenderData::~D3DWindowRenderData() {}

void D3DWindowRenderData::ResizeWindow()
{
    RECT clientSize;
    GetClientRect(mWindowHandle, &clientSize);

    Core::D3DCoreDevice *coreDevice = static_cast<Core::D3DCoreDevice *>(Core::CoreDevice::GetInstance());
    coreDevice->FlushCommandQueue();
    ResizeBackBuffer(clientSize.right, clientSize.bottom);
    ResizeDepthStencilBuffer(clientSize.right, clientSize.bottom);

    mCurrentBackBufferIndex = 0;
}

GRM::GRMPtr D3DWindowRenderData::GetBackBuffer(int index) const
{
    return mBackBuffer[index];
}

GRM::GRMPtr D3DWindowRenderData::GetDepthStencilBuffer() const
{
    return mDepthStencilBuffer;
}

int D3DWindowRenderData::GetCurrentBackBufferIndex() const
{
    return mCurrentBackBufferIndex;
}

void D3DWindowRenderData::IncrementBackBufferIndex()
{
    mCurrentBackBufferIndex = (mCurrentBackBufferIndex + 1) % 2;
}

Microsoft::WRL::ComPtr<IDXGISwapChain> D3DWindowRenderData::GetSwapChain() const
{
    return mSwapChain;
}

Microsoft::WRL::ComPtr<ID3D12Fence> D3DWindowRenderData::GetFence() const
{
    return mFence;
}

size_t D3DWindowRenderData::GetCurrentFenceValue() const
{
    return mCurrentFenceValue;
}

void D3DWindowRenderData::IncrementFenceValue()
{
    mCurrentFenceValue++;
}

HANDLE D3DWindowRenderData::GetFenceEventHandle()
{
    return mFenceEventHandle;
}

void D3DWindowRenderData::CreateSwapChain(const Render::CreationRenderChannelInfo &creationInfo)
{

    HWND windowHandle = creationInfo.mWindowHandle;
    RECT windowSize; // 창의 클라이언트 크기
    GetClientRect(windowHandle, &windowSize);

    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    swapChainDesc.BufferDesc.Width = windowSize.right;
    swapChainDesc.BufferDesc.Height = windowSize.bottom;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 60;
    swapChainDesc.BufferDesc.RefreshRate.Numerator = 1;
    swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = 2;
    swapChainDesc.OutputWindow = creationInfo.mWindowHandle;
    swapChainDesc.Windowed = true;
    swapChainDesc.SwapEffect =
        DXGI_SWAP_EFFECT_FLIP_DISCARD; // direct12에서는 flip만사용가능. 그리고 후면버퍼에서 멀티샘플링불가능
    swapChainDesc.Flags = 0;

    HRESULT hresult = mFactory->CreateSwapChain(mCommandQueue.Get(), &swapChainDesc, mSwapChain.GetAddressOf());

    Microsoft::WRL::ComPtr<ID3D12Resource> backBufferResource[2];
    mSwapChain->GetBuffer(0, IID_PPV_ARGS(backBufferResource[0].GetAddressOf()));
    mSwapChain->GetBuffer(1, IID_PPV_ARGS(backBufferResource[1].GetAddressOf()));

    /*	mBackBuffer[0]
     * =static_cast<D3DGRM::D3DGpuTexture*>(mGpuResourceManager->RegisterSwapChainBackBuffer(backBufferResource[0].Get()));*/

    // mBackBuffer[1] =
    // static_cast<D3DGRM::D3DGpuTexture*>(mGpuResourceManager->RegisterSwapChainBackBuffer(backBufferResource[1].Get()));

    mBackBuffer[0] = mGpuResourceManager->RegisterSwapChainBackBuffer(backBufferResource[0].Get());
    mBackBuffer[1] = mGpuResourceManager->RegisterSwapChainBackBuffer(backBufferResource[1].Get());

    D3DGRM::D3DGpuTexture *d3dBackBuffer0 = static_cast<D3DGRM::D3DGpuTexture *>(mBackBuffer[0].getResource());
    D3DGRM::D3DGpuTexture *d3dBackBuffer1 = static_cast<D3DGRM::D3DGpuTexture *>(mBackBuffer[1].getResource());

    d3dBackBuffer0->SetResourceState(D3D12_RESOURCE_STATE_PRESENT);
    d3dBackBuffer1->SetResourceState(D3D12_RESOURCE_STATE_PRESENT);

    CreateDepthStencilBuffer(creationInfo);
}
void D3DWindowRenderData::ResizeBackBuffer(UINT clientWidth, UINT clientHeight)
{

    for (size_t bufferIndex = 0; bufferIndex < 2; ++bufferIndex)
    {
        mGpuResourceManager->ReleaseSwapChainBackBuffer(mBackBuffer[bufferIndex]);
    }

    HRESULT reulst = mSwapChain->ResizeBuffers(2, clientWidth, clientHeight, mBackBufferForamt,
                                               DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);

    Microsoft::WRL::ComPtr<ID3D12Resource> backBufferResource[2];
    mSwapChain->GetBuffer(0, IID_PPV_ARGS(backBufferResource[0].GetAddressOf()));
    mSwapChain->GetBuffer(1, IID_PPV_ARGS(backBufferResource[1].GetAddressOf()));

    // 사이즈가 조절된 후면버퍼에대한 처리
    mGpuResourceManager->ResizeSwapChainBackBuffer(mBackBuffer[0], backBufferResource[0].Get());
    mGpuResourceManager->ResizeSwapChainBackBuffer(mBackBuffer[1], backBufferResource[1].Get());

    D3DGRM::D3DGpuTexture *d3dBackBuffer0 = static_cast<D3DGRM::D3DGpuTexture *>(mBackBuffer[0].getResource());
    D3DGRM::D3DGpuTexture *d3dBackBuffer1 = static_cast<D3DGRM::D3DGpuTexture *>(mBackBuffer[1].getResource());

    // CD3DX12_RESOURCE_BARRIER backBufferTransition[2];
    // backBufferTransition[0] = CD3DX12_RESOURCE_BARRIER::Transition(d3dBackBuffer0->GetResource().Get(),
    // D3D12_RESOURCE_STATE_COMMON, 	D3D12_RESOURCE_STATE_PRESENT); backBufferTransition[1] =
    // CD3DX12_RESOURCE_BARRIER::Transition(d3dBackBuffer0->GetResource().Get(), D3D12_RESOURCE_STATE_COMMON,
    //	D3D12_RESOURCE_STATE_PRESENT);

    // mCommandList->ResourceBarrier(2, backBufferTransition);

    d3dBackBuffer0->SetResourceState(D3D12_RESOURCE_STATE_PRESENT);
    d3dBackBuffer1->SetResourceState(D3D12_RESOURCE_STATE_PRESENT);
}
void D3DWindowRenderData::ResizeDepthStencilBuffer(UINT clientWidth, UINT clientHeight)
{

    mDepthStencilDesc.mScratchImage.mMetadata.mWidth = clientWidth;
    mDepthStencilDesc.mScratchImage.mMetadata.mHeight = clientHeight;
    if (mDepthStencilBuffer.getResource() != nullptr)
    {
        mGpuResourceManager->ChangeTextureData(mDepthStencilBuffer, mDepthStencilDesc);
        mGpuResourceManager->TransitionResourceState(mDepthStencilBuffer.getResource(),
                                                     D3D12_RESOURCE_STATE_DEPTH_WRITE);
    }
}
void D3DWindowRenderData::CreateDepthStencilBuffer(const Render::CreationRenderChannelInfo &creationInfo)
{

    HWND windowHandle = creationInfo.mWindowHandle;
    RECT windowSize; // 창의 클라이언트 크기
    GetClientRect(windowHandle, &windowSize);
    GRM::TextureDesc textureDesc;
    GRM::TexMetaData &texMetaData = textureDesc.mScratchImage.mMetadata;

    texMetaData.mWidth = windowSize.right;
    texMetaData.mHeight = windowSize.bottom;
    texMetaData.mDepth = 1;
    texMetaData.mArraySize = 1;
    texMetaData.mMipLevels = 1;
    texMetaData.mMiscFlags = 0;
    texMetaData.mMiscFlags2 = 0;
    texMetaData.mFormat = GRM::ETextureFormat::eD24_UNORM_S8_UINT;

    textureDesc.mScratchImage.mMetadata.mDimension = GRM::ETextureType::eTexture2D;
    textureDesc.mTextureUsage = GRM::ETextureUsage::eDepthStencil;
    textureDesc.hasOptimizedClearValue = true;
    textureDesc.mOptimizedClearValue.mOptimizedDepthStencilValue.mOptimizedClearDepth = creationInfo.mMaxZ;
    textureDesc.mOptimizedClearValue.mOptimizedDepthStencilValue.mOptimizedClearStencil = 0;
    textureDesc.mScratchImage.mSize = 0;
    textureDesc.mScratchImage.mimagesNum = 0;

    mDepthStencilDesc = textureDesc;

    mDepthStencilBuffer = mGpuResourceManager->CreateTexture(mDepthStencilDesc);
    mGpuResourceManager->TransitionResourceState(mDepthStencilBuffer.getResource(), D3D12_RESOURCE_STATE_DEPTH_WRITE);
}
} // namespace D3DRender