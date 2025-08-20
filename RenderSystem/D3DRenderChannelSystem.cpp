#include "pch.h"

#include "RenderSystem/D3DRenderChannelSystem.h"
#include "RenderSystem/D3DRenderType.h"
#include "RenderSystem/D3DWindowRenderData.h"
#include "RenderSystem/D3DWindowRenderManager.h"
#include "RenderSystem/ID3DRenderPass.h"
#include <D3DGpuResourceManager/D3DGpuDescriptorHeapManager.h>
#include <D3DGpuResourceManager/D3DGpuResourceManager.h>
#include <D3DGpuResourceManager/D3DGpuTexture.h>
#include <d3dx12.h>

D3DRender::D3DRenderChannelSystem::D3DRenderChannelSystem(const Render::CreationRenderChannelInfo &creationChannelInfo,
                                                          Microsoft::WRL::ComPtr<ID3D12Device> device,
                                                          Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue,
                                                          D3DGRM::D3DGpuResourceManager *gpuResourceManager,
                                                          std::vector<std::unique_ptr<ID3DRenderPass>> renderPipeline)

    : mWindowHandle(creationChannelInfo.mWindowHandle),
      mViewport({creationChannelInfo.mLeft, creationChannelInfo.mTop, creationChannelInfo.mWidth,
                 creationChannelInfo.mHeight, creationChannelInfo.mMinZ, creationChannelInfo.mMaxZ}),
      mDevice(device), mCommandQueue(commandQueue), mGpuResourceManager(gpuResourceManager)
{
    // 결과 검사할것

    HRESULT result;
    result = mDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&mCommandAllocator));

    result = mDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mCommandAllocator.Get(), nullptr,
                                        IID_PPV_ARGS(&mCommandList));

    mRenderPassVector = std::move(renderPipeline);

    mCommandList->Close();
}

D3DRender::D3DRenderChannelSystem::~D3DRenderChannelSystem() {}

void D3DRender::D3DRenderChannelSystem::BeginFrame(const Render::FrameContext &frameContext)
{
    // 여러가지 초기설정을 수행한다.
    auto windowRenderManager = D3DWindowRenderManager::GetInstance();
    std::shared_ptr<D3DWindowRenderData> windowRenderData = windowRenderManager->GetWindowRenderData(mWindowHandle);

    // 채널의 시작전에 RenderSyste에서 동기화이미 했음으로
    // 펜스가필요없다.

    mCommandAllocator->Reset();
    mCommandList->Reset(mCommandAllocator.Get(), nullptr);

    // viewport, scissor rect 전역적인 passbuffer

    // 사전 각 패스들의 의존성 주입
    // 지금은 완벽히 알수는없지

    mChannelFrameContext.mFrameContext = frameContext;
    mChannelFrameContext.mCurrentWindowBackBufferIndex = windowRenderData->GetCurrentBackBufferIndex();

    D3DGRM::D3DGpuTexture *backBuffer = static_cast<D3DGRM::D3DGpuTexture *>(
        windowRenderData->GetBackBuffer(mChannelFrameContext.mCurrentWindowBackBufferIndex).getResource());
    D3DGRM::D3DGpuTexture *depthStencilBuffer =
        static_cast<D3DGRM::D3DGpuTexture *>(windowRenderData->GetDepthStencilBuffer().getResource());
    // 디폴트 후면버퍼, 깊이버퍼초기화 ,상태전이

    D3DGRM::D3DDescriptorHandle backBufferDescriptorHandle;
    D3DGRM::D3DDescriptorHandle depthStencilBufferDescriptorHandle;
    backBuffer->GetDescriptorHandle(D3DGRM::ED3DResourceDescriptorType::eRTV, backBufferDescriptorHandle);
    depthStencilBuffer->GetDescriptorHandle(D3DGRM::ED3DResourceDescriptorType::eDSV,
                                            depthStencilBufferDescriptorHandle);

    // 후면버퍼, 깊이버퍼 클리어
    RECT channelViewportRect = {frameContext.mViewport.TopLeftX, frameContext.mViewport.TopLeftY,
                                frameContext.mViewport.TopLeftX + frameContext.mViewport.Width,
                                frameContext.mViewport.TopLeftY + frameContext.mViewport.Height};

    mCommandList->ClearRenderTargetView(backBufferDescriptorHandle.mCpuDescriptorHandle,
                                        mChannelFrameContext.mFrameContext.mBackGroundColor, 1, &channelViewportRect);
    mCommandList->ClearDepthStencilView(
        depthStencilBufferDescriptorHandle.mCpuDescriptorHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
        mChannelFrameContext.mDepthClearValue, mChannelFrameContext.mStencilClearValue, 0, nullptr);

    ID3D12DescriptorHeap *descriptorHeaps[] = {mGpuResourceManager->GetCSUHeapManager()->GetDescriptorHeap().Get(),
                                               mGpuResourceManager->GetSMPHeapManager()->GetDescriptorHeap().Get()};
    mCommandList->SetDescriptorHeaps(2, descriptorHeaps);
}

void D3DRender::D3DRenderChannelSystem::SubmitRenderItem(
    const std::vector<Render::InternalRenderItem> &renderItemVector)
{

    // 정리
    // 분류
    mRenderItemVector.insert(mRenderItemVector.end(), renderItemVector.begin(), renderItemVector.end());
}

void D3DRender::D3DRenderChannelSystem::SubmitRenderItem(std::vector<Render::InternalRenderItem> &&renderItemVector)
{
    mRenderItemVector.insert(mRenderItemVector.end(), // 현재 벡터의 끝에 추가
                             std::make_move_iterator(renderItemVector.begin()),
                             std::make_move_iterator(renderItemVector.end()));
}

void D3DRender::D3DRenderChannelSystem::EndFrame()
{

    // 최종 DRAW

    // 이 renderpass의 종류와 순서는 정해져있다 .
    // 엔진에서 정한다
    // 특정 종류에 해당하는 패스내부에서 각각의 머터리얼을 사용하는것이다.

    // 일반
    // shadow
    //..
    // post
    // 이순서로 고정된다
    // 물론 특정 단계 pass를 비활성화한다던가, 할수도

    // 초기화할떄 엔진에서 고정으로 패스들을 넣버리고
    // 활성화 비활성화 기능만넣는거지

    for (auto &renderPass : mRenderPassVector)
    {

        renderPass->BeginPass(mCommandList.Get(), mChannelFrameContext);

        if (!mRenderItemVector.empty())
            renderPass->Draw(mCommandList.Get(), mRenderItemVector);

        renderPass->EndPass(mCommandList.Get());
    }

    mCommandList->Close();

    ID3D12CommandList *commandList[] = {mCommandList.Get()};
    mCommandQueue->ExecuteCommandLists(1, commandList);

    mRenderItemVector.clear();
}
