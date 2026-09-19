#include "RenderPassUI.h"
#include <CoreAsset/AssetManager.h>
#include <CoreAsset/Texture.h>
#include <D3DGpuResourceManager/GpuBufferContextSystem.h>
#include <D3DGpuResourceManager/GpuSamplerSystem.h>
#include <D3DGpuResourceManager/IGpuResourceManager.h>
#include <RenderFrontend/AssetResolver.h>
#include <RenderFrontend/RenderMaterialResolver.h>
#include <RenderFrontend/RenderPassGraph.h>
#include <RenderFrontend/UIRenderItemBuilder.h>
#include <UiSystem/UIElement.h>
#include <UiSystem/UIRenderableComponent.h>

Render::RenderPassUI::RenderPassUI()
{
    SetPassName("MainUIPass");
    SetBufferID(1);
    mUseDefaultViewport = false;
}

void Render::RenderPassUI::AddToGraph(RenderPassGraph &renderPassGraph, const RenderPassSetUpData &passSetUpData)
{

    renderPassGraph.RegisterRenderPassCallback(
        GetName(),
        [pPass = this, passSetUpData](RenderPassGraphBuilder &builder)
        {
            // builder.Write("BackBuffer", "Opaque_UIMainPass", EResourceState::ePresent);
            RenderResourceDesc outputTargetDesc = {.mWidth = passSetUpData.mWindowWidth,
                                                   .mHeight = passSetUpData.mWindowHeight,
                                                   .mResourceFormat = GRM::ETextureFormat::eR8G8B8A8_UNORM,
                                                   .mRtvFormat = GRM::ETextureFormat::eR8G8B8A8_UNORM,
                                                   .mDsvFormat = std::nullopt,
                                                   .mSrvFormat = GRM::ETextureFormat::eR8G8B8A8_UNORM,
                                                   .mUsage = GRM::ETextureUsage::eRenderTarget};
            builder.Create(pPass->mOutputTargetName, outputTargetDesc, EResourceState::eRenderTarget);

            RenderResourceDesc outputDepthStencilDesc = {.mWidth = passSetUpData.mWindowWidth,
                                                         .mHeight = passSetUpData.mWindowHeight,
                                                         .mResourceFormat = GRM::ETextureFormat::eD24_UNORM_S8_UINT,
                                                         .mRtvFormat = std::nullopt,
                                                         .mDsvFormat = GRM::ETextureFormat::eD24_UNORM_S8_UINT,
                                                         .mSrvFormat = std::nullopt,
                                                         .mUsage = GRM::ETextureUsage::eDepthStencil};
            builder.Create(pPass->mOutputDepthStencilName, outputDepthStencilDesc, EResourceState::eWriteDepthStencil);

            builder.SetRenderTarget(pPass->mOutputTargetName, pPass->GetName(), pPass->mClearRenderTarget, nullptr);
            builder.SetDepthStencil(
                pPass->mOutputDepthStencilName, pPass->GetName(),
                DepthStencilClearDesc{.mClearDepth = false, .mClearStencil = false, .mDepth = 1.0f, .mStencil = 0},
                true);
        },
        [pPass = this](const RenderPassExecuteContext &executeContext) { pPass->Execute(executeContext); });

    SetViewport({0, (float)passSetUpData.mWindowWidth, 0, (float)passSetUpData.mWindowHeight});
}

void Render::RenderPassUI::Execute(const RenderPassExecuteContext &renderPassExecuteContext)
{

    Render::IRenderSystem *renderSystem = Render::IRenderSystem::GetInstance();

    FrameContext frameContext;
    SetGlobalData(renderPassExecuteContext.mUIGlobalFrameData, frameContext);

    std::vector<Render::RenderItem> renderItemList = BuildRenderItem(renderPassExecuteContext);

    if (renderItemList.size() == 0)
        return;

    renderSystem->SetUpPassData(renderPassExecuteContext.mCommandContext, frameContext);

    renderSystem->Draw(renderPassExecuteContext.mCommandContext, renderItemList);
}

void Render::RenderPassUI::SetGlobalData(const Core::GlobalFrameData &globalFrameData, FrameContext &oFrameContext)
{

    /*
    viewport
    pass constant buffer

    render target


    scissor rect
    */

    ConstantPass passConstantData;
    passConstantData.mViewProj = globalFrameData.mViewProj;
    passConstantData.mScreenHeight = globalFrameData.mSceneViewport.Height;
    passConstantData.mScreenWidth = globalFrameData.mSceneViewport.Width;

    // TODO buffer ID가 올바르지못한값이 생성자에서 설정해야함
    uint32_t bufferID = static_cast<uint8_t>(EDefaultGpuBufferType::eConstantPass256);
    //    GetBufferID();

    GRM::IGpuResourceManager *gpuResourceManager = GRM::IGpuResourceManager::GetInstance();

    GRM::GpuBufferContextSystem *gpuBufferContextSystem = GRM::GpuBufferContextSystem::GetInstance();

    GRM::GpuConstantBufferContext *gpuBufferContext =
        static_cast<GRM::GpuConstantBufferContext *>(gpuBufferContextSystem->GetGpuBufferContext(bufferID));

    uint32_t bufferIndexOffset = gpuBufferContext->mAllocateRange.UseRange(1);
    uint32_t bufferSizeOffset = bufferIndexOffset * gpuBufferContext->mBufferDesc.mElementDataSize;
    gpuResourceManager->UploadBufferData(gpuBufferContext->mGpuBuffer, &passConstantData, sizeof(passConstantData), 1,
                                         bufferSizeOffset);
    oFrameContext.mGlobalPassBufferResouce.gpuResource = gpuBufferContext->mGpuBuffer.getResource();
    oFrameContext.mGlobalPassBufferResouce.mOffset = bufferSizeOffset;
    oFrameContext.mGlobalPassBufferResouce.mType = Render::EShaderResourceType::eConstantBuffer;
    oFrameContext.mViewport = globalFrameData.mSceneViewport;

    oFrameContext.mViewport.TopLeftX = globalFrameData.mSceneViewport.TopLeftX;
    oFrameContext.mViewport.TopLeftY = globalFrameData.mSceneViewport.TopLeftY;

    oFrameContext.mRenderTarget = nullptr;
    oFrameContext.mScissorRect.mLeft = oFrameContext.mViewport.TopLeftX;
    // globalFrameData.mSceneViewport.TopLeftX;
    oFrameContext.mScissorRect.mRight = oFrameContext.mScissorRect.mLeft + globalFrameData.mSceneViewport.Width;
    oFrameContext.mScissorRect.mTop = oFrameContext.mViewport.TopLeftY; // globalFrameData.mSceneViewport.TopLeftY;
    oFrameContext.mScissorRect.mBottom = oFrameContext.mScissorRect.mTop + globalFrameData.mSceneViewport.Height;

    mGlobalScissorRect = oFrameContext.mScissorRect;
}

std::vector<Render::RenderItem> Render::RenderPassUI::BuildRenderItem(
    const RenderPassExecuteContext &renderPassExecuteContext)
{
    auto renderMaterialResolver = RenderMaterialResolver::GetInstance();
    const std::vector<UIRenderCommand> &renderCommandList = renderPassExecuteContext.mUIRenderCommandList;
    const Core::GlobalFrameData &globalFrameData = renderPassExecuteContext.mUIGlobalFrameData;

    GRM::GpuBufferContextSystem *gpuBufferContextSystem = GRM::GpuBufferContextSystem::GetInstance();
    Render::UIRenderItemBuilder *uiRenderItemBuilder = Render::UIRenderItemBuilder::GetInstance();

    const std::vector<UI::UIVertex> &vertexBuffer = *renderPassExecuteContext.mUIVertexBuffer;
    const std::vector<uint32_t> &indexBuffer = *renderPassExecuteContext.mUIIndexBuffer;

    GRM::GRMPtr vertexBufferPtr;
    GRM::GRMPtr indexBufferPtr;
    uiRenderItemBuilder->UploadStart(vertexBufferPtr, indexBufferPtr);

    CoreAsset::AssetID prevTexID = NoneAssetID;
    Render::MaterialID prevGpuMatID = MaterialIDNone;
    bool prevUseScissor = false;
    SRECT prevScissorRect = mGlobalScissorRect;
    Render::RenderItem *currentRenderItem = nullptr;

    std::vector<Render::RenderItem> renderItemVec;
    std::vector<UIRenderCommand> currentBatchCommands;

    for (int i = 0; i < renderCommandList.size(); ++i)
    {
        const UIRenderCommand &renderCommand = renderCommandList[i];

        bool bNeedNewBatch = false;
        CoreAsset::AssetID currTexID = renderCommand.mMatSnapshot.mTextureAssetID;
        Render::MaterialID currGpuMatID = MaterialIDNone;

        switch (renderCommand.mRole)
        {
        case UI::UIRenderRole::eImage:
            currGpuMatID =
                renderMaterialResolver->ResolveSystemGpuMaterial(ESystemMaterialRole::DefaultUI);
            break;
        case UI::UIRenderRole::eFont:
            currGpuMatID =
                renderMaterialResolver->ResolveSystemGpuMaterial(ESystemMaterialRole::DefaultUIFont);
            break;
        }

        if (currGpuMatID == MaterialIDNone)
            continue;

        CoreAsset::Texture *texture = nullptr;
        CoreAsset::AssetManager *assetManager = CoreAsset::AssetManager::GetInstance();
        if (currTexID != NoneAssetID)
        {
            texture = assetManager->GetAsset<CoreAsset::Texture>(currTexID).As<CoreAsset::Texture>();
        }
        if (texture == nullptr)
        {
            texture = assetManager->GetDefaultAsset(CoreAsset::EAssetType::eTexture).As<CoreAsset::Texture>();
            currTexID = texture != nullptr ? texture->GetID() : NoneAssetID;
        }
        if (texture == nullptr)
            continue;

        SRECT currScissorRect = renderCommand.mScissorRect;
        bool currUseScissor = renderCommand.mUseScissorRect;
        Render::ScissorRect sicssorRect;
        if (currUseScissor)
        {

            sicssorRect = ConvertWorldToScreenRect(currScissorRect, globalFrameData);
        }
        else
        {
            sicssorRect = mGlobalScissorRect;
        }

        //  Render::ScissorRect sicssorRect = ConvertWorldToScreenRect(currScissorRect, globalFrameData);
        if ((int)sicssorRect.mLeft == (int)sicssorRect.mRight || ((int)sicssorRect.mTop == (int)sicssorRect.mBottom))
        {
            // render 할 필요가없다.
            continue;
        }

        if (currentRenderItem == nullptr)
        {
            // 첫번쨰는 무조건 새 배치 시작
            bNeedNewBatch = true;
        }
        else
        {

            if (prevTexID != currTexID)
                bNeedNewBatch = true;
            else if (prevGpuMatID != currGpuMatID)
                bNeedNewBatch = true;
            else if (prevUseScissor != currUseScissor)
                bNeedNewBatch = true;
            else if (prevScissorRect != sicssorRect)
                bNeedNewBatch = true;
        }

        if (bNeedNewBatch)
        {
            if (currentRenderItem)
            {
                // 새로운배치시작전 이전배치 마무리

                uint32_t vertexNum = 0;

                uiRenderItemBuilder->UploadVertexBuffer(
                    currentBatchCommands, currentRenderItem->mMeshItem.mVertexOffset,
                    currentRenderItem->mMeshItem.mIndexOffset, vertexNum, currentRenderItem->mMeshItem.mIndexNum,
                    vertexBuffer, indexBuffer);

                currentRenderItem->mMeshItem.mVertexBuffer = vertexBufferPtr.getResource();
                currentRenderItem->mMeshItem.mIndexBuffer = indexBufferPtr.getResource();
                currentRenderItem->mDrawType = Render::EDrawType::eIndex;
            }

            // 새로운 배치 시작
            renderItemVec.push_back({});
            currentRenderItem = &renderItemVec.back();
            currentRenderItem->bUseScissor = currUseScissor;
            currentRenderItem->mScissor = sicssorRect;
            currentRenderItem->mMaterialID = currGpuMatID;
            currentBatchCommands.clear();

            prevTexID = currTexID;
            prevGpuMatID = currGpuMatID;
            prevScissorRect = sicssorRect;
            prevUseScissor = currUseScissor;

            SetUpRenderItemShaderResource(*currentRenderItem, currTexID);
        }

        currentBatchCommands.push_back(renderCommand);
    }

    // 마지막 배치
    if (currentRenderItem && !currentBatchCommands.empty())
    {
        uint32_t vertexOffset = 0;
        uint32_t indexOffset = 0;
        uint32_t vertexNum = 0;
        uint32_t indexNum = 0;

        // TODO vertex,index 출력매개변수가 리턴되지않음
        uiRenderItemBuilder->UploadVertexBuffer(currentBatchCommands, vertexOffset, indexOffset, vertexNum, indexNum,
                                                vertexBuffer, indexBuffer);

        currentRenderItem->mMeshItem.mVertexBuffer = vertexBufferPtr.getResource();
        currentRenderItem->mMeshItem.mIndexBuffer = indexBufferPtr.getResource();

        currentRenderItem->mMeshItem.mVertexOffset = vertexOffset;
        currentRenderItem->mMeshItem.mIndexOffset = indexOffset;
        currentRenderItem->mMeshItem.mIndexNum = indexNum;

        currentRenderItem->mDrawType = Render::EDrawType::eIndex;

        // uiRenderItemBuilder->UploadVertexBuffer(currentBatchCommands, vertexOffset, indexOffset, vertexNum,
        // indexNum);
    }

    return renderItemVec;
}

SRECT Render::RenderPassUI::ConvertWorldToScreenRect(const SRECT &rect, const Core::GlobalFrameData &globalFrameData)
{
    //  glm::mat4 viewProj = glm::transpose(globalFrameData.mViewProj);
    CoreMath::Matrix4X4 viewProj = globalFrameData.mViewProj.GetTransposed();

    long w = globalFrameData.mSceneViewport.Width;
    long h = globalFrameData.mSceneViewport.Height;
    long viewportOffsetX = globalFrameData.mSceneViewport.TopLeftX;
    long viewportOffsetY = globalFrameData.mSceneViewport.TopLeftY;

    CoreMath::Vector4 worldLT = {rect.mLeft, rect.mTop, 0, 1.0f};
    CoreMath::Vector4 worldRB = {rect.mRight, rect.mBottom, 0, 1.0f};

    CoreMath::Vector4 clipLT = viewProj * worldLT;
    CoreMath::Vector4 clipRB = viewProj * worldRB;

    CoreMath::Vector4 ndcLT = clipLT / clipLT.W;
    CoreMath::Vector4 ndcRB = clipRB / clipRB.W;

    float rawLeft = (ndcLT.X + 1.0f) * 0.5f * w;
    float rawRight = (ndcRB.X + 1.0f) * 0.5f * w;
    float rawTop = (1.0f - ndcLT.Y) * 0.5f * h;
    float rawBottom = (1.0f - ndcRB.Y) * 0.5f * h;

    // 5. Min/Max 정렬 (회전이나 좌표계에 따라 뒤집힐 수 있으므로)
    long finalLeft = std::floor(std::min(rawLeft, rawRight) + viewportOffsetX);
    long finalRight = std::ceil(std::max(rawLeft, rawRight) + viewportOffsetX);
    long finalTop = std::floor(std::min(rawTop, rawBottom) + viewportOffsetY);
    long finalBottom = std::ceil(std::max(rawTop, rawBottom) + viewportOffsetY);

    long maxW = viewportOffsetX + w;
    long maxH = viewportOffsetY + h;

    finalLeft = std::max(viewportOffsetX, finalLeft);
    finalTop = std::max(viewportOffsetY, finalTop);

    // 우측/하단은 왼쪽/상단보다 작아질 수 없게 한번 더 방어
    finalRight = std::max(finalLeft, std::min(maxW, finalRight));
    finalBottom = std::max(finalTop, std::min(maxH, finalBottom));

    return SRECT{(float)finalLeft, (float)finalRight, (float)finalTop, (float)finalBottom};
}

void Render::RenderPassUI::SetUpRenderItemShaderResource(RenderItem &renderItem,
                                                         CoreAsset::AssetID textureAssetID)
{
    BuildRenderItemTexGpuResources(textureAssetID, renderItem.mBindingGpuTexResourceVector);

    renderItem.mInstance.mInstanceCount = 1;
}

void Render::RenderPassUI::BuildRenderItemTexGpuResources(CoreAsset::AssetID textureAssetID,
                                                          std::vector<BindingGpuResource> &bindingGpuResourceVector)
{
    CoreAsset::AssetManager *assetManager = CoreAsset::AssetManager::GetInstance();
    CoreAsset::Texture *texture = nullptr;

    if (textureAssetID != NoneAssetID)
    {
        texture = assetManager->GetAsset<CoreAsset::Texture>(textureAssetID).As<CoreAsset::Texture>();
    }
    if (texture == nullptr)
    {
        texture = assetManager->GetDefaultAsset(CoreAsset::EAssetType::eTexture).As<CoreAsset::Texture>();
    }
    if (texture == nullptr)
        return;

    mAssetResolver->RequestResolveAsset(texture);

    Render::BindingGpuResource bindingGpuResource;
    bindingGpuResource.gpuResource = mAssetResolver->GetGpuResource(texture).getResource();
    bindingGpuResource.mType = EShaderResourceType::eTexture;
    bindingGpuResourceVector.push_back(std::move(bindingGpuResource));
}
