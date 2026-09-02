#include "SkySphereRenderPass.h"
#include <CoreAsset/StaticMesh.h>
#include <D3DGpuResourceManager/GpuBufferContextSystem.h>
#include <D3DGpuResourceManager/IGpuResourceManager.h>
#include <RenderFrontend/AssetResolver.h>
#include <RenderFrontend/RenderMaterialResolver.h>
#include <RenderFrontend/RenderPassGraph.h>
#include <RenderSystem/IRenderSystem.h>

#include <CoreAsset/Texture.h>

Render::SkySphereRenderPass::SkySphereRenderPass()
{

    SetBufferID(1);

    SetPassName("SkySphereRenderPass");
    mUseDefaultViewport = false;
}

Render::SkySphereRenderPass::~SkySphereRenderPass() {}

void Render::SkySphereRenderPass::AddToGraph(RenderPassGraph &renderPassGraph, const RenderPassSetUpData &passSetUpData)
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
            builder.SetDepthStencil(pPass->mOutputDepthStencilName, pPass->GetName(), false, 1.0f, false);
        },
        [pPass = this](const RenderPassExecuteContext &executeContext) { pPass->Execute(executeContext); });
    SetViewport({0, (float)passSetUpData.mWindowWidth, 0, (float)passSetUpData.mWindowHeight});
}

void Render::SkySphereRenderPass::Execute(const RenderPassExecuteContext &renderPassExecuteContext)
{
    if (renderPassExecuteContext.mSkySphereSnapshot.mActiveFlag == false)
        return;

    auto renderSystem = renderPassExecuteContext.renderSystem;

    std::vector<RenderItem> renderItem = BuildRenderItem(renderPassExecuteContext);

    FrameContext passData;

    SetGlobalData(renderPassExecuteContext.mGlobalFrameData, renderPassExecuteContext, passData);

    renderSystem->SetUpPassData(renderPassExecuteContext.mCommandContext,
                                passData); // target binding , 전역 pass buffer binding 등등이

    renderSystem->Draw(renderPassExecuteContext.mCommandContext, renderItem);
}

void Render::SkySphereRenderPass::SetGlobalData(const Core::GlobalFrameData &globalFrameData,
                                                const RenderPassExecuteContext &executeContext, FrameContext &oPassData)
{

    SkySphereConstnatData skySphereConstantData;
    // 필요한 pass 데이터
    skySphereConstantData.mViewProj = globalFrameData.mViewProj;
    skySphereConstantData.mLightNums = executeContext.mLightRenderCommandList.size();
    skySphereConstantData.mCameraPosWorld = globalFrameData.mCameraPositionWorld;
    skySphereConstantData.mAmbientLight = globalFrameData.mAmbientLight;
    skySphereConstantData.mWorld = executeContext.mSkySphereSnapshot.mTransform;

    uint32_t bufferID = GetBufferID();

    // 여기는 공통으로 올리수있음

    GRM::IGpuResourceManager *gpuResourceManager = GRM::IGpuResourceManager::GetInstance();

    GRM::GpuBufferContextSystem *gpuBufferContextSystem = GRM::GpuBufferContextSystem::GetInstance();

    GRM::GpuConstantBufferContext *gpuBufferContext =
        static_cast<GRM::GpuConstantBufferContext *>(gpuBufferContextSystem->GetGpuBufferContext(bufferID));

    uint32_t bufferIndexOffset = gpuBufferContext->mAllocateRange.UseRange(1);

    uint32_t bufferSizeOffset = bufferIndexOffset * gpuBufferContext->mBufferDesc.mElementDataSize;

    gpuResourceManager->UploadBufferData(gpuBufferContext->mGpuBuffer, &skySphereConstantData,
                                         sizeof(skySphereConstantData), 1, bufferSizeOffset);

    BindingGpuResource passBufferGpuResource;
    passBufferGpuResource.gpuResource = gpuBufferContext->mGpuBuffer.getResource();
    passBufferGpuResource.mOffset = bufferSizeOffset; //
    // bufferIndexOffset;
    passBufferGpuResource.mType = Render::EShaderResourceType::eConstantBuffer;

    // 일반적인 MainPass들은 전체화면이라고생각
    //  최종

    oPassData.mViewport = globalFrameData.mSceneViewport;
    oPassData.mViewport.TopLeftX = globalFrameData.mSceneViewport.TopLeftX;
    oPassData.mViewport.TopLeftY = globalFrameData.mSceneViewport.TopLeftY;

    oPassData.mGlobalPassBufferResouce = passBufferGpuResource;
    oPassData.mRenderTarget = nullptr; // 기본적으로 후면버퍼를 사용하겠다 라는 의미.
    oPassData.mScissorRect.mLeft = globalFrameData.mSceneViewport.TopLeftX; //  globalFrameData.mSceneViewport.TopLeftX;
    oPassData.mScissorRect.mRight = oPassData.mScissorRect.mLeft + globalFrameData.mSceneViewport.Width;
    oPassData.mScissorRect.mTop = globalFrameData.mSceneViewport.TopLeftY; // globalFrameData.mSceneViewport.TopLeftY;
    oPassData.mScissorRect.mBottom = oPassData.mScissorRect.mTop + globalFrameData.mSceneViewport.Height;
}
std::vector<Render::RenderItem> Render::SkySphereRenderPass::BuildRenderItem(
    const RenderPassExecuteContext &executeContext)
{

    // snapshot 이 있어야한다.
    // snapshot안에는 texture id 정보

    std::vector<RenderItem> renderItemList;

    auto renderMaterialResolver = RenderMaterialResolver::GetInstance();
    // 일단 하나
    for (int i = 0; i < 1; ++i)
    {
        RenderItem renderItem;

        if (!BuildRenderItemMeshData(executeContext.mSkySphereSnapshot, renderItem))
        {
            continue;
        }

        if (!BuildRenderItemTexGpuResources(executeContext.mSkySphereSnapshot, renderItem.mBindingGpuTexResourceVector))
        {
            continue;
        }

        RenderMaterialContext rmc;

        rmc.mGeometryType = ERenderGeometryType::eStaticMesh;
        rmc.mShadingModel = CoreAsset::EShadingModel::eNone;
        rmc.mTransparent = false;

        renderItem.mMaterialID = renderMaterialResolver->Resolve(rmc, Render::ERenderPassType::eSkySphere);

        renderItemList.push_back(renderItem);
    }

    return renderItemList;
}

bool Render::SkySphereRenderPass::BuildRenderItemMeshData(const Render::SkySphereSnapshot &skySphereSnapshot,
                                                          Render::RenderItem &renderItem)
{

    const std::vector<CoreAsset::SubMesh> &subMeshVector = skySphereSnapshot.mSphereMesh->GetSubMeshVector();
    const CoreAsset::SubMesh subMesh = subMeshVector[0];

    // instance
    renderItem.mInstance.mInstanceCount = 1;
    renderItem.mInstance.mInstanceBufferOffset = 0;

    // draw type
    renderItem.mDrawType = EDrawType::eIndex;

    // mesh
    renderItem.mMeshItem.mIndexNum = subMesh.mIndexNum;
    renderItem.mMeshItem.mIndexOffset = subMesh.mIndexOffset;
    renderItem.mMeshItem.mVertexOffset = subMesh.mVertexOffset;

    Render::MeshGpuResourceContext meshGpuContext =
        mAssetResolver->GetMeshGpuResourceContext(skySphereSnapshot.mSphereMesh);

    if (meshGpuContext.mVertexBuffer.getResource() == nullptr)
    {
        mAssetResolver->RequestResolveAsset(skySphereSnapshot.mSphereMesh);
        return false;
    }

    renderItem.mMeshItem.mIndexBuffer = meshGpuContext.mIndexBuffer.getResource();
    renderItem.mMeshItem.mVertexBuffer = meshGpuContext.mVertexBuffer.getResource();
}

bool Render::SkySphereRenderPass::BuildRenderItemTexGpuResources(
    const SkySphereSnapshot &skySphereSnapshot, std::vector<BindingGpuResource> &bindingGpuResourceVector)
{

    // tex
    auto assetResolver = AssetResolver::GetInstance();

    // diffuse
    std::vector<CoreAsset::Texture *> texList;

    CoreAsset::Texture *skyTexture = skySphereSnapshot.mSkyTexture;

    if (skyTexture == nullptr)
    {
        return false;
    }
    else
    {

        texList.push_back(skyTexture);
    }

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
