#include "RenderPassUI.h"
#include <CoreAsset/Material.h>
#include <D3DGpuResourceManager/GpuBufferContextSystem.h>
#include <D3DGpuResourceManager/GpuSamplerSystem.h>
#include <D3DGpuResourceManager/IGpuResourceManager.h>
#include <RenderFrontend/AssetResolver.h>
#include <RenderFrontend/RenderPassGraph.h>
#include <RenderFrontend/UIRenderItemBuilder.h>
#include <RenderSystem/IMaterialManager.h>
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
            RenderResourceDesc outputTargetDesc = {passSetUpData.mWindowWidth, passSetUpData.mWindowHeight,
                                                   GRM::ETextureFormat::eR8G8B8A8_UNORM,
                                                   GRM::ETextureUsage::eRenderTarget};
            builder.Create(pPass->mOutputTargetName, outputTargetDesc, EResourceState::eRenderTarget);

            RenderResourceDesc outputDepthStencilDesc = {passSetUpData.mWindowWidth, passSetUpData.mWindowHeight,
                                                         GRM::ETextureFormat::eD24_UNORM_S8_UINT,
                                                         GRM::ETextureUsage::eDepthStencil};
            builder.Create(pPass->mOutputDepthStencilName, outputDepthStencilDesc, EResourceState::eWriteDepthStencil);

            builder.SetRenderTarget(pPass->mOutputTargetName, pPass->GetName(), pPass->mClearRenderTarget, nullptr);
            builder.SetDepthStencil(pPass->mOutputDepthStencilName, pPass->GetName(), false, 1.0f, true);
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
    uint32_t bufferID = GetBufferID();

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

    oFrameContext.mViewport.TopLeftX = 0;
    oFrameContext.mViewport.TopLeftY = 0;

    oFrameContext.mRenderTarget = nullptr;
    oFrameContext.mScissorRect.mLeft = 0;
    // globalFrameData.mSceneViewport.TopLeftX;
    oFrameContext.mScissorRect.mRight = oFrameContext.mScissorRect.mLeft + globalFrameData.mSceneViewport.Width;
    oFrameContext.mScissorRect.mTop = 0; // globalFrameData.mSceneViewport.TopLeftY;
    oFrameContext.mScissorRect.mBottom = oFrameContext.mScissorRect.mTop + globalFrameData.mSceneViewport.Height;

    mGlobalScissorRect = oFrameContext.mScissorRect;
}

std::vector<Render::RenderItem> Render::RenderPassUI::BuildRenderItem(
    const RenderPassExecuteContext &renderPassExecuteContext)
{

    const std::vector<UIRenderCommand> &renderCommandList = renderPassExecuteContext.mUIRenderCommandList;
    const Core::GlobalFrameData &globalFrameData = renderPassExecuteContext.mUIGlobalFrameData;

    GRM::GpuBufferContextSystem *gpuBufferContextSystem = GRM::GpuBufferContextSystem::GetInstance();
    Render::UIRenderItemBuilder *uiRenderItemBuilder = Render::UIRenderItemBuilder::GetInstance();

    const std::vector<UI::UIVertex> &vertexBuffer = *renderPassExecuteContext.mUIVertexBuffer;
    const std::vector<uint32_t> &indexBuffer = *renderPassExecuteContext.mUIIndexBuffer;

    GRM::GRMPtr vertexBufferPtr;
    GRM::GRMPtr indexBufferPtr;
    uiRenderItemBuilder->UploadStart(vertexBufferPtr, indexBufferPtr);

    CoreAsset::AssetID prevMatID = 0;
    bool prevUseScissor = false;
    SRECT prevScissorRect = mGlobalScissorRect;
    Render::RenderItem *currentRenderItem = nullptr;

    std::vector<Render::RenderItem> renderItemVec;
    std::vector<UIRenderCommand> currentBatchCommands;

    for (int i = 0; i < renderCommandList.size(); ++i)
    {
        const UIRenderCommand &renderCommand = renderCommandList[i];

        // 배치를 이어갈수있는가 ? 새로운배치가 필요한가?
        // mat , scissorRect로 판정한다 .

        bool bNeedNewBatch = false;
        CoreAsset::AssetID currMatID = renderCommand.mUIMaterialID;
        //        renderCommand.mUIMaterial->GetID();

        //  SRECT currScissorRect = renderProxy->mRenderableComponent->GetOwnerUIElement()->GetSicssorRectRegion();
        SRECT currScissorRect = renderCommand.mScissorRect;
        //  bool currUseScissor = renderProxy->mRenderableComponent->GetOwnerUIElement()->GetUseScissorRect();
        bool currUseScissor = renderCommand.mUseScissorRect;
        if (currUseScissor == false)
        {
            currScissorRect = mGlobalScissorRect;
        }

        Render::ScissorRect sicssorRect = ConvertWorldToScreenRect(currScissorRect, globalFrameData);
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

            if (prevMatID != currMatID)
                bNeedNewBatch = true;
            else if (prevUseScissor != currUseScissor)
                bNeedNewBatch = true;
            else if (prevUseScissor && (prevScissorRect != currScissorRect))
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
            currentRenderItem->mMaterialID = renderCommand.mGpuMaterialID;
            currentBatchCommands.clear();

            prevMatID = currMatID;
            prevScissorRect = currScissorRect;
            prevUseScissor = currUseScissor;

            SetUpRenderItemShaderResource(renderPassExecuteContext, *currentRenderItem, renderCommand);
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
    long viewportOffsetX = 0;
    // globalFrameData.mSceneViewport.TopLeftX;
    long viewportOffsetY = 0; // globalFrameData.mSceneViewport.TopLeftY;

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

void Render::RenderPassUI::SetUpRenderItemShaderResource(const RenderPassExecuteContext &renderPassExecuteContext,
                                                         RenderItem &renderItem, const UIRenderCommand &renderCommand)
{
    Render::IMaterialManager *gpuMaterialManager = Render::IMaterialManager::GetInstance();
    AssetResolver *assetResolver = AssetResolver::GetInstance();

    const MaterialRenderSnapshot &materialRenderSnapshot =
        renderPassExecuteContext.mUIMaterialRenderSnapshotTable.find(renderCommand.mUIMaterialID)->second;

    BuildRenderItemTexGpuResources(materialRenderSnapshot, renderItem.mBindingGpuTexResourceVector);

    renderItem.mInstance.mInstanceCount = 1;
}

void Render::RenderPassUI::BuildRenderItemTexGpuResources(const MaterialRenderSnapshot &materialRenderSnapshot,
                                                          std::vector<BindingGpuResource> &bindingGpuResourceVector)
{

    // albedo
    std::vector<CoreAsset::Texture *> texList;
    for (auto &tex : materialRenderSnapshot.mAlbedoMapList)
    {

        texList.push_back(tex);
    }

    // normal map
    // if (material->has)
    //  texList.push_back(static_cast<CoreAsset::Texture *>(material->GetNormalTexResource().mTexture.Get()));

    // add bindingGpuResource
    for (auto tex : texList)
    {
        Render::BindingGpuResource bindingGpuResource;
        // bindingGpuResource.gpuResource = mAssetResolver->GetGpuResource();

        mAssetResolver->RequestResolveAsset(tex);
        bindingGpuResource.gpuResource = mAssetResolver->GetGpuResource(tex).getResource();

        bindingGpuResource.mType = EShaderResourceType::eTexture;
        bindingGpuResourceVector.push_back(std::move(bindingGpuResource));
    }
}