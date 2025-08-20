#include "pch.h"

#define USE_PIX
#include "RenderSystem/D3DMaterialManager.h"
#include "RenderSystem/D3DRenderChannelSystem.h"
#include "RenderSystem/D3DRenderPassManager.h"
#include "RenderSystem/D3DRenderSystem.h"
#include "RenderSystem/D3DWindowRenderData.h"
#include "RenderSystem/D3DWindowRenderManager.h"
#include "RenderSystem/RenderSystemInternalHelper.h"
#include <CoreDevice/CoreDevice.h>
#include <CoreDevice/D3DCoreDevice.h>
#include <D3DGpuResourceManager/D3DGpuResourceManager.h>
#include <D3DGpuResourceManager/D3DGpuTexture.h>
#include <Pix/WinPixEventRuntime/pix3.h>
#include <algorithm>
#include <d3dx12.h>

D3DRender::D3DRenderSystem::D3DRenderSystem(Microsoft::WRL::ComPtr<IDXGIFactory> factory,
                                            Microsoft::WRL::ComPtr<ID3D12Device> device,
                                            Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue,
                                            D3DGRM::D3DGpuResourceManager *gpuResourceManager)
    : mFactory(factory), mDevice(device), mCommandQueue(commandQueue), mNextChannelID(0),
      mGpuResourceManager(gpuResourceManager)
{
    mWindowRenderManager = std::make_unique<D3DWindowRenderManager>(mDevice, mFactory, commandQueue);
    mMaterialManager = std::make_shared<D3DMaterialManager>(device, gpuResourceManager);
    Render::IMaterialManager::SetMaterialManagerImpl(mMaterialManager.get());

    mRenderPassManager = std::make_unique<D3DRenderPassManager>(device, gpuResourceManager, mMaterialManager);

    HRESULT result = S_OK;
    result = mDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&mCommandAllocator));
    result = mDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mCommandAllocator.Get(), nullptr,
                                        IID_PPV_ARGS(&mCommandList));

    mCommandList->Close();
}

D3DRender::D3DRenderSystem::~D3DRenderSystem() {}

Render::RenderChannelID D3DRender::D3DRenderSystem::RegisterRenderChannel(
    const Render::CreationRenderChannelInfo &creationChannelInfo)
{

    bool result = mWindowRenderManager->RegisterWindow(creationChannelInfo);

    std::shared_ptr<D3DWindowRenderData> windowRenderData =
        mWindowRenderManager->GetWindowRenderData(creationChannelInfo.mWindowHandle);

    mRenderChannelSystemTable[mNextChannelID] =
        std::make_unique<D3DRenderChannelSystem>(creationChannelInfo, mDevice, mCommandQueue, mGpuResourceManager,
                                                 mRenderPassManager->CreateDefaultRenderPipeline(windowRenderData));

    mChannelWindowMapTable[creationChannelInfo.mWindowHandle].push_back(mNextChannelID);

    return mNextChannelID++;
}

void D3DRender::D3DRenderSystem::StartWindow(Render::WindowHandle windowHandle)
{

    FlushCommandQueue();

    mCommandAllocator->Reset();
    mCommandList->Reset(mCommandAllocator.Get(), nullptr);

    // window curr back buffer를 렌더타켓상태로 전환
    std::shared_ptr<D3DWindowRenderData> windowRenderData = mWindowRenderManager->GetWindowRenderData(windowHandle);
    int currentBackBufferIndex = windowRenderData->GetCurrentBackBufferIndex();

    D3DGRM::D3DGpuTexture *backBuffer =
        static_cast<D3DGRM::D3DGpuTexture *>(windowRenderData->GetBackBuffer(currentBackBufferIndex).getResource());
    D3DGRM::D3DGpuTexture *depthStencilBuffer =
        static_cast<D3DGRM::D3DGpuTexture *>(windowRenderData->GetDepthStencilBuffer().getResource());
    Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain = windowRenderData->GetSwapChain();
    D3DGRM::D3DDescriptorHandle backBufferDescriptorHandle;
    D3DGRM::D3DDescriptorHandle depthStecnilDescriptorHandle;

    if (depthStencilBuffer->GetDescriptorHandle(D3DGRM::ED3DResourceDescriptorType::eDSV, depthStecnilDescriptorHandle))
    {
        // StartWindow함수로 정보를 받아 설정할수있도록 확장하자
        mCommandList->ClearDepthStencilView(depthStecnilDescriptorHandle.mCpuDescriptorHandle,
                                            D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
    }

    if (backBuffer->GetDescriptorHandle(D3DGRM::ED3DResourceDescriptorType::eRTV, backBufferDescriptorHandle))
    {

        auto backBufferPresentTransition = CD3DX12_RESOURCE_BARRIER::Transition(
            backBuffer->GetResource().Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

        mCommandList->ResourceBarrier(1, &backBufferPresentTransition);

        float color[4] = {1, 0, 0, 0};
        mCommandList->ClearRenderTargetView(backBufferDescriptorHandle.mCpuDescriptorHandle, color, 0, nullptr);
    }

    mCommandList->Close();
    ID3D12CommandList *commandLists[] = {mCommandList.Get()};
    mCommandQueue->ExecuteCommandLists(1, commandLists);
}

void D3DRender::D3DRenderSystem::BeginFrame(Render::RenderChannelID channelID, const Render::FrameContext &frameContext)
{
    auto it = mRenderChannelSystemTable.find(channelID);

    if (it == mRenderChannelSystemTable.end())
        return;

    (*it).second->BeginFrame(frameContext);
}

void D3DRender::D3DRenderSystem::SubmitRenderItems(Render::RenderChannelID channelID,
                                                   const std::vector<Render::RenderItem> &renderItemVector)
{
    auto it = mRenderChannelSystemTable.find(channelID);
    if (it == mRenderChannelSystemTable.end())
        return;

    // 정렬

    std::vector<Render::InternalRenderItem> internalRenderItemVector;
    internalRenderItemVector.reserve(renderItemVector.size());
    for (size_t i = 0; i < renderItemVector.size(); ++i)
    {
        internalRenderItemVector.push_back(ConvertInteranlRenderItem(renderItemVector[i]));
    }

    // 그후 렌더패스들에게는 이렇게 정렬된 렌더ITEM들이제공된다
    std::sort(internalRenderItemVector.begin(), internalRenderItemVector.end());

    (*it).second->SubmitRenderItem(std::move(internalRenderItemVector));
}

void D3DRender::D3DRenderSystem::EndFrame(Render::RenderChannelID channelID)
{
    auto it = mRenderChannelSystemTable.find(channelID);
    if (it == mRenderChannelSystemTable.end())
        return;

    (*it).second->EndFrame();
}

void D3DRender::D3DRenderSystem::PresentWindow(Render::WindowHandle windowHandle)
{
    std::shared_ptr<D3DWindowRenderData> windowRenderData = mWindowRenderManager->GetWindowRenderData(windowHandle);
    int currentBackBufferIndex = windowRenderData->GetCurrentBackBufferIndex();

    D3DGRM::D3DGpuTexture *backBuffer =
        static_cast<D3DGRM::D3DGpuTexture *>(windowRenderData->GetBackBuffer(currentBackBufferIndex).getResource());
    Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain = windowRenderData->GetSwapChain();
    D3DGRM::D3DDescriptorHandle backBufferDescriptorHandle;

    if (backBuffer->GetDescriptorHandle(D3DGRM::ED3DResourceDescriptorType::eRTV, backBufferDescriptorHandle))
    {
        mCommandList->Reset(mCommandAllocator.Get(), nullptr);

        auto backBufferPresentTransition = CD3DX12_RESOURCE_BARRIER::Transition(
            backBuffer->GetResource().Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

        mCommandList->ResourceBarrier(1, &backBufferPresentTransition);

        mCommandList->Close();
        ID3D12CommandList *commandLists[] = {mCommandList.Get()};
        mCommandQueue->ExecuteCommandLists(1, commandLists);

        Core::D3DCoreDevice *d3dCoreDevice = static_cast<Core::D3DCoreDevice *>(Core::D3DCoreDevice::GetInstance());
        d3dCoreDevice->FlushCommandQueue();

        swapChain->Present(1, 0);
        windowRenderData->IncrementBackBufferIndex();
    }
}

void D3DRender::D3DRenderSystem::WindowResize(Render::WindowHandle windowHandle)
{
    mWindowRenderManager->WindowResize(windowHandle);
}

Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> D3DRender::D3DRenderSystem::GetCommandList() const
{
    return mCommandList;
}

Microsoft::WRL::ComPtr<ID3D12CommandAllocator> D3DRender::D3DRenderSystem::GetCommandAllocator() const
{
    return mCommandAllocator;
}

void D3DRender::D3DRenderSystem::FlushCommandQueue()
{

    Core::D3DCoreDevice *d3dCoreDevice = static_cast<Core::D3DCoreDevice *>(Core::CoreDevice::GetInstance());
    d3dCoreDevice->FlushCommandQueue();
}

Render::InternalRenderItem D3DRender::D3DRenderSystem::ConvertInteranlRenderItem(
    const Render::RenderItem &renderItem) const
{

    Render::InternalRenderItem internalRenderItem;
    internalRenderItem.mScissor = renderItem.mScissor;
    internalRenderItem.mMaterialID = renderItem.mMaterialID;
    internalRenderItem.mInstance = renderItem.mInstance;
    internalRenderItem.mMeshItem = renderItem.mMeshItem;
    internalRenderItem.mBindingGpuResourceVector = renderItem.mBindingGpuResourceVector;

    internalRenderItem.mFirstSortKey = Render::GetRenderItemFirstSortKey(renderItem.mScissor, renderItem.mMaterialID);
    internalRenderItem.mSecondSortKey = Render::GetRenderItemSecondSortKey(renderItem.mMeshItem, renderItem.mInstance);

    return internalRenderItem;
}
