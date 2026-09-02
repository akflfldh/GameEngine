#include "BillboardRenderPass.h"
#include "AssetResolver.h"
#include "RenderUploadManager.h"
#include <D3DGpuResourceManager/GpuBufferContextSystem.h>
#include <D3DGpuResourceManager/IGpuResourceManager.h>
#include <RenderFrontend/RenderMaterialResolver.h>
#include <RenderFrontend/RenderPassGraph.h>
#include <RenderSystem/IRenderSystem.h>
#include <coreasset/Texture.h>
#include <unordered_map>

Render::BillboardRenderPass::BillboardRenderPass()
{
    SetPassName("BillboardRenderPass");
    SetBufferID(1);

    //  mGpuMaterialID = AssetResolver::GetInstance()->GetPassGpuMaterialID(BillboardMat);

    RenderMaterialContext rmc;
    rmc.mGeometryType = ERenderGeometryType::eBillboard;
    rmc.mTransparent = false;
    mGpuMaterialID = RenderMaterialResolver::GetInstance()->Resolve(rmc, ERenderPassType::eBillboard);
}
Render::BillboardRenderPass::~BillboardRenderPass() {}

void Render::BillboardRenderPass::AddToGraph(RenderPassGraph &renderPassGraph, const RenderPassSetUpData &passSetUpData)
{

    renderPassGraph.RegisterRenderPassCallback(
        GetName(),
        [pPass = this, passSetUpData](RenderPassGraphBuilder &builder)
        {
            RenderResourceDesc outputTargetDesc = {passSetUpData.mWindowWidth, passSetUpData.mWindowHeight,
                                                   GRM::ETextureFormat::eR8G8B8A8_UNORM,
                                                   GRM::ETextureUsage::eRenderTarget};
            builder.Create(pPass->mOutputTargetName, outputTargetDesc, EResourceState::eRenderTarget);

            RenderResourceDesc outputDepthStencilDesc = {passSetUpData.mWindowWidth, passSetUpData.mWindowHeight,
                                                         GRM::ETextureFormat::eD24_UNORM_S8_UINT,
                                                         GRM::ETextureUsage::eDepthStencil};

            builder.Create(pPass->mOutputDepthStencilName, outputDepthStencilDesc, EResourceState::eWriteDepthStencil);

            builder.SetRenderTarget(pPass->mOutputTargetName, pPass->GetName(), pPass->mClearRenderTarget,
                                    passSetUpData.mBackBufferClearColor);
            builder.SetDepthStencil(pPass->mOutputDepthStencilName, pPass->GetName(), false, 1.0f, true);
        },
        [pPass = this](const RenderPassExecuteContext &executeContext) { pPass->Execute(executeContext); });
    SetViewport({0, (float)passSetUpData.mWindowWidth, 0, (float)passSetUpData.mWindowHeight});
}

void Render::BillboardRenderPass::Execute(const RenderPassExecuteContext &renderPassExecuteContext)
{

    auto renderSystem = renderPassExecuteContext.renderSystem;

    std::vector<RenderItem> renderItemList = BuildRenderItem(renderPassExecuteContext);

    SetGlobalData(renderPassExecuteContext.mGlobalFrameData, renderPassExecuteContext);
    renderSystem->SetUpPassData(renderPassExecuteContext.mCommandContext,
                                mPassData); // target binding , 전역 pass buffer binding 등등이

    renderSystem->Draw(renderPassExecuteContext.mCommandContext, renderItemList);
}

void Render::BillboardRenderPass::SetGlobalData(const Core::GlobalFrameData &globalFrameData,
                                                const RenderPassExecuteContext &executeContext)
{

    BillboardPassConstantData passConstantData;
    passConstantData.mViewProj = globalFrameData.mViewProj;
    passConstantData.mCameraPosWorld = globalFrameData.mCameraPositionWorld;

    uint32_t bufferID = GetBufferID();

    // 여기는 공통으로 올리수있음

    GRM::IGpuResourceManager *gpuResourceManager = GRM::IGpuResourceManager::GetInstance();

    GRM::GpuBufferContextSystem *gpuBufferContextSystem = GRM::GpuBufferContextSystem::GetInstance();

    GRM::GpuConstantBufferContext *gpuBufferContext =
        static_cast<GRM::GpuConstantBufferContext *>(gpuBufferContextSystem->GetGpuBufferContext(bufferID));

    uint32_t bufferIndexOffset = gpuBufferContext->mAllocateRange.UseRange(1);

    uint32_t bufferSizeOffset = bufferIndexOffset * gpuBufferContext->mBufferDesc.mElementDataSize;

    gpuResourceManager->UploadBufferData(gpuBufferContext->mGpuBuffer, &passConstantData, sizeof(passConstantData), 1,
                                         bufferSizeOffset);

    BindingGpuResource bindingPassBufferGpuResource;
    bindingPassBufferGpuResource.gpuResource = gpuBufferContext->mGpuBuffer.getResource();
    bindingPassBufferGpuResource.mOffset = bufferSizeOffset;
    bindingPassBufferGpuResource.mType = EShaderResourceType::eConstantBuffer;

    mPassData.mGlobalPassBufferResouce = bindingPassBufferGpuResource;
    mPassData.mViewport = globalFrameData.mSceneViewport;
    mPassData.mViewport.TopLeftX = globalFrameData.mSceneViewport.TopLeftX;
    mPassData.mViewport.TopLeftY = globalFrameData.mSceneViewport.TopLeftY;

    mPassData.mRenderTarget = nullptr; // 기본적으로 후면버퍼를 사용하겠다 라는 의미.
    mPassData.mScissorRect.mLeft = globalFrameData.mSceneViewport.TopLeftX; //  globalFrameData.mSceneViewport.TopLeftX;
    mPassData.mScissorRect.mRight = mPassData.mScissorRect.mLeft + globalFrameData.mSceneViewport.Width;
    mPassData.mScissorRect.mTop = globalFrameData.mSceneViewport.TopLeftY; // globalFrameData.mSceneViewport.TopLeftY;
    mPassData.mScissorRect.mBottom = mPassData.mScissorRect.mTop + globalFrameData.mSceneViewport.Height;
}

std::vector<Render::RenderItem> Render::BillboardRenderPass::BuildRenderItem(
    const RenderPassExecuteContext &executeContext)
{

    std::vector<Render::RenderItem> renderItemList;
    const std::vector<BillboardRenderCommand> &cmdList = executeContext.mBillboardRenderCommandList;
    auto gpuBufferContextSystem = GRM::GpuBufferContextSystem::GetInstance();
    auto assetResolver = AssetResolver::GetInstance();
    // depth test - true : false 분류
    // table [texture, cmd index list ] 분류

    std::unordered_map<CoreAsset::AssetID, std::vector<size_t>> table;

    for (size_t i = 0; i < cmdList.size(); ++i)
    {
        const BillboardRenderCommand &cmd = cmdList[i];

        if (cmd.mTexture == nullptr)
        {
            // 렌더하지않는다.
            continue; //
        }
        table[cmd.mTexture->GetID()].push_back(i);
    }

    uint32_t objectStructuredBufferOffset = 0;
    for (const auto &e : table)
    {
        const auto &cmdIndexList = e.second;

        RenderItem renderItem;

        renderItem.mMeshItem.mVertexNum = cmdIndexList.size();
        renderItem.mPrimitiveTopology = EPrimitiveTopology::ePointList;
        renderItem.mDrawType = EDrawType::eVertex;
        renderItem.mMaterialID = mGpuMaterialID;
        BuildObjectStructuredBuffer(renderItem, cmdList, cmdIndexList, objectStructuredBufferOffset);

        // 텍스처 리소스 바인딩
        BuildRenderItemTexGpuResource(renderItem, cmdList[cmdIndexList[0]].mTexture);
        // 렌더아이템 생성

        renderItemList.push_back(renderItem);
    }

    return renderItemList;
}

void Render::BillboardRenderPass::BuildObjectStructuredBuffer(RenderItem &renderItem,
                                                              const std::vector<BillboardRenderCommand> &cmdList,
                                                              const std::vector<size_t> &cmdIndexList,
                                                              uint32_t &objectStructuredBufferOffset)
{
    auto gpuResourceManager = GRM::IGpuResourceManager::GetInstance();
    auto gpuBufferContextSystem = GRM::GpuBufferContextSystem::GetInstance();

    BindingGpuResource objectStructedBufferBinidngGpuResource;
    objectStructedBufferBinidngGpuResource.mType = EShaderResourceType::eStructuredBuffer;

    GRM::GpuStructuredBufferContext *gpuStructuredBufferContext = static_cast<GRM::GpuStructuredBufferContext *>(
        gpuBufferContextSystem->GetGpuBufferContext(AssetResolver::GetInstance()->GetBillboardStructuredGpuBufferID()));

    gpuStructuredBufferContext->mGpuBuffersPerFrame[gpuStructuredBufferContext->mCurrFrameIndex];

    objectStructedBufferBinidngGpuResource.gpuResource =
        gpuStructuredBufferContext->mGpuBuffersPerFrame[gpuStructuredBufferContext->mCurrFrameIndex].getResource();
    objectStructedBufferBinidngGpuResource.mOffset = objectStructuredBufferOffset * sizeof(BillboardData);

    uint32_t buffeSizeOffset = objectStructedBufferBinidngGpuResource.mOffset;

    std::vector<BillboardData> dataVec(cmdIndexList.size());
    for (size_t i = 0; i < cmdIndexList.size(); ++i)
    {
        size_t cmdIndex = cmdIndexList[i];
        const auto &cmd = cmdList[cmdIndex];
        // 구조적버퍼 채우기,바인딩
        RenderUploadManager::GetInstance()->UploadBillboardData(cmd, dataVec[i]);
    }

    gpuResourceManager->UploadBufferData(objectStructedBufferBinidngGpuResource.gpuResource, dataVec.data(),
                                         sizeof(BillboardData), dataVec.size(), buffeSizeOffset);

    renderItem.mBindingGpuBufferResourceVector.push_back(objectStructedBufferBinidngGpuResource);

    objectStructuredBufferOffset += cmdIndexList.size();
}

void Render::BillboardRenderPass::BuildRenderItemTexGpuResource(RenderItem &renderItem, CoreAsset::Texture *texture)
{

    auto assetResolver = AssetResolver::GetInstance();
    BindingGpuResource bindingTexResource;

    GRM::IGpuResource *gpuResource = nullptr;
    if (texture == nullptr)
    {
        gpuResource = assetResolver->GetDefaultTexture().getResource();
    }
    else
    {

        assetResolver->RequestResolveAsset(texture);
        gpuResource = assetResolver->GetGpuResource(texture).getResource();
    }

    bindingTexResource.gpuResource = gpuResource;
    bindingTexResource.mType = EShaderResourceType::eTexture;

    renderItem.mBindingGpuTexResourceVector.push_back(bindingTexResource);
}
