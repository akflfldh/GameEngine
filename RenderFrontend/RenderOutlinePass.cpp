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
    // auto assetResolver = Render::AssetResolver::GetInstance();
    // mStaticMeshStencilMaterialID = assetResolver->GetPassGpuMaterialID(OutlineStencilMat);
    // mStaticMeshOutlineMaterialID = assetResolver->GetPassGpuMaterialID(OutlineDrawMat);

    RenderMaterialContext rmc;
    rmc.mGeometryType = ERenderGeometryType::eStaticMesh;
    rmc.mTransparent = false;
    mStaticMeshStencilMaterialID =
        RenderMaterialResolver::GetInstance()->Resolve(rmc, Render::ERenderPassType::eOutlineStencil);

    mStaticMeshOutlineMaterialID =
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
            builder.SetDepthStencil(pPass->mOutputDepthStencilName, pPass->GetName(), true, 1.0f, true);
        },
        [pPass = this](const RenderPassExecuteContext &executeContext) { pPass->Execute(executeContext); });
    SetViewport({0, (float)passSetUpData.mWindowWidth, 0, (float)passSetUpData.mWindowHeight});
}
void Render::RenderOutlinePass::SetPassConstantBufferResource(Render::BindingGpuResource bindingConstnatBuffer) {}
void Render::RenderOutlinePass::SetGlobalData(const Core::GlobalFrameData &globalFrameData)
{

    OutlineConstantData constantData;
    // 필요한 pass 데이터
    constantData.mViewProj = globalFrameData.mViewProj;

    uint32_t bufferID = GetBufferID();

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

    // 일반적인 MainPass들은 전체화면이라고생각
    //  최종

    mPassData.mViewport = globalFrameData.mSceneViewport;
    mPassData.mViewport.TopLeftX = globalFrameData.mSceneViewport.TopLeftX;
    mPassData.mViewport.TopLeftY = globalFrameData.mSceneViewport.TopLeftY;

    mPassData.mGlobalPassBufferResouce = mPassConstantBufferResource;
    mPassData.mRenderTarget = nullptr; // 기본적으로 후면버퍼를 사용하겠다 라는 의미.
    mPassData.mScissorRect.mLeft = globalFrameData.mSceneViewport.TopLeftX;
    mPassData.mScissorRect.mRight = mPassData.mScissorRect.mLeft + globalFrameData.mSceneViewport.Width;
    mPassData.mScissorRect.mTop = globalFrameData.mSceneViewport.TopLeftY;
    mPassData.mScissorRect.mBottom = mPassData.mScissorRect.mTop + globalFrameData.mSceneViewport.Height;
}
std::vector<Render::RenderItem> Render::RenderOutlinePass::BuildRenderItem(
    const Render::RenderPassExecuteContext &executeContext)
{
    std::vector<Render::RenderItem> renderItemVec;

    for (size_t i = 0; i < executeContext.mOutlineStaticMeshRenderCommandIndexList.size(); ++i)
    {
        const Render::StaticMeshOutlineRenderCommand &command =
            executeContext.mOutlineStaticMeshRenderCommandIndexList[i];
        // auto command = executeContext.mStaticMeshRenderCommand[outlineCommand.mStaticMeshRenderCommnadIndex];

        // staticMesh가 지정되지않아서 무시
        if (command.mStaticMesh == nullptr)
            continue;

        const std::vector<CoreAsset::SubMesh> &subMeshVector = command.mStaticMesh->GetSubMeshVector();
        const CoreAsset::SubMesh &subMesh = subMeshVector[command.mSubMeshIndex];
        Render::RenderItem renderItem;

        renderItem.mScissor = mPassData.mScissorRect;

        // instance
        renderItem.mInstance.mInstanceCount = 1;
        renderItem.mInstance.mInstanceBufferOffset = 0;

        // draw type
        renderItem.mDrawType = EDrawType::eIndex;

        // gpu material
        // renderItem.mMaterialID = material->GetGpuMaterialID();

        // TODO
        //  static mesh 버전의 고정된 gpuMaterial을 사용하게한다.
        renderItem.mMaterialID = mStaticMeshStencilMaterialID;

        // mesh
        renderItem.mMeshItem.mIndexNum = subMeshVector[command.mSubMeshIndex].mIndexNum;
        renderItem.mMeshItem.mIndexOffset = subMeshVector[command.mSubMeshIndex].mIndexOffset;
        renderItem.mMeshItem.mVertexOffset = subMeshVector[command.mSubMeshIndex].mVertexOffset;

        Render::MeshGpuResourceContext meshGpuContext = mAssetResolver->GetMeshGpuResourceContext(command.mStaticMesh);

        if (meshGpuContext.mVertexBuffer.getResource() == nullptr)
        {
            bool ret = mAssetResolver->RequestResolveAsset(command.mStaticMesh);
            meshGpuContext = mAssetResolver->GetMeshGpuResourceContext(command.mStaticMesh);
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
    }
}

void Render::RenderOutlinePass::BuildRenderItemBufferGpuResources(
    const Render::StaticMeshOutlineRenderCommand &command,
    std::vector<Render::BindingGpuResource> &bindingGpuResourceVector)
{

    // object buffer
    Render::BindingGpuResource bindingGpuResource;

    GRM::GpuConstantBufferContext *gpuBufferContext =
        static_cast<GRM::GpuConstantBufferContext *>(mGpuBufferContextSystem->GetGpuBufferContext(mObjectBufferID));

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

    bindingGpuResourceVector.push_back(bindingGpuResource);
}

void Render::RenderOutlinePass::Execute(const RenderPassExecuteContext &renderPassExecuteContext)
{

    IRenderSystem *renderSystem = renderPassExecuteContext.renderSystem;

    SetGlobalData(renderPassExecuteContext.mGlobalFrameData);
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
