#include "RenderOutlinePass.h"
#include <CoreAsset/StaticMesh.h>
#include <D3DGpuResourceManager/GpuBufferContextSystem.h>
#include <D3DGpuResourceManager/IGpuResourceManager.h>
#include <RenderFrontend/AssetResolver.h>
#include <RenderFrontend/RenderMaterialResolver.h>
#include <RenderFrontend/RenderPassGraph.h>
#include <RenderFrontend/RenderUploadManager.h>
#include <RenderSystem/IMaterialManager.h>
#include <RenderSystem/IRenderSystem.h>

Render::RenderOutlinePass::RenderOutlinePass()
{
    SetPassName("RenderOutlinePass");
    SetBufferID(1);
    RenderMaterialContext rmc;
    rmc.mGeometryType = ERenderGeometryType::eStaticMesh;
    rmc.mTransparent = false;
    mStaticMeshStencilMaterialID =
        RenderMaterialResolver::GetInstance()->Resolve(rmc, Render::ERenderPassType::eOutlineStencil);

    mStaticMeshOutlineMaterialID =
        RenderMaterialResolver::GetInstance()->Resolve(rmc, Render::ERenderPassType::eOutlineDraw);

    rmc.mGeometryType = ERenderGeometryType::eSkinnedMesh;
    mSkinningMeshStencilMaterialID =
        RenderMaterialResolver::GetInstance()->Resolve(rmc, Render::ERenderPassType::eOutlineStencil);
    mSkinningMeshOutlineMaterialID =
        RenderMaterialResolver::GetInstance()->Resolve(rmc, Render::ERenderPassType::eOutlineDraw);

    mObjectBufferID = 2;
}

Render::RenderOutlinePass::~RenderOutlinePass() {}

void Render::RenderOutlinePass::AddToGraph(RenderPassGraph &renderPassGraph, const RenderPassSetUpData &passSetUpData)
{
    renderPassGraph.RegisterRenderPassCallback(
        GetName(),
        [pPass = this, passSetUpData](RenderPassGraphBuilder &builder)
        {
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

            builder.SetRenderTarget(pPass->mOutputTargetName, pPass->GetName(), pPass->mClearRenderTarget,
                                    passSetUpData.mBackBufferClearColor);
            builder.SetDepthStencil(
                pPass->mOutputDepthStencilName, pPass->GetName(),
                DepthStencilClearDesc{.mClearDepth = true, .mClearStencil = true, .mDepth = 1.0f, .mStencil = 0}, true);
        },
        [pPass = this](const RenderPassExecuteContext &executeContext) { pPass->Execute(executeContext); });
    SetViewport({0, (float)passSetUpData.mWindowWidth, 0, (float)passSetUpData.mWindowHeight});
}
void Render::RenderOutlinePass::SetPassConstantBufferResource(Render::BindingGpuResource bindingConstnatBuffer) {}
void Render::RenderOutlinePass::SetGlobalData(const RenderPassExecuteContext &executeContext)
{
    mPassData.mGlobalStructuredBufferResource2 = {};

    OutlineConstantData constantData;
    // 필요한 pass 데이터
    constantData.mViewProj = executeContext.mGlobalFrameData.mViewProj;

    uint32_t bufferID = static_cast<uint8_t>(EDefaultGpuBufferType::eConstantPass256);
    // GetBufferID();

    // 여기는 공통으로 올리수있음

    GRM::IGpuResourceManager *gpuResourceManager = GRM::IGpuResourceManager::GetInstance();

    GRM::GpuBufferContextSystem *gpuBufferContextSystem = GRM::GpuBufferContextSystem::GetInstance();

    GRM::GpuConstantBufferContext *gpuBufferContext =
        static_cast<GRM::GpuConstantBufferContext *>(gpuBufferContextSystem->GetGpuBufferContext(bufferID));

    uint32_t bufferIndexOffset = gpuBufferContext->mAllocateRange.UseRange(1);

    uint32_t bufferSizeOffset = bufferIndexOffset * gpuBufferContext->mBufferDesc.mElementDataSize;

    gpuResourceManager->UploadBufferData(gpuBufferContext->mGpuBuffer, &constantData, sizeof(constantData), 1,
                                         bufferSizeOffset);

    mPassConstantBufferResource.gpuResource = gpuBufferContext->mGpuBuffer.getResource();
    mPassConstantBufferResource.mOffset = bufferSizeOffset;
    mPassConstantBufferResource.mType = Render::EShaderResourceType::eConstantBuffer;
    mPassConstantBufferResource.mSemantic = EMasterRootBindingSemantic::ePassConstantBuffer;

    // 스텐실 기록과 외곽선 모두 카메라 투영에 대응하는 3D 영역을 사용한다.
    // 백버퍼에 직접 그리므로 창 내부 3D 좌상단에 논리적 창 위치만 더한다.
    mPassData.mViewport = executeContext.mGlobalFrameData.mSceneViewport;
    mPassData.mViewport.TopLeftX += executeContext.mGlobalSceneViewport.TopLeftX;
    mPassData.mViewport.TopLeftY += executeContext.mGlobalSceneViewport.TopLeftY;

    mPassData.mGlobalPassBufferResouce = mPassConstantBufferResource;
    if (!executeContext.mSkinPaletteSnapshot.empty())
    {
        GRM::GpuStructuredBufferContext *skinPaletteBufferContext =
            static_cast<GRM::GpuStructuredBufferContext *>(gpuBufferContextSystem->GetGpuBufferContext(
                AssetResolver::GetInstance()->GetSkinPaletteStructuredGpuBufferID()));
        mPassData.mGlobalStructuredBufferResource2.gpuResource =
            skinPaletteBufferContext->mGpuBuffersPerFrame[skinPaletteBufferContext->mCurrFrameIndex].getResource();
        mPassData.mGlobalStructuredBufferResource2.mOffset = 0;
        mPassData.mGlobalStructuredBufferResource2.mType = EShaderResourceType::eStructuredBuffer;
        mPassData.mGlobalStructuredBufferResource2.mSemantic = EMasterRootBindingSemantic::eSkinPaletteStructuredBuffer;
    }
    mPassData.mRenderTarget = nullptr; // 기본적으로 후면버퍼를 사용하겠다 라는 의미.
    mPassData.mScissorRect.mLeft = mPassData.mViewport.TopLeftX;
    mPassData.mScissorRect.mRight = mPassData.mScissorRect.mLeft + mPassData.mViewport.Width;

    mPassData.mScissorRect.mTop = mPassData.mViewport.TopLeftY;
    mPassData.mScissorRect.mBottom = mPassData.mScissorRect.mTop + mPassData.mViewport.Height;
}
std::vector<Render::RenderItem> Render::RenderOutlinePass::BuildRenderItem(
    const Render::RenderPassExecuteContext &executeContext)
{
    std::vector<Render::RenderItem> renderItemVec;

    for (size_t i = 0; i < executeContext.mOutlineMeshRenderCommandIndexList.size(); ++i)
    {
        const Render::MeshOutlineRenderCommand &command = executeContext.mOutlineMeshRenderCommandIndexList[i];
        // auto command = executeContext.mStaticMeshRenderCommand[outlineCommand.mStaticMeshRenderCommnadIndex];

        // staticMesh가 지정되지않아서 무시
        if (command.mMesh == nullptr || command.mSubMeshIndex < 0 ||
            static_cast<size_t>(command.mSubMeshIndex) >= command.mMesh->GetSubMeshVector().size())
            continue;

        const std::vector<CoreAsset::SubMesh> &subMeshVector = command.mMesh->GetSubMeshVector();
        const CoreAsset::SubMesh &subMesh = subMeshVector[command.mSubMeshIndex];
        Render::RenderItem renderItem;

        renderItem.mScissor = mPassData.mScissorRect;

        // instance
        renderItem.mInstance.mInstanceCount = 1;
        renderItem.mInstance.mInstanceBufferOffset = 0;

        // draw type
        renderItem.mDrawType = EDrawType::eIndex;

        renderItem.mMaterialID = command.mGeometryType == ERenderGeometryType::eSkinnedMesh
                                     ? mSkinningMeshStencilMaterialID
                                     : mStaticMeshStencilMaterialID;

        // mesh
        renderItem.mMeshItem.mIndexNum = subMeshVector[command.mSubMeshIndex].mIndexNum;
        renderItem.mMeshItem.mIndexOffset = subMeshVector[command.mSubMeshIndex].mIndexOffset;
        renderItem.mMeshItem.mVertexOffset = subMeshVector[command.mSubMeshIndex].mVertexOffset;

        Render::MeshGpuResourceContext meshGpuContext = mAssetResolver->GetMeshGpuResourceContext(command.mMesh);

        if (meshGpuContext.mVertexBuffer.getResource() == nullptr)
        {
            bool ret = mAssetResolver->RequestResolveAsset(command.mMesh);
            meshGpuContext = mAssetResolver->GetMeshGpuResourceContext(command.mMesh);
        }

        renderItem.mMeshItem.mIndexBuffer = meshGpuContext.mIndexBuffer.getResource();
        renderItem.mMeshItem.mVertexBuffer = meshGpuContext.mVertexBuffer.getResource();

        // scissor는 Pass와 동일하게
        renderItem.mScissor;

        // gpu material - binding object resource list

        // buffer
        BuildRenderItemBufferGpuResources(command, renderItem.mBindingGpuBufferResourceVector);

        renderItemVec.push_back(std::move(renderItem));
    }

    return renderItemVec;
}
void Render::RenderOutlinePass::ChangeMaterial(std::vector<RenderItem> &renderItemVec)
{

    // static 스텐실 버전 - > static outline 버전
    // skinning 스텐실 버전 -> skinning outline 버전

    // 두 세이더 모두 동일한 리소스사용, 구조임으로
    //  오브젝트버퍼, 패스버퍼 다 동일하게 유지할수있다.

    for (auto &renderItem : renderItemVec)
    {
        if (renderItem.mMaterialID == mStaticMeshStencilMaterialID)
        {
            renderItem.mMaterialID = mStaticMeshOutlineMaterialID;
        }
        else if (renderItem.mMaterialID == mSkinningMeshStencilMaterialID)
        {
            renderItem.mMaterialID = mSkinningMeshOutlineMaterialID;
        }
    }
}

void Render::RenderOutlinePass::BuildRenderItemBufferGpuResources(
    const Render::MeshOutlineRenderCommand &command, std::vector<Render::BindingGpuResource> &bindingGpuResourceVector)
{

    // object buffer
    Render::BindingGpuResource bindingGpuResource;

    GRM::GpuConstantBufferContext *gpuBufferContext = static_cast<GRM::GpuConstantBufferContext *>(
        mGpuBufferContextSystem->GetGpuBufferContext(static_cast<uint8_t>(EDefaultGpuBufferType::eConstantObject128)));

    // 일단 버퍼 하나
    uint32_t bufferIndexOffset = gpuBufferContext->mAllocateRange.UseRange(1);

    uint32_t bufferOffset = bufferIndexOffset * gpuBufferContext->mBufferDesc.mElementDataSize;

    StaticMeshOutlineData objectData;
    mRenderUploadManager->UploadStaticMeshOutlineData(command, objectData);

    // upload
    mGpuResourceManager->UploadBufferData(gpuBufferContext->mGpuBuffer, &objectData, sizeof(objectData), 1,
                                          bufferOffset);
    bindingGpuResource.gpuResource = gpuBufferContext->mGpuBuffer.getResource();
    bindingGpuResource.mOffset = bufferOffset;
    bindingGpuResource.mType = EShaderResourceType::eConstantBuffer;
    bindingGpuResource.mSemantic = EMasterRootBindingSemantic::eObjectConstantBuffer;

    bindingGpuResourceVector.push_back(bindingGpuResource);
}

void Render::RenderOutlinePass::Execute(const RenderPassExecuteContext &renderPassExecuteContext)
{

    IRenderSystem *renderSystem = renderPassExecuteContext.renderSystem;

    SetGlobalData(renderPassExecuteContext);
    renderSystem->SetUpPassData(renderPassExecuteContext.mCommandContext,
                                mPassData); // target binding , 전역 pass buffer binding 등등이

    // 먼저 스텐실값기록 머터리얼로 draw
    std::vector<RenderItem> renderItemList = BuildRenderItem(renderPassExecuteContext);

    renderSystem->SetUpStencilValue(renderPassExecuteContext.mCommandContext, 1);
    renderSystem->Draw(renderPassExecuteContext.mCommandContext, renderItemList);

    // renderItem의 머터리얼 재설정
    ChangeMaterial(renderItemList);

    // 그다음 아웃라인 머터리얼로 draw
    renderSystem->Draw(renderPassExecuteContext.mCommandContext, renderItemList);
    renderSystem->SetUpStencilValue(renderPassExecuteContext.mCommandContext, 0);
}
