#include "ShadowRenderPass.h"
#include <CoreAsset/StaticMesh.h>
#include <D3DGpuResourceManager/GpuBufferContextSystem.h>
#include <D3DGpuResourceManager/IGpuResourceManager.h>
#include <RenderFrontend/AssetResolver.h>
#include <RenderFrontend/RenderFrontendType.h>
#include <RenderFrontend/RenderMaterialResolver.h>
#include <RenderFrontend/RenderPassGraph.h>
#include <RenderFrontend/RenderUploadManager.h>
#include <RenderSystem/IRenderSystem.h>
Render::ShadowRenderPass::ShadowRenderPass()
{

    SetPassName("ShadowRenderPass");
    mUseDefaultViewport = false;
    SetViewport({0, 4096, 0, 4096});
    RenderMaterialContext rmc;
    rmc.mGeometryType = ERenderGeometryType::eStaticMesh;
    rmc.mTransparent = false;
    mGpuMaterialID = RenderMaterialResolver::GetInstance()->Resolve(rmc, ERenderPassType::eShadow);
}

Render::ShadowRenderPass::~ShadowRenderPass() {}

void Render::ShadowRenderPass::AddToGraph(RenderPassGraph &renderPassGraph, const RenderPassSetUpData &passSetUpData)
{

    renderPassGraph.RegisterRenderPassCallback(
        GetName(),
        [pPass = this, passSetUpData](RenderPassGraphBuilder &builder)
        {
            /* RenderResourceDesc outputTargetDesc = {.mWidth = passSetUpData.mWindowWidth,
                                                    .mHeight = passSetUpData.mWindowHeight,
                                                    .mResourceFormat = GRM::ETextureFormat::eR8G8B8A8_UNORM,
                                                    .mRtvFormat = GRM::ETextureFormat::eR8G8B8A8_UNORM,
                                                    .mDsvFormat = std::nullopt,
                                                    .mSrvFormat = GRM::ETextureFormat::eR8G8B8A8_UNORM,
                                                    .mUsage = GRM::ETextureUsage::eRenderTarget};
             builder.Create(pPass->mOutputTargetName, outputTargetDesc, EResourceState::eRenderTarget);*/

            RenderResourceDesc outputDepthStencilDesc = {.mWidth = (uint32_t)pPass->GetViewport().mRight,
                                                         // passSetUpData.mWindowWidth,
                                                         .mHeight = (uint32_t)pPass->GetViewport().mBottom,
                                                         // passSetUpData.mWindowHeight,
                                                         .mResourceFormat = GRM::ETextureFormat::eR32_TYPELESS,
                                                         .mRtvFormat = std::nullopt,
                                                         .mDsvFormat = GRM::ETextureFormat::eD32_FLOAT,
                                                         .mSrvFormat = GRM::ETextureFormat::eR32_FLOAT,
                                                         .mUsage = GRM::ETextureUsage::eDepthStencilShaderResource};

            builder.Create(pPass->mOutputDepthStencilName, outputDepthStencilDesc, EResourceState::eWriteDepthStencil);

            // RenderTarget 설정은없다.
            /*       builder.SetRenderTarget(pPass->mOutputTargetName, pPass->GetName(), pPass->mClearRenderTarget,
                                           passSetUpData.mBackBufferClearColor);*/

            builder.SetDepthStencil(
                pPass->mOutputDepthStencilName, pPass->GetName(),
                DepthStencilClearDesc{.mClearDepth = true, .mClearStencil = false, .mDepth = 1.0f, .mStencil = 0},
                true);
        },
        [pPass = this](const RenderPassExecuteContext &executeContext) { pPass->Execute(executeContext); });
}

void Render::ShadowRenderPass::Execute(const RenderPassExecuteContext &renderPassExecuteContext)
{

    // 마지막에 작성
    if (renderPassExecuteContext.mDirectonalShadowRenderData.mEnabled == false)
        return;

    auto renderSystem = renderPassExecuteContext.renderSystem;
    SetGlobalData(renderPassExecuteContext);

    std::vector<RenderItem> renderItemList = BuildRenderItem(renderPassExecuteContext);

    renderSystem->Draw(renderPassExecuteContext.mCommandContext, renderItemList);
}

void Render::ShadowRenderPass ::SetGlobalData(const RenderPassExecuteContext &executeContext)
{

    auto renderSystem = executeContext.renderSystem;

    // pass buffer
    ShadowPassBuffer passConstantData;
    passConstantData.mViewProj = executeContext.mDirectonalShadowRenderData.mViewProj;

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

    BindingGpuResource bindingPassBufferGpuResource;
    bindingPassBufferGpuResource.gpuResource = gpuBufferContext->mGpuBuffer.getResource();
    bindingPassBufferGpuResource.mOffset = bufferSizeOffset;
    bindingPassBufferGpuResource.mType = EShaderResourceType::eConstantBuffer;

    mPassData.mGlobalPassBufferResouce = bindingPassBufferGpuResource;
    mPassData.mViewport.TopLeftX = GetViewport().mLeft;
    mPassData.mViewport.TopLeftY = GetViewport().mTop;
    mPassData.mViewport.Width = GetViewport().mRight - mPassData.mViewport.TopLeftX;
    mPassData.mViewport.Height = GetViewport().mBottom - mPassData.mViewport.TopLeftY;

    mPassData.mRenderTarget = nullptr;
    mPassData.mScissorRect.mLeft = mPassData.mViewport.TopLeftX;
    mPassData.mScissorRect.mRight = mPassData.mViewport.TopLeftX + mPassData.mViewport.Width;
    mPassData.mScissorRect.mTop = mPassData.mViewport.TopLeftY;
    mPassData.mScissorRect.mBottom = mPassData.mViewport.TopLeftY + mPassData.mViewport.Height;

    renderSystem->SetUpPassData(executeContext.mCommandContext,
                                mPassData); // target binding , 전역 pass buffer binding 등등이
}

std::vector<Render::RenderItem> Render::ShadowRenderPass::BuildRenderItem(
    const RenderPassExecuteContext &executeContext)
{

    // opaque 오브젝트

    // vertex만

    std::vector<Render::RenderItem> renderItemVec;

    for (const auto &command : executeContext.mOpaqueStaticMeshRenderCommandList)
    {

        // staticMesh가 지정되지않아서 무시
        if (command.mStaticMesh == nullptr)
            continue;

        Render::RenderItem renderItem;

        // BuildMeshData();
        if (!BuildRenderItemMeshData(command, renderItem))
        {
            continue;
            // 렌더아이템의 메시구축실패 - >렌더하지않는다.
        }

        /*      uint32_t materialHandle = command.mMaterialHandle;
              auto matIt = executeContext.mMaterialRenderSnapshotTable.find(materialHandle);

              const MaterialRenderSnapshot &materialRenderSnapshot = matIt->second;

              RenderMaterialContext rmc;
              rmc.mGeometryType = ERenderGeometryType::eStaticMesh;
              rmc.mTransparent = false;
              rmc.mShadingModel = materialRenderSnapshot.mShadingModel;*/

        renderItem.mMaterialID = mGpuMaterialID;

        renderItem.mScissor = mPassData.mScissorRect;

        // binding gpu resource (모두 object단위 gpu resource)
        // buffer, tex

        // gpu material - binding object resource list
        BuildRenderItemBufferGpuResources(command, renderItem.mBindingGpuBufferResourceVector);

        renderItemVec.push_back(std::move(renderItem));
    }

    return renderItemVec;
}

bool Render::ShadowRenderPass::BuildRenderItemMeshData(const Render::StaticMeshRenderCommnad &command,
                                                       Render::RenderItem &renderItem)
{

    const std::vector<CoreAsset::SubMesh> &subMeshVector = command.mStaticMesh->GetSubMeshVector();
    const CoreAsset::SubMesh subMesh = subMeshVector[command.mSubMeshIndex];

    // instance
    renderItem.mInstance.mInstanceCount = 1;
    renderItem.mInstance.mInstanceBufferOffset = 0;

    // draw type
    renderItem.mDrawType = EDrawType::eIndex;

    // mesh
    renderItem.mMeshItem.mIndexNum = subMesh.mIndexNum;
    renderItem.mMeshItem.mIndexOffset = subMesh.mIndexOffset;
    renderItem.mMeshItem.mVertexOffset = subMesh.mVertexOffset;

    Render::MeshGpuResourceContext meshGpuContext = mAssetResolver->GetMeshGpuResourceContext(command.mStaticMesh);

    if (meshGpuContext.mVertexBuffer.getResource() == nullptr)
    {
        mAssetResolver->RequestResolveAsset(command.mStaticMesh);
        return false;
    }

    if (meshGpuContext.mIndexBuffer.getResource() == nullptr)
    {
        mAssetResolver->RequestResolveAsset(command.mStaticMesh);
        return false;
    }

    renderItem.mMeshItem.mIndexBuffer = meshGpuContext.mIndexBuffer.getResource();
    renderItem.mMeshItem.mVertexBuffer = meshGpuContext.mVertexBuffer.getResource();

    return true;
}

void Render::ShadowRenderPass::BuildRenderItemBufferGpuResources(
    const Render::StaticMeshRenderCommnad &command, std::vector<BindingGpuResource> &bindingGpuResourceVector)
{
    // buffer

    // object buffer
    Render::BindingGpuResource bindingGpuResource;

    GRM::GpuConstantBufferContext *gpuBufferContext = static_cast<GRM::GpuConstantBufferContext *>(
        mGpuBufferContextSystem->GetGpuBufferContext(static_cast<uint8_t>(EDefaultGpuBufferType::eConstantObject128)));

    // 일단 버퍼 하나
    uint32_t bufferIndexOffset = gpuBufferContext->mAllocateRange.UseRange(1);

    uint32_t bufferOffset = bufferIndexOffset * gpuBufferContext->mBufferDesc.mElementDataSize;

    StaticMeshObjectData objectData;
    mRenderUploadManager->UploadStaticMeshObjectBuffer(command, objectData);

    // upload
    mGpuResourceManager->UploadBufferData(gpuBufferContext->mGpuBuffer, &objectData, sizeof(objectData), 1,
                                          bufferOffset);
    bindingGpuResource.gpuResource = gpuBufferContext->mGpuBuffer.getResource();
    bindingGpuResource.mOffset = bufferOffset;
    bindingGpuResource.mType = Render::EShaderResourceType::eConstantBuffer;

    bindingGpuResourceVector.push_back(bindingGpuResource);
}
