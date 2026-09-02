#include "RenderEditOverlayPass.h"
#include <Core/CoreType.h>
#include <CoreAsset/Material.h>
#include <CoreAsset/StaticMesh.h>
#include <D3DGpuResourceManager/GpuBufferContextSystem.h>
#include <D3DGpuResourceManager/GpuSamplerSystem.h>
#include <D3DGpuResourceManager/IGpuResourceManager.h>
#include <RenderFrontend/AssetResolver.h>
#include <RenderFrontend/RenderMaterialResolver.h>
#include <RenderFrontend/RenderPassGraph.h>
#include <RenderFrontend/RenderPipelineManager.h>
#include <RenderFrontend/RenderUploadManager.h>
#include <RenderSystem/IMaterialManager.h>
#include <RenderSystem/IRenderSystem.h>

Render::RenderEditOverlayPass::RenderEditOverlayPass()
{

    SetPassName("RenderEditOverlayPass");
    SetBufferID(1);

    mObjectBufferID = 2;
    mUseDefaultViewport = false;
}

Render::RenderEditOverlayPass::~RenderEditOverlayPass() {}

void Render::RenderEditOverlayPass::AddToGraph(RenderPassGraph &renderPassGraph,
                                               const RenderPassSetUpData &passSetUpData)
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

void Render::RenderEditOverlayPass::SetPassConstantBufferResource(Render::BindingGpuResource bindingConstnatBuffer) {}
void Render::RenderEditOverlayPass::SetGlobalData(const Core::GlobalFrameData &globalFrameData)
{
    EditorOverlayPassData mainConstantData;
    // 필요한 pass 데이터
    mainConstantData.mViewProj = globalFrameData.mViewProj;

    uint32_t bufferID = GetBufferID();

    // 여기는 공통으로 올리수있음

    GRM::IGpuResourceManager *gpuResourceManager = GRM::IGpuResourceManager::GetInstance();

    GRM::GpuBufferContextSystem *gpuBufferContextSystem = GRM::GpuBufferContextSystem::GetInstance();

    GRM::GpuConstantBufferContext *gpuBufferContext =
        static_cast<GRM::GpuConstantBufferContext *>(gpuBufferContextSystem->GetGpuBufferContext(bufferID));

    uint32_t bufferIndexOffset = gpuBufferContext->mAllocateRange.UseRange(1);

    uint32_t bufferSizeOffset = bufferIndexOffset * gpuBufferContext->mBufferDesc.mElementDataSize;

    gpuResourceManager->UploadBufferData(gpuBufferContext->mGpuBuffer, &mainConstantData, sizeof(mainConstantData), 1,
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

std::vector<Render::RenderItem> Render::RenderEditOverlayPass::BuildRenderItem(
    const RenderPassExecuteContext &executeContext)
{
    // renderItem 정적 불투명 메시들에대해서 렌더아이템 생성

    auto renderMaterialResolver = RenderMaterialResolver::GetInstance();

    std::vector<Render::RenderItem> renderItemVec;

    for (auto command : executeContext.mEditorOverlayStaticMeshRenderCommandList)
    {

        // staticMesh가 지정되지않아서 무시
        if (command.mStaticMesh == nullptr)
            continue;
        Render::RenderItem renderItem;
        renderItem.mScissor = mPassData.mScissorRect;

        uint32_t materialHandle = command.mMaterialHandle;
        const MaterialRenderSnapshot &materialRenderSnapshot =
            executeContext.mMaterialRenderSnapshotTable.find(materialHandle)->second;

        //        CoreAsset::Material *material = command.mMaterial;

        if (materialRenderSnapshot.mUseExplicitGpuMat)
        {
            renderItem.mMaterialID = materialRenderSnapshot.mGpuMatID;
        }
        else
        {

            RenderMaterialContext rmc;
            rmc.mGeometryType = ERenderGeometryType::eStaticMesh;
            rmc.mTransparent = false;
            renderItem.mMaterialID = renderMaterialResolver->Resolve(rmc, Render::ERenderPassType::eMain);
        }

        BuildRenderItemMeshData(command, renderItem);

        BuildRenderItemBufferGpuResources(command, renderItem.mBindingGpuBufferResourceVector);

        BuildRenderItemTexGpuResources(materialRenderSnapshot, renderItem.mBindingGpuTexResourceVector);
        // binding gpu resource (모두 object단위 gpu resource)
        // tex, sampler, buffer

        //// gpu material - binding object resource list
        // const Render::ShaderResourceInfoSet &bindingShaderResourceInfoSet =
        //     mGpuMaterialManager->GetMaterialShaderResourceInfo(renderItem.mMaterialID);

        //// tex
        // const std::vector<CoreAsset::AssetMaterialTexResourceContext> &texResourceContextVector =
        //     material->GetTexResourceContextList();
        // for (int i = 0; i < bindingShaderResourceInfoSet.mObjectTextureShaderResourceInfoVector.size(); ++i)
        //{
        //     Render::BindingGpuResource bindingGpuResource;
        //     bindingGpuResource.gpuResource =
        //         mAssetResolver->GetGpuResource(texResourceContextVector[i].mTexture.Get()).getResource();

        //    if (bindingGpuResource.gpuResource == nullptr)
        //    {
        //        mAssetResolver->RequestResolveAsset(texResourceContextVector[i].mTexture.Get());
        //        bindingGpuResource.gpuResource =
        //            mAssetResolver->GetGpuResource(texResourceContextVector[i].mTexture.Get()).getResource();
        //    }

        //    bindingGpuResource.mType = EShaderResourceType::eTexture;
        //    bindingGpuResource.mName = bindingShaderResourceInfoSet.mObjectTextureShaderResourceInfoVector[i].mName;
        //    renderItem.mBindingGpuResourceVector.push_back(std::move(bindingGpuResource));
        //}

        //// buffer
        // for (int i = 0; i < bindingShaderResourceInfoSet.mObjectBufferShaderResourceInfoVector.size(); ++i)
        //{
        //     uint32_t bufferID = bindingShaderResourceInfoSet.mObjectBufferShaderResourceInfoVector[i].mBufferID;
        //     Render::BindingGpuResource bindingGpuResource;

        //    bindingGpuResource.mType = bindingShaderResourceInfoSet.mObjectBufferShaderResourceInfoVector[i].mType;

        //    GRM::GpuBufferContext *gpuBufferContext = mGpuBufferContextSystem->GetGpuBufferContext(bufferID);

        //    // 일단 버퍼 하나
        //    uint32_t bufferIndexOffset = gpuBufferContext->mAllocateRange.UseRange(1);

        //    uint32_t bufferOffset = bufferIndexOffset * gpuBufferContext->mBufferDesc.mElementDataSize;

        //    // std::vector<uint8_t> pData(gpuBufferContext->mBufferDesc.mElementDataSize);

        //    // bindingShaderResourceInfoSet.mObjectBufferShaderResourceInfoVector[i].mCreateBufferData(&command,
        //    // nullptr,
        //    //                                                                                         pData.data());

        //    Render::StaticMeshGizmoData objectData;

        //    mRenderUploadManager->UploadStaticMeshGizmoData(command, objectData);
        //    // upload
        //    mGpuResourceManager->UploadBufferData(gpuBufferContext->mGpuBuffer, &objectData,
        //                                          gpuBufferContext->mBufferDesc.mElementDataSize, 1, bufferOffset);

        //    bindingGpuResource.gpuResource = gpuBufferContext->mGpuBuffer.getResource();
        //    bindingGpuResource.mName = bindingShaderResourceInfoSet.mObjectBufferShaderResourceInfoVector[i].mName;
        //    bindingGpuResource.mOffset = bufferIndexOffset;

        //    renderItem.mBindingGpuResourceVector.push_back(std::move(bindingGpuResource));
        //}

        renderItemVec.push_back(std::move(renderItem));
    }

    return renderItemVec;
}

void Render::RenderEditOverlayPass::Execute(const RenderPassExecuteContext &renderPassExecuteContext)
{

    RenderPassExecuteContext mainPassExecuteContext = renderPassExecuteContext;

    IRenderSystem *renderSystem = renderPassExecuteContext.renderSystem;

    SetGlobalData(renderPassExecuteContext.mGlobalFrameData);
    renderSystem->SetUpPassData(renderPassExecuteContext.mCommandContext,
                                mPassData); // target binding , 전역 pass buffer binding 등등이

    // 렌더아이템리스트 전달 draw
    renderSystem->Draw(renderPassExecuteContext.mCommandContext, BuildRenderItem(renderPassExecuteContext));
}

void Render::RenderEditOverlayPass::BuildRenderItemMeshData(const Render::StaticMeshRenderCommnad &command,
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
        bool ret = mAssetResolver->RequestResolveAsset(command.mStaticMesh);
        meshGpuContext = mAssetResolver->GetMeshGpuResourceContext(command.mStaticMesh);
    }

    renderItem.mMeshItem.mIndexBuffer = meshGpuContext.mIndexBuffer.getResource();
    renderItem.mMeshItem.mVertexBuffer = meshGpuContext.mVertexBuffer.getResource();
}

void Render::RenderEditOverlayPass::BuildRenderItemBufferGpuResources(
    const Render::StaticMeshRenderCommnad &command, std::vector<BindingGpuResource> &bindingGpuResourceVector)
{
    // buffer

    // object buffer
    Render::BindingGpuResource bindingGpuResource;
    GRM::GpuConstantBufferContext *gpuBufferContext =
        static_cast<GRM::GpuConstantBufferContext *>(mGpuBufferContextSystem->GetGpuBufferContext(mObjectBufferID));

    // 일단 버퍼 하나
    uint32_t bufferIndexOffset = gpuBufferContext->mAllocateRange.UseRange(1);

    uint32_t bufferOffset = bufferIndexOffset * gpuBufferContext->mBufferDesc.mElementDataSize;

    Render::StaticMeshGizmoData objectData;

    mRenderUploadManager->UploadStaticMeshGizmoData(command, objectData);
    // upload
    mGpuResourceManager->UploadBufferData(gpuBufferContext->mGpuBuffer, &objectData,
                                          gpuBufferContext->mBufferDesc.mElementDataSize, 1, bufferOffset);
    bindingGpuResource.gpuResource = gpuBufferContext->mGpuBuffer.getResource();
    bindingGpuResource.mOffset = bufferOffset;
    bindingGpuResource.mType = EShaderResourceType::eConstantBuffer;

    bindingGpuResourceVector.push_back(bindingGpuResource);
    // material buffer ...
}

void Render::RenderEditOverlayPass::BuildRenderItemTexGpuResources(
    const MaterialRenderSnapshot &materialRenderSnapshot, std::vector<BindingGpuResource> &bindingGpuResourceVector)
{
    // tex

    // albedo
    std::vector<CoreAsset::Texture *> texList;

    const std::vector<CoreAsset::Texture *> &albedoMapList = materialRenderSnapshot.mAlbedoMapList;

    for (auto &tex : albedoMapList)
    {
        texList.push_back(tex);
    }

    // texList.push_back(static_cast<CoreAsset::Texture *>(material->GetNormalTexResource().mTexture.Get()));

    // add bindingGpuResource
    for (auto tex : texList)
    {
        Render::BindingGpuResource bindingGpuResource;
        bindingGpuResource.gpuResource = mAssetResolver->GetGpuResource(tex).getResource();

        if (bindingGpuResource.gpuResource == nullptr)
        {
            mAssetResolver->RequestResolveAsset(tex);
            bindingGpuResource.gpuResource = mAssetResolver->GetGpuResource(tex).getResource();
        }

        bindingGpuResource.mType = EShaderResourceType::eTexture;
        bindingGpuResourceVector.push_back(std::move(bindingGpuResource));
    }
}
