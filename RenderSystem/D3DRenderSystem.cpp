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
#include <CoreDevice/D3DCommandContext.h>
#include <CoreDevice/D3DCoreDevice.h>
#include <D3DGpuResourceManager/D3DGpuConstantBuffer.h>
#include <D3DGpuResourceManager/D3DGpuDescriptorHeapManager.h>
#include <D3DGpuResourceManager/D3DGpuIndexBuffer.h>
#include <D3DGpuResourceManager/D3DGpuResourceManager.h>
#include <D3DGpuResourceManager/D3DGpuStructuredBuffer.h>
#include <D3DGpuResourceManager/D3DGpuTexture.h>
#include <D3DGpuResourceManager/D3DGpuType.h>
#include <D3DGpuResourceManager/D3DGpuVertexBuffer.h>
// #include <Pix/WinPixEventRuntime/pix3.h>
#include <algorithm>
#include <d3dx12.h>
#include <wrl.h>

#include <RenderSystem/D3DMaterialManager.h>

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

    for (int i = 0; i < 3; ++i)
    {
        result = mDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&mCommandAllocator[i]));

        result = mDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mCommandAllocator[i].Get(), nullptr,
                                            IID_PPV_ARGS(&mCommandList[i]));

        mCommandList[i]->Close();
    }

    mCurrentCommandAllocator = mCommandAllocator[0];
    mCurrentCommandList = mCommandList[0];
}

D3DRender::D3DRenderSystem::~D3DRenderSystem() {}

void D3DRender::D3DRenderSystem::RegisterWindow(const Render::CreationRenderChannelInfo &creationChannelInfo)
{

    bool result = mWindowRenderManager->RegisterWindow(creationChannelInfo);

    // std::shared_ptr<D3DWindowRenderData> windowRenderData =
    //     mWindowRenderManager->GetWindowRenderData(creationChannelInfo.mWindowHandle);

    /*  mRenderChannelSystemTable[mNextChannelID] =
          std::make_unique<D3DRenderChannelSystem>(creationChannelInfo, mDevice, mCommandQueue, mGpuResourceManager,
                                                   mRenderPassManager->CreateDefaultRenderPipeline(windowRenderData));

      mChannelWindowMapTable[creationChannelInfo.mWindowHandle].push_back(mNextChannelID);*/
}

void D3DRender::D3DRenderSystem::StartWindow(Core::CommandContext *commandContext, Render::WindowHandle windowHandle,
                                             int backBufferIndex)
{

    Core::D3DCommandContext *d3dCommandContext = static_cast<Core::D3DCommandContext *>(commandContext);
    ID3D12GraphicsCommandList *commandList = d3dCommandContext->GetCommandList();

    ID3D12DescriptorHeap *descriptorHeaps[2] = {mGpuResourceManager->GetCSUHeapManager()->GetDescriptorHeap().Get(),
                                                mGpuResourceManager->GetSMPHeapManager()->GetDescriptorHeap().Get()};

    commandList->SetDescriptorHeaps(2, descriptorHeaps);

    auto materialManager = static_cast<D3DMaterialManager *>(D3DMaterialManager::GetInstance());
    commandList->SetGraphicsRootSignature(materialManager->GetMasterRootSignature());

    // window curr back buffer를 렌더타켓상태로 전환
    std::shared_ptr<D3DWindowRenderData> windowRenderData =
        mWindowRenderManager->GetWindowRenderData((HWND)windowHandle);
    int currentBackBufferIndex = backBufferIndex;

    // BACK BUFFER 2번버퍼 포인터 문제
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
        commandList->ClearDepthStencilView(depthStecnilDescriptorHandle.mCpuDescriptorHandle,
                                           D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
    }

    if (backBuffer->GetDescriptorHandle(D3DGRM::ED3DResourceDescriptorType::eRTV, backBufferDescriptorHandle))
    {

        auto backBufferPresentTransition = CD3DX12_RESOURCE_BARRIER::Transition(
            backBuffer->GetResource().Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

        commandList->ResourceBarrier(1, &backBufferPresentTransition);

        float color[4] = {0.2, 0.2, 0.2, 0};
        commandList->ClearRenderTargetView(backBufferDescriptorHandle.mCpuDescriptorHandle, color, 0, nullptr);
    }

    // mCurrentCommandList->Close();
    // ID3D12CommandList *commandLists[] = {mCurrentCommandList.Get()};
    // mCommandQueue->ExecuteCommandLists(1, commandLists);
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

void D3DRender::D3DRenderSystem::PresentWindow(Core::CommandContext *commandContext, Render::WindowHandle windowHandle,
                                               int backBufferIndex)
{

    Core::D3DCommandContext *d3dCommandContext = static_cast<Core::D3DCommandContext *>(commandContext);
    ID3D12GraphicsCommandList *commandList = d3dCommandContext->GetCommandList();

    std::shared_ptr<D3DWindowRenderData> windowRenderData =
        mWindowRenderManager->GetWindowRenderData((HWND)windowHandle);
    int currentBackBufferIndex = backBufferIndex;
    D3DGRM::D3DGpuTexture *backBuffer =
        static_cast<D3DGRM::D3DGpuTexture *>(windowRenderData->GetBackBuffer(currentBackBufferIndex).getResource());
    Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain = windowRenderData->GetSwapChain();
    D3DGRM::D3DDescriptorHandle backBufferDescriptorHandle;

    if (backBuffer->GetDescriptorHandle(D3DGRM::ED3DResourceDescriptorType::eRTV, backBufferDescriptorHandle))
    {
        // mCommandList->Reset(mCommandAllocator.Get(), nullptr);

        auto backBufferPresentTransition = CD3DX12_RESOURCE_BARRIER::Transition(
            backBuffer->GetResource().Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

        commandList->ResourceBarrier(1, &backBufferPresentTransition);

        commandList->Close();
        ID3D12CommandList *commandLists[] = {commandList};
        mCommandQueue->ExecuteCommandLists(1, commandLists);

        // Core::D3DCoreDevice *d3dCoreDevice = static_cast<Core::D3DCoreDevice *>(Core::D3DCoreDevice::GetInstance());
        // d3dCoreDevice->FlushCommandQueue();

        HRESULT hr = swapChain->Present(0, 0);
        if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
        {
            int a = 2;
        }

        windowRenderData->IncrementBackBufferIndex();
    }
}

int D3DRender::D3DRenderSystem::WindowResize(Render::WindowHandle windowHandle)
{
    return mWindowRenderManager->WindowResize((HWND)windowHandle);
}

// Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> D3DRender::D3DRenderSystem::GetCommandList() const
//{
//     return mCommandList;
// }
//
// Microsoft::WRL::ComPtr<ID3D12CommandAllocator> D3DRender::D3DRenderSystem::GetCommandAllocator() const
//{
//     return mCommandAllocator;
// }

#pragma region Refactoring

void D3DRender::D3DRenderSystem::StartFrame(Render::WindowHandle windowHandle)
{

    // FlushCommandQueue();

    mCurrentCommandAllocator->Reset();
    mCurrentCommandList->Reset(mCurrentCommandAllocator.Get(), nullptr);

    mCurrWindowHandle = windowHandle;

    // window curr back buffer를 렌더타켓상태로 전환
    std::shared_ptr<D3DWindowRenderData> windowRenderData =
        mWindowRenderManager->GetWindowRenderData((HWND)windowHandle);
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
        mCurrentCommandList->ClearDepthStencilView(depthStecnilDescriptorHandle.mCpuDescriptorHandle,
                                                   D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0,
                                                   nullptr);
    }

    if (backBuffer->GetDescriptorHandle(D3DGRM::ED3DResourceDescriptorType::eRTV, backBufferDescriptorHandle))
    {

        auto backBufferPresentTransition = CD3DX12_RESOURCE_BARRIER::Transition(
            backBuffer->GetResource().Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

        mCurrentCommandList->ResourceBarrier(1, &backBufferPresentTransition);

        float color[4] = {0.2, 0.2, 0.2, 0};
        mCurrentCommandList->ClearRenderTargetView(backBufferDescriptorHandle.mCpuDescriptorHandle, color, 0, nullptr);
    }

    //  mCommandList->Close();
    //  ID3D12CommandList *commandLists[] = {mCommandList.Get()};
    // mCommandQueue->ExecuteCommandLists(1, commandLists);
}

// void D3DRender::D3DRenderSystem::StartView(Render::RenderChannelID channelID)
//{
//
//     // commandList reset 등의 정말 단순한 초기화 ,설정 작업만 수행
//
//     auto it = mRenderChannelSystemTable.find(channelID);
//
//     if (it == mRenderChannelSystemTable.end())
//         return;
//
//     (*it).second->StartView();
// }
//
// void D3DRender::D3DRenderSystem::EndView(Render::RenderChannelID channelID) {}

void D3DRender::D3DRenderSystem::SetUpPassData(Core::CommandContext *commandContext,
                                               const Render::FrameContext &passData)
{
    Core::D3DCommandContext *d3dCommandContext = static_cast<Core::D3DCommandContext *>(commandContext);
    ID3D12GraphicsCommandList *commandList = d3dCommandContext->GetCommandList();
    // 이게 전역설정들이다, 이것을 유지한다.
    // 그리고 pso,root시그니처가 다시 바인딩될때마다 ,이 전역을 재설정한다. 뭐 이런방식으로진행
    mCurrPassFrameContext = passData;

    D3D12_RECT scissorRect = ConvertToD3DRect(passData.mScissorRect);
    commandList->RSSetScissorRects(1, &scissorRect);

    D3D12_VIEWPORT viewport = ConvertToD3DViewport(passData.mViewport);
    commandList->RSSetViewports(1, &viewport);
}

void D3DRender::D3DRenderSystem::SetUpStencilValue(Core::CommandContext *commandContext, uint32_t value)
{

    Core::D3DCommandContext *d3dCommandContext = static_cast<Core::D3DCommandContext *>(commandContext);
    ID3D12GraphicsCommandList *commandList = d3dCommandContext->GetCommandList();
    commandList->OMSetStencilRef(value);
}

void D3DRender::D3DRenderSystem::Draw(Core::CommandContext *commandContext,
                                      const std::vector<Render::RenderItem> &renderItemList)
{

    Core::D3DCommandContext *d3dCommandContext = static_cast<Core::D3DCommandContext *>(commandContext);
    ID3D12GraphicsCommandList *commandList = d3dCommandContext->GetCommandList();

    if (renderItemList.empty())
        return;

    //// descriptor heap binding
    // ID3D12DescriptorHeap *descriptorHeaps[2] = {mGpuResourceManager->GetCSUHeapManager()->GetDescriptorHeap().Get(),
    //                                             mGpuResourceManager->GetSMPHeapManager()->GetDescriptorHeap().Get()};

    // commandList->SetDescriptorHeaps(2, descriptorHeaps);

    const Render::RenderItem *beforeRenderItem = nullptr;
    MaterialItem *beforeMaterialItem = nullptr;

    for (size_t renderItemIndex = 0; renderItemIndex < renderItemList.size(); ++renderItemIndex)
    {

        const Render::RenderItem *currRenderItem = &renderItemList[renderItemIndex];

        MaterialItem *currMaterialItem = mMaterialManager->GetMaterialItem(currRenderItem->mMaterialID);

        // 이전 draw와의 비교
        // 이전 렌더아이템들과 비교

        BindPSOIfNeeded(commandList, beforeRenderItem, currRenderItem, beforeMaterialItem, currMaterialItem);

        BindScissorRectIfNeeded(commandList, beforeRenderItem, currRenderItem);

        BindShaderResources(commandList, currRenderItem);

        BindMeshBufferIfNeeded(commandList, beforeRenderItem, currRenderItem);

        DrawRenderItem(commandList, currRenderItem);

        beforeRenderItem = currRenderItem;
        beforeMaterialItem = currMaterialItem;
    }
}

void D3DRender::D3DRenderSystem::DrawFullScreen(Core::CommandContext *commandContext,
                                                const Render::RenderItem &renderItem)
{
    Core::D3DCommandContext *d3dCommandContext = static_cast<Core::D3DCommandContext *>(commandContext);
    ID3D12GraphicsCommandList *commandList = d3dCommandContext->GetCommandList();
    const Render::RenderItem *beforeRenderItem = nullptr;
    MaterialItem *beforeMaterialItem = nullptr;

    const Render::RenderItem *currRenderItem = &renderItem;

    MaterialItem *currMaterialItem = mMaterialManager->GetMaterialItem(currRenderItem->mMaterialID);

    // PSO
    BindPSOIfNeeded(commandList, beforeRenderItem, currRenderItem, beforeMaterialItem, currMaterialItem);

    // ScissorRect

    BindScissorRectIfNeeded(commandList, beforeRenderItem, currRenderItem);

    BindShaderResources(commandList, currRenderItem);

    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    commandList->DrawInstanced(4, 1, 0, 0);
}

void D3DRender::D3DRenderSystem::ExecuteResourceBarriers(Core::CommandContext *commandContext,
                                                         std::vector<Render::ResourceBarrier> &resourceBarriers)
{
    Core::D3DCommandContext *d3dCommandContext = static_cast<Core::D3DCommandContext *>(commandContext);
    ID3D12GraphicsCommandList *commandList = d3dCommandContext->GetCommandList();

    std::vector<D3D12_RESOURCE_BARRIER> d3dResourceBarrierVec(resourceBarriers.size());
    for (int i = 0; i < resourceBarriers.size(); ++i)
    {
        D3DGRM::D3DGpuResource *d3dGpuResource = (D3DGRM::D3DGpuResource *)resourceBarriers[i].mResource;

        d3dResourceBarrierVec[i].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        d3dResourceBarrierVec[i].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        d3dResourceBarrierVec[i].Transition.pResource = d3dGpuResource->GetResource().Get();
        d3dResourceBarrierVec[i].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        d3dResourceBarrierVec[i].Transition.StateBefore =
            D3DGRM::ConvertToD3DResourceState(resourceBarriers[i].mBeforeState);

        if (d3dResourceBarrierVec[i].Transition.StateBefore == D3D12_RESOURCE_STATE_COMMON)
        {
            int a = 2;
        }

        d3dResourceBarrierVec[i].Transition.StateAfter =
            D3DGRM::ConvertToD3DResourceState(resourceBarriers[i].mAfterState);
    }

    if (d3dResourceBarrierVec.size() != 0)
        commandList->ResourceBarrier(d3dResourceBarrierVec.size(), d3dResourceBarrierVec.data());
}

void D3DRender::D3DRenderSystem::ExecuteResourceBarrier(const Render::ResourceBarrier &resourceBarrier) {}

void D3DRender::D3DRenderSystem::ClearRenderTarget(Core::CommandContext *commandContext,
                                                   GRM::IGpuResource *renderTarget, float color[4], Render::RECT rect)
{
    Core::D3DCommandContext *d3dCommandContext = static_cast<Core::D3DCommandContext *>(commandContext);
    ID3D12GraphicsCommandList *commandList = d3dCommandContext->GetCommandList();

    D3DGRM::D3DGpuTexture *d3dRenderTarget = static_cast<D3DGRM::D3DGpuTexture *>(renderTarget);
    D3DGRM::D3DDescriptorHandle renderTargetHandle;
    if (d3dRenderTarget->GetDescriptorHandle(D3DGRM::ED3DResourceDescriptorType::eRTV, renderTargetHandle))
    {
        D3D12_RECT d3dRect = ConvertToD3DRect(rect);
        commandList->ClearRenderTargetView(renderTargetHandle.mCpuDescriptorHandle, color, 1, &d3dRect);
    }
}

void D3DRender::D3DRenderSystem::ClearDepthStencil(Core::CommandContext *commandContext,
                                                   GRM::IGpuResource *depthStencil, float value, Render::RECT rect)
{
    Core::D3DCommandContext *d3dCommandContext = static_cast<Core::D3DCommandContext *>(commandContext);
    ID3D12GraphicsCommandList *commandList = d3dCommandContext->GetCommandList();

    D3DGRM::D3DGpuTexture *d3dDepthStencil = static_cast<D3DGRM::D3DGpuTexture *>(depthStencil);
    D3DGRM::D3DDescriptorHandle cpuHandle;
    if (d3dDepthStencil->GetDescriptorHandle(D3DGRM::ED3DResourceDescriptorType::eDSV, cpuHandle))
    {
        D3D12_RECT d3dRect = ConvertToD3DRect(rect);
        // 일단 두개모두 설정...

        D3D12_CLEAR_FLAGS clearFlags = D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL;
        commandList->ClearDepthStencilView(cpuHandle.mCpuDescriptorHandle, clearFlags, value, 0, 1, &d3dRect);
    }
}

void D3DRender::D3DRenderSystem::SetRenderTarget(Core::CommandContext *commandContext, GRM::IGpuResource *rendertarget,
                                                 GRM::IGpuResource *depthStencil)
{
    Core::D3DCommandContext *d3dCommandContext = static_cast<Core::D3DCommandContext *>(commandContext);
    ID3D12GraphicsCommandList *commandList = d3dCommandContext->GetCommandList();

    D3DGRM::D3DGpuTexture *d3dRenderTarget = static_cast<D3DGRM::D3DGpuTexture *>(rendertarget);
    D3DGRM::D3DGpuTexture *d3dDepthStencil = static_cast<D3DGRM::D3DGpuTexture *>(depthStencil);

    D3D12_CPU_DESCRIPTOR_HANDLE *renderTargetCpuHandlePtr = nullptr;
    D3D12_CPU_DESCRIPTOR_HANDLE *depthStencilCpuHandlePtr = nullptr;

    D3DGRM::D3DDescriptorHandle renderTargetHandle;
    if (d3dRenderTarget &&
        d3dRenderTarget->GetDescriptorHandle(D3DGRM::ED3DResourceDescriptorType::eRTV, renderTargetHandle))
    {
        renderTargetCpuHandlePtr = &renderTargetHandle.mCpuDescriptorHandle;
    }

    D3DGRM::D3DDescriptorHandle detphStencilHandle;
    if (d3dDepthStencil &&
        d3dDepthStencil->GetDescriptorHandle(D3DGRM::ED3DResourceDescriptorType::eDSV, detphStencilHandle))
    {

        depthStencilCpuHandlePtr = &detphStencilHandle.mCpuDescriptorHandle;
    }

    commandList->OMSetRenderTargets(1, renderTargetCpuHandlePtr, true, depthStencilCpuHandlePtr);
}

void D3DRender::D3DRenderSystem::BindPSOIfNeeded(ID3D12GraphicsCommandList *commandList,
                                                 const Render::RenderItem *beforeRenderItem,
                                                 const Render::RenderItem *currRenderItem,
                                                 MaterialItem *beforeMaterialItem, MaterialItem *currMaterialItem)
{

    if (beforeRenderItem == nullptr)
    {
        commandList->SetPipelineState(currMaterialItem->mPso);
        //  commandList->SetGraphicsRootSignature(currMatMainPass->mRootSignature.Get());
        BindGlobalShaderResource(commandList);
    }
    else if (beforeRenderItem->mMaterialID != currRenderItem->mMaterialID)
    {
        // 이전 PSO와 비교
        commandList->SetPipelineState(currMaterialItem->mPso);

        //// RootSinature비교
        // if (beforeMatMainPass->mRootSignature != currMatMainPass->mRootSignature)
        //{
        //     commandList->SetGraphicsRootSignature(currMatMainPass->mRootSignature.Get());
        //     // 루트 시그니처가 다시 새로운것으로 설정됬다.-> 모든 셰이더리소스들이 다시 다 바인딩되어야한다.
        //     // 따라서 전역 셰이더리소스들도 다시 모두 바인딩한다.
        //     BindGlobalShaderResource(commandList, currMatMainPass);
        //     // 또한 다른 Object단위의 셰이더 리소스들도 무조건 다시바인딩되도록해야한다.
        // }
    }
}

void D3DRender::D3DRenderSystem::BindScissorRectIfNeeded(ID3D12GraphicsCommandList *commandList,
                                                         const Render::RenderItem *beforeRenderItem,
                                                         const Render::RenderItem *currRenderItem)
{
    // scissorRect -> 고민,

    if (currRenderItem->bUseScissor)
    {
        D3D12_RECT scissorRect = ConvertToD3DRect(currRenderItem->mScissor);

        if (beforeRenderItem)
        {
            if (beforeRenderItem->bUseScissor)
            {
                if ((beforeRenderItem->mScissor != currRenderItem->mScissor))
                {
                    commandList->RSSetScissorRects(1, &scissorRect);
                }
            }
            else
            {
                commandList->RSSetScissorRects(1, &scissorRect);
            }
        }
        else
        {
            commandList->RSSetScissorRects(1, &scissorRect);
        }
    }
    else
    {

        if (beforeRenderItem && beforeRenderItem->bUseScissor)
        {
            // pass에서 설정한 scissorRect로 설정한다.
            D3D12_RECT scissorRect = ConvertToD3DRect(mCurrPassFrameContext.mScissorRect);
            commandList->RSSetScissorRects(1, &scissorRect);
        }
    }

    // if (beforeRenderItem != nullptr && (beforeRenderItem->mScissor != currRenderItem->mScissor))
    //{

    //    commandList->RSSetScissorRects(1, (D3D12_RECT *)&currRenderItem->mScissor);
    //    // commandList->RSSetScissorRects(0,nullptr);
    //}
}

void D3DRender::D3DRenderSystem::BindMeshBufferIfNeeded(ID3D12GraphicsCommandList *commandList,
                                                        const Render::RenderItem *beforeRenderItem,
                                                        const Render::RenderItem *currRenderItem)
{
    if (currRenderItem->mPrimitiveTopology == Render::EPrimitiveTopology::eTriangleList)
    {

        // vertex buffer, index buffer
        if (beforeRenderItem == nullptr || (beforeRenderItem->mMeshItem != currRenderItem->mMeshItem))
        {
            D3DGRM::D3DGpuVertexBuffer *vertexBuffer =
                static_cast<D3DGRM::D3DGpuVertexBuffer *>(currRenderItem->mMeshItem.mVertexBuffer);
            D3DGRM::D3DGpuIndexBuffer *indeBuffer =
                static_cast<D3DGRM::D3DGpuIndexBuffer *>(currRenderItem->mMeshItem.mIndexBuffer);

            D3D12_VERTEX_BUFFER_VIEW vbv = vertexBuffer->GetVertexBufferView();
            D3D12_INDEX_BUFFER_VIEW ibv = indeBuffer->GetIndexBufferView();
            commandList->IASetVertexBuffers(0, 1, &vbv);
            commandList->IASetIndexBuffer(&ibv);
            commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        }
    }
    else if (currRenderItem->mPrimitiveTopology == Render::EPrimitiveTopology::ePointList)
    {

        commandList->IASetVertexBuffers(0, 0, nullptr);
        commandList->IASetIndexBuffer(nullptr);
        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
    }
    else if (currRenderItem->mPrimitiveTopology == Render::EPrimitiveTopology::eLineList)
    {
        D3DGRM::D3DGpuVertexBuffer *vertexBuffer =
            static_cast<D3DGRM::D3DGpuVertexBuffer *>(currRenderItem->mMeshItem.mVertexBuffer);
        D3D12_VERTEX_BUFFER_VIEW vbv = vertexBuffer->GetVertexBufferView();

        commandList->IASetVertexBuffers(0, 1, &vbv);
        commandList->IASetIndexBuffer(nullptr);
        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
    }
}

void D3DRender::D3DRenderSystem::BindShaderResources(ID3D12GraphicsCommandList *commandList,
                                                     const Render::RenderItem *currRenderItem)
{
    // resourceBinding
    // 일단동일한 패스버퍼라도 루트파라미터의 인덱스가 달라지면 다시바인딩해야하니
    // 일단은 리소스는 항상 다시 바인딩

    // 1. buffer (일반상수버퍼)
    // 순서대로 1번파라미터 부터 바인딩 수행
    int rootParameterIndex = 1;
    for (const auto &bufferResource : currRenderItem->mBindingGpuBufferResourceVector)
    {
        if (bufferResource.mType != Render::EShaderResourceType::eConstantBuffer)
            continue;

        D3DGRM::D3DGpuConstantBuffer *d3dConstantBuffer = (D3DGRM::D3DGpuConstantBuffer *)bufferResource.gpuResource;

        D3D12_GPU_VIRTUAL_ADDRESS addr =
            d3dConstantBuffer->GetResource()->GetGPUVirtualAddress() + bufferResource.mOffset;
        commandList->SetGraphicsRootConstantBufferView(rootParameterIndex, addr);

        rootParameterIndex++;
    }

    rootParameterIndex = 4;
    // 4 . tex
    for (const auto &texResource : currRenderItem->mBindingGpuTexResourceVector)
    {
        D3DGRM::D3DDescriptorHandle handle;
        D3DGRM::D3DGpuResource *d3dResource = (D3DGRM::D3DGpuResource *)texResource.gpuResource;
        d3dResource->GetDescriptorHandle(D3DGRM::ED3DResourceDescriptorType::eSRV, handle);
        commandList->SetGraphicsRootDescriptorTable(rootParameterIndex, handle.mGpuDescriptorHandle);
        rootParameterIndex++;
    }

    // 오브젝트용 구조적버퍼 루트파라미터 8번부터 시작 최대 10번까지
    rootParameterIndex = 8;
    for (const auto &bufferResource : currRenderItem->mBindingGpuBufferResourceVector)
    {
        if (bufferResource.mType != Render::EShaderResourceType::eStructuredBuffer)
            continue;

        // 2 .structured buffer;
        D3DGRM::D3DGpuStructuredBuffer *structuredBuffer =
            static_cast<D3DGRM::D3DGpuStructuredBuffer *>(bufferResource.gpuResource);

        D3D12_GPU_VIRTUAL_ADDRESS addr =
            structuredBuffer->GetResource()->GetGPUVirtualAddress() + bufferResource.mOffset;

        commandList->SetGraphicsRootShaderResourceView(rootParameterIndex, addr);

        rootParameterIndex++;
    }

    // for (const auto &resourceElement : currRenderItem->mBindingGpuResourceVector)
    //{

    //    std::unordered_map<std::string, int>::const_iterator rootParameterIt =
    //        currMatMainPass->mShaderResourceRootParameterBindingInfo.find(resourceElement.mName);

    //    int rootParameterIndex = rootParameterIt->second;

    //    D3DGRM::D3DDescriptorHandle handle;
    //    switch (resourceElement.mType)
    //    {
    //    case Render::EShaderResourceType::eConstantBuffer:
    //    {

    //        D3DGRM::D3DGpuConstantBuffer *d3dConstantBuffer =
    //            (D3DGRM::D3DGpuConstantBuffer *)resourceElement.gpuResource;
    //        // d3dConstantBuffer->GetDescriptorHandle(D3DGRM::ED3DResourceDescriptorType::eCBV, handle);
    //        handle = d3dConstantBuffer->GetConstantDescriptorHandle(resourceElement.mOffset);
    //    }
    //    break;
    //    case Render::EShaderResourceType::eStructuredBuffer:
    //    case Render::EShaderResourceType::eTexture:
    //    {
    //        D3DGRM::D3DGpuResource *d3dResource = (D3DGRM::D3DGpuResource *)resourceElement.gpuResource;
    //        d3dResource->GetDescriptorHandle(D3DGRM::ED3DResourceDescriptorType::eSRV, handle);
    //    }
    //    break;
    //    case Render::EShaderResourceType::eSampler:

    //    {

    //        D3DGRM::D3DGpuResource *d3dResource = (D3DGRM::D3DGpuResource *)resourceElement.gpuResource;
    //        d3dResource->GetDescriptorHandle(D3DGRM::ED3DResourceDescriptorType::eSMP, handle);
    //    }

    //    break;
    //    }

    //    commandList->SetGraphicsRootDescriptorTable(rootParameterIndex, handle.mGpuDescriptorHandle);
    //}
}

void D3DRender::D3DRenderSystem::DrawRenderItem(ID3D12GraphicsCommandList *commandList,
                                                const Render::RenderItem *currRenderItem)
{

    // 인스턴스버퍼 offset
    // 인스턴스버퍼가 여러종류에 다양한 offset을 가질수있을텐데
    if (currRenderItem->mDrawType == Render::EDrawType::eIndex)
    {
        commandList->DrawIndexedInstanced(currRenderItem->mMeshItem.mIndexNum, currRenderItem->mInstance.mInstanceCount,
                                          currRenderItem->mMeshItem.mIndexOffset,
                                          currRenderItem->mMeshItem.mVertexOffset,
                                          currRenderItem->mInstance.mInstanceBufferOffset);
    }
    else
    {
        commandList->DrawInstanced(currRenderItem->mMeshItem.mVertexNum, currRenderItem->mInstance.mInstanceCount,
                                   currRenderItem->mMeshItem.mVertexOffset,
                                   currRenderItem->mInstance.mInstanceBufferOffset);
    }
}

void D3DRender::D3DRenderSystem::BindGlobalShaderResource(ID3D12GraphicsCommandList *commandList)
{

    // 전역버퍼 바인딩 . 무조건 0번 슬롯 (루트파라미터)

    D3DGRM::D3DGpuConstantBuffer *passBuffer =
        static_cast<D3DGRM::D3DGpuConstantBuffer *>(mCurrPassFrameContext.mGlobalPassBufferResouce.gpuResource);

    D3D12_GPU_VIRTUAL_ADDRESS passBufferGpuAddr =
        passBuffer->GetResource()->GetGPUVirtualAddress() + mCurrPassFrameContext.mGlobalPassBufferResouce.mOffset;

    commandList->SetGraphicsRootConstantBufferView(0, passBufferGpuAddr);

    if (mCurrPassFrameContext.mGlobalStructuredBufferResource.gpuResource)
    {
        // 2 .structured buffer;
        D3DGRM::D3DGpuStructuredBuffer *structuredBuffer = static_cast<D3DGRM::D3DGpuStructuredBuffer *>(
            mCurrPassFrameContext.mGlobalStructuredBufferResource.gpuResource);

        D3D12_GPU_VIRTUAL_ADDRESS addr = structuredBuffer->GetResource()->GetGPUVirtualAddress() +
                                         mCurrPassFrameContext.mGlobalStructuredBufferResource.mOffset;

        commandList->SetGraphicsRootShaderResourceView(3, addr);
    }
    // 혹시나 전역 텍스처 리소스가 있다면 바인딩.

    for (auto &element : mCurrPassFrameContext.mGlobalPassTexResourceVector)
    {
        uint32_t slotIndex = element.first;

        D3DGRM::D3DGpuTexture *tex = static_cast<D3DGRM::D3DGpuTexture *>(element.second.gpuResource);

        D3DGRM::D3DDescriptorHandle gpuHandle;
        tex->GetDescriptorHandle(D3DGRM::ED3DResourceDescriptorType::eSRV, gpuHandle);
        commandList->SetGraphicsRootDescriptorTable(slotIndex, gpuHandle.mGpuDescriptorHandle);
    }
}

void D3DRender::D3DRenderSystem::SetUpRenderTarget(ID3D12GraphicsCommandList *commandList)
{
    D3DGRM::D3DGpuTexture *renderTarget = static_cast<D3DGRM::D3DGpuTexture *>(mCurrPassFrameContext.mRenderTarget);
    D3DGRM::D3DGpuTexture *depthStencilBuffer = nullptr;
    if (renderTarget == nullptr)
    {
        // 기본 후면버퍼

        // window curr back buffer를 렌더타켓상태로 전환
        std::shared_ptr<D3DWindowRenderData> windowRenderData =
            mWindowRenderManager->GetWindowRenderData((HWND)mCurrWindowHandle);
        int currentBackBufferIndex = windowRenderData->GetCurrentBackBufferIndex();

        renderTarget =
            static_cast<D3DGRM::D3DGpuTexture *>(windowRenderData->GetBackBuffer(currentBackBufferIndex).getResource());
        depthStencilBuffer =
            static_cast<D3DGRM::D3DGpuTexture *>(windowRenderData->GetDepthStencilBuffer().getResource());
    }

    D3DGRM::D3DDescriptorHandle renderTargetDescriptorHandle;
    renderTarget->GetDescriptorHandle(D3DGRM::ED3DResourceDescriptorType::eRTV, renderTargetDescriptorHandle);

    D3DGRM::D3DDescriptorHandle depthStencilDescriptorHandle;
    depthStencilBuffer->GetDescriptorHandle(D3DGRM::ED3DResourceDescriptorType::eDSV, depthStencilDescriptorHandle);

    // 렌더타켓으로 설정
    commandList->OMSetRenderTargets(1, &renderTargetDescriptorHandle.mCpuDescriptorHandle, false,
                                    &depthStencilDescriptorHandle.mCpuDescriptorHandle);
}

// D3D12_RESOURCE_STATES D3DRender::D3DRenderSystem::ConvertToD3DResourceState(EResourceState resourceState)
//{
//
//     switch (resourceState)
//     {
//     case EResourceState::eNone:
//         return D3D12_RESOURCE_STATE_COMMON;
//     case EResourceState::eRenderTarget:
//         return D3D12_RESOURCE_STATE_RENDER_TARGET;
//     case EResourceState::eGenericRead:
//         return D3D12_RESOURCE_STATE_GENERIC_READ;
//     case EResourceState::ePresent:
//         return D3D12_RESOURCE_STATE_PRESENT;
//     case EResourceState::eReadDepthStencil:
//         return D3D12_RESOURCE_STATE_DEPTH_READ;
//     case EResourceState::eWriteDepthStencil:
//         return D3D12_RESOURCE_STATE_DEPTH_WRITE;
//     case EResourceState::eCopySource:
//         return D3D12_RESOURCE_STATE_COPY_SOURCE;
//     case EResourceState::eCopyDest:
//         return D3D12_RESOURCE_STATE_COPY_DEST;
//     default:
//         return D3D12_RESOURCE_STATE_COMMON;
//     }
// }

D3D12_RECT D3DRender::D3DRenderSystem::ConvertToD3DRect(const Render::RECT &rect)
{
    D3D12_RECT d3dRect;
    d3dRect.left = rect.mLeft;
    d3dRect.right = rect.mRight;
    d3dRect.top = rect.mTop;
    d3dRect.bottom = rect.mBottom;
    return d3dRect;
}

#pragma endregion

void D3DRender::D3DRenderSystem::FlushCommandQueue()
{

    Core::D3DCoreDevice *d3dCoreDevice = static_cast<Core::D3DCoreDevice *>(Core::CoreDevice::GetInstance());
    d3dCoreDevice->FlushCommandQueue();
}

Render::InternalRenderItem D3DRender::D3DRenderSystem::ConvertInteranlRenderItem(
    const Render::RenderItem &renderItem) const
{

    Render::InternalRenderItem internalRenderItem;
    /* internalRenderItem.mScissor = renderItem.mScissor;
     internalRenderItem.mMaterialID = renderItem.mMaterialID;
     internalRenderItem.mInstance = renderItem.mInstance;
     internalRenderItem.mMeshItem = renderItem.mMeshItem;
     internalRenderItem.mBindingGpuResourceVector = renderItem.mBindingGpuResourceVector;

     internalRenderItem.mFirstSortKey = Render::GetRenderItemFirstSortKey(renderItem.mScissor, renderItem.mMaterialID);
     internalRenderItem.mSecondSortKey = Render::GetRenderItemSecondSortKey(renderItem.mMeshItem,
     renderItem.mInstance);*/

    return internalRenderItem;
}
