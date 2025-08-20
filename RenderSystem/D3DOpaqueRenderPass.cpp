#include "pch.h"

#include "RenderSystem/D3DMaterialManager.h"
#include "RenderSystem/D3DOpaqueRenderPass.h"
#include <D3DGpuResourceManager/D3DGpuIndexBuffer.h>
#include <D3DGpuResourceManager/D3DGpuResource.h>
#include <D3DGpuResourceManager/D3DGpuResourceManager.h>
#include <D3DGpuResourceManager/D3DGpuTexture.h>
#include <D3DGpuResourceManager/D3DGpuVertexBuffer.h>
D3DRender::D3DOpaqueRenderPass::D3DOpaqueRenderPass(Microsoft::WRL::ComPtr<ID3D12Device> device,
                                                    D3DGRM::D3DGpuResourceManager *resourceManager,
                                                    std::shared_ptr<D3DRender::D3DMaterialManager> materialManager,
                                                    D3DGRM::D3DGpuTexture *windowBackBuffer[2],
                                                    D3DGRM::D3DGpuTexture *windowDepthStencilBuffer)
    : ID3DRenderPass(materialManager), mDevice(device), mResourceManager(resourceManager),
      mWindowDepthStencilBuffer(windowDepthStencilBuffer)
{
    mWindowBackBuffer[0] = windowBackBuffer[0];
    mWindowBackBuffer[1] = windowBackBuffer[1];
}

void D3DRender::D3DOpaqueRenderPass::BeginPass(ID3D12GraphicsCommandList *commandList,
                                               const D3DRender::D3DRenderChannelFrameContext &channelFrameContext)
{
    // 기본설정
    D3D12_VIEWPORT viewport = ConvertToD3DViewport(channelFrameContext.mFrameContext.mViewport);
    commandList->RSSetViewports(1, &viewport);

    // ScissorRect는 렌더아이템별로 수행될것같은데
    // commandList->RSSetScissorRects(1, reinterpret_cast<const
    // D3D12_RECT*>(&channelFrameContext.mFrameContext.mScissorRect));

    // 렌더타켓을 window back buffer로 설정
    D3DGRM::D3DDescriptorHandle renderTargetHandle;
    D3DGRM::D3DDescriptorHandle depthStencilHandle;
    mWindowBackBuffer[channelFrameContext.mCurrentWindowBackBufferIndex]->GetDescriptorHandle(
        D3DGRM::ED3DResourceDescriptorType::eRTV, renderTargetHandle);
    mWindowDepthStencilBuffer->GetDescriptorHandle(D3DGRM::ED3DResourceDescriptorType::eDSV, depthStencilHandle);

    commandList->OMSetRenderTargets(1, &renderTargetHandle.mCpuDescriptorHandle, true,
                                    &depthStencilHandle.mCpuDescriptorHandle);
    // 깊이 버퍼도 설정

    mChannelFrameContext = channelFrameContext;
}

void D3DRender::D3DOpaqueRenderPass::Draw(ID3D12GraphicsCommandList *commandList,
                                          const std::vector<Render::InternalRenderItem> &renderItems)
{
    // 각 렌더아이템이 참조하는 머터리얼의 pso rootsignature , shader resource binding 작업과 draw수행

    if (renderItems.empty())
        return;

    const Render::InternalRenderItem *beforeRenderItem = nullptr;
    D3DMaterialItem *beforeMaterialItem = nullptr;

    for (size_t renderItemIndex = 0; renderItemIndex < renderItems.size(); ++renderItemIndex)
    {

        const Render::InternalRenderItem *currRenderItem = &renderItems[renderItemIndex];

        D3DMaterialItem *currMaterialItem = mMaterialManager->GetMaterialItem(currRenderItem->mMaterialID);

        D3DMainRenderPassInfo *currMatMainPass = &currMaterialItem->mMainPass;
        D3DMainRenderPassInfo *beforeMatMainPass = nullptr;
        if (beforeMaterialItem != nullptr)
            beforeMatMainPass = &beforeMaterialItem->mMainPass;

        // 이전 draw와의 비교
        // 이전 렌더아이템들과 비교

        BindPSOIfNeeded(commandList, beforeRenderItem, currRenderItem, beforeMatMainPass, currMatMainPass);

        BindScissorRectIfNeeded(commandList, beforeRenderItem, currRenderItem);

        BindShaderResources(commandList, currRenderItem, currMatMainPass);

        BindMeshBufferIfNeeded(commandList, beforeRenderItem, currRenderItem);

        DrawRenderItem(commandList, currRenderItem);

        beforeRenderItem = currRenderItem;
        beforeMaterialItem = currMaterialItem;
    }
}

void D3DRender::D3DOpaqueRenderPass::EndPass(ID3D12GraphicsCommandList *commandList) {}

void D3DRender::D3DOpaqueRenderPass::BindPSOIfNeeded(ID3D12GraphicsCommandList *commandList,
                                                     const Render::InternalRenderItem *beforeRenderItem,
                                                     const Render::InternalRenderItem *currRenderItem,
                                                     const D3DMainRenderPassInfo *beforeMatMainPass,
                                                     const D3DMainRenderPassInfo *currMatMainPass)

{

    if (beforeRenderItem == nullptr)
    {
        commandList->SetPipelineState(currMatMainPass->mPSO.Get());
        commandList->SetGraphicsRootSignature(currMatMainPass->mRootSignature.Get());
    }
    else if (beforeRenderItem->mMaterialID != currRenderItem->mMaterialID)
    {
        // 이전 PSO와 비교
        if (beforeMatMainPass->mPSO != currMatMainPass->mPSO)
        {
            commandList->SetPipelineState(currMatMainPass->mPSO.Get());
        }

        // RootSinature비교
        if (beforeMatMainPass->mRootSignature != currMatMainPass->mRootSignature)
        {
            commandList->SetGraphicsRootSignature(currMatMainPass->mRootSignature.Get());
        }
    }
}

void D3DRender::D3DOpaqueRenderPass::BindScissorRectIfNeeded(ID3D12GraphicsCommandList *commandList,
                                                             const Render::InternalRenderItem *beforeRenderItem,
                                                             const Render::InternalRenderItem *currRenderItem)
{
    // scissorRect
    if (beforeRenderItem == nullptr || (beforeRenderItem->mScissor != currRenderItem->mScissor))
    {
        commandList->RSSetScissorRects(1, (D3D12_RECT *)&currRenderItem->mScissor);
        // commandList->RSSetScissorRects(0,nullptr);
    }
}

void D3DRender::D3DOpaqueRenderPass::BindMeshBufferIfNeeded(ID3D12GraphicsCommandList *commandList,
                                                            const Render::InternalRenderItem *beforeRenderItem,
                                                            const Render::InternalRenderItem *currRenderItem)
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

void D3DRender::D3DOpaqueRenderPass::BindShaderResources(ID3D12GraphicsCommandList *commandList,
                                                         const Render::InternalRenderItem *currRenderItem,
                                                         const D3DMainRenderPassInfo *currMatMainPass)
{
    // resourceBinding
    // 일단동일한 패스버퍼라도 루트파라미터의 인덱스가 달라지면 다시바인딩해야하니
    // 일단은 리소스는 항상 다시 바인딩
    for (const auto &resourceElement : currRenderItem->mBindingGpuResourceVector)
    {

        std::unordered_map<std::string, int>::const_iterator rootParameterIt =
            currMatMainPass->mShaderResourceRootParameterBindingInfo.find(resourceElement.mName);

        int rootParameterIndex = rootParameterIt->second;

        D3DGRM::D3DDescriptorHandle handle;
        switch (resourceElement.mType)
        {
        case Render::EShaderResourceType::eConstantBuffer:
        {

            D3DGRM::D3DGpuTexture *d3dConstantBuffer = (D3DGRM::D3DGpuTexture *)resourceElement.gpuResource;
            d3dConstantBuffer->GetDescriptorHandle(D3DGRM::ED3DResourceDescriptorType::eCBV, handle);
        }
        break;
        case Render::EShaderResourceType::eStructuredBuffer:
        case Render::EShaderResourceType::eTexture:
        {
            D3DGRM::D3DGpuResource *d3dResource = (D3DGRM::D3DGpuResource *)resourceElement.gpuResource;
            d3dResource->GetDescriptorHandle(D3DGRM::ED3DResourceDescriptorType::eSRV, handle);
        }
        break;
        case Render::EShaderResourceType::eSampler:

        {

            D3DGRM::D3DGpuResource *d3dResource = (D3DGRM::D3DGpuResource *)resourceElement.gpuResource;
            d3dResource->GetDescriptorHandle(D3DGRM::ED3DResourceDescriptorType::eSMP, handle);
        }

        break;
        }

        commandList->SetGraphicsRootDescriptorTable(rootParameterIndex, handle.mGpuDescriptorHandle);
    }
}

void D3DRender::D3DOpaqueRenderPass::DrawRenderItem(ID3D12GraphicsCommandList *commandList,
                                                    const Render::InternalRenderItem *currRenderItem)
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
