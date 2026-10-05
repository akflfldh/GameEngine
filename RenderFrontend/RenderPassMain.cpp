#include "RenderPassMain.h"
#include "RenderDebugGridPass.h"
#include <Core/CoreType.h>
#include <CoreAsset/AssetManager.h>
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
#include <utility>

Render::RenderPassMain::RenderPassMain()
{
    SetPassName("RenderPassMain");
    SetBufferID(1);
    mObjectBufferID = 2;

    mUseDefaultViewport = false;
}

Render::RenderPassMain::~RenderPassMain() {}

void Render::RenderPassMain::AddToGraph(RenderPassGraph &renderPassGraph, const RenderPassSetUpData &passSetUpData)
{

    renderPassGraph.RegisterRenderPassCallback(
        GetName(),
        [pPass = this, passSetUpData](RenderPassGraphBuilder &builder)
        {
            RenderResourceDesc outputTargetDesc = {.mWidth = passSetUpData.mWindowWidth,
                                                   .mHeight = passSetUpData.mWindowHeight,
                                                   .mResourceFormat = GRM::ETextureFormat::eR16G16B16A16_FLOAT,
                                                   .mRtvFormat = GRM::ETextureFormat::eR16G16B16A16_FLOAT,
                                                   .mDsvFormat = std::nullopt,
                                                   .mSrvFormat = GRM::ETextureFormat::eR16G16B16A16_FLOAT,
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

            builder.Read("DirectionalShadowMap", pPass->GetName(), EResourceState::ePixelShaderResource);

            builder.SetRenderTarget(pPass->mOutputTargetName, pPass->GetName(), pPass->mClearRenderTarget,
                                    passSetUpData.mBackBufferClearColor);
            builder.SetDepthStencil(
                pPass->mOutputDepthStencilName, pPass->GetName(),
                DepthStencilClearDesc{.mClearDepth = true, .mClearStencil = true, .mDepth = 1.0f, .mStencil = 0}, true);
        },
        [pPass = this](const RenderPassExecuteContext &executeContext) { pPass->Execute(executeContext); });
    SetViewport({0, (float)passSetUpData.mWindowWidth, 0, (float)passSetUpData.mWindowHeight});
}

void Render::RenderPassMain::Execute(const RenderPassExecuteContext &renderPassExecuteContext)
{

    IRenderSystem *renderSystem = renderPassExecuteContext.renderSystem;

    SetGlobalData(renderPassExecuteContext.mGlobalFrameData, renderPassExecuteContext);
    renderSystem->SetUpPassData(renderPassExecuteContext.mCommandContext,
                                mPassData); // target binding , 전역 pass buffer binding 등등이

    // 렌더아이템리스트 전달 draw
    renderSystem->Draw(renderPassExecuteContext.mCommandContext, BuildRenderItem(renderPassExecuteContext));
}

void Render::RenderPassMain::SetPassConstantBufferResource(Render::BindingGpuResource bindingConstnatBuffer)
{
    mPassConstantBufferResource = bindingConstnatBuffer;
}

void Render::RenderPassMain::SetGlobalData(const Core::GlobalFrameData &globalFrameData,
                                           const RenderPassExecuteContext &executeContext)
{

    // FrameContext는 pass 객체가 재사용하므로 선택적 global resource가 이전 프레임에서 남지 않게 초기화한다.
    mPassData.mGlobalStructuredBufferResource = {};
    mPassData.mGlobalStructuredBufferResource2 = {};
    mPassData.mGlobalPassTexResourceVector.clear();

    MainConstnatData mainConstantData;
    // 필요한 pass 데이터
    mainConstantData.mViewProj = globalFrameData.mViewProj;
    mainConstantData.mLightNums = executeContext.mLightRenderCommandList.size();
    mainConstantData.mCameraPosWorld = globalFrameData.mCameraPositionWorld;
    mainConstantData.mAmbientLight = globalFrameData.mAmbientLight;

    mainConstantData.mLightViewProj = executeContext.mDirectonalShadowRenderData.mViewProj;
    mainConstantData.mShadowEnabled = executeContext.mDirectonalShadowRenderData.mEnabled;
    mainConstantData.mShadowMapInvSize = {1.0F / executeContext.mDirectonalShadowRenderData.mShadowMapSize,
                                          1.0F / executeContext.mDirectonalShadowRenderData.mShadowMapSize};

    uint32_t bufferID = static_cast<uint8_t>(EDefaultGpuBufferType::eConstantPass256);
    //    GetBufferID();

    // 여기는 공통으로 올리수있음

    GRM::IGpuResourceManager *gpuResourceManager = GRM::IGpuResourceManager::GetInstance();

    GRM::GpuBufferContextSystem *gpuBufferContextSystem = GRM::GpuBufferContextSystem::GetInstance();

    GRM::GpuConstantBufferContext *gpuBufferContext =
        static_cast<GRM::GpuConstantBufferContext *>(gpuBufferContextSystem->GetGpuBufferContext(bufferID));

    uint32_t bufferIndexOffset = gpuBufferContext->mAllocateRange.UseRange(1);

    uint32_t bufferSizeOffset = bufferIndexOffset * gpuBufferContext->mBufferDesc.mElementDataSize;

    gpuResourceManager->UploadBufferData(gpuBufferContext->mGpuBuffer, &mainConstantData, sizeof(MainConstnatData), 1,
                                         bufferSizeOffset);

    mPassConstantBufferResource.gpuResource = gpuBufferContext->mGpuBuffer.getResource();
    mPassConstantBufferResource.mOffset = bufferSizeOffset; //
    // bufferIndexOffset;
    mPassConstantBufferResource.mType = Render::EShaderResourceType::eConstantBuffer;
    mPassConstantBufferResource.mSemantic = EMasterRootBindingSemantic::ePassConstantBuffer;

    // 일반적인 MainPass들은 전체화면이라고생각
    //  최종

    mPassData.mViewport = globalFrameData.mSceneViewport;
    mPassData.mViewport.TopLeftX = globalFrameData.mSceneViewport.TopLeftX;
    mPassData.mViewport.TopLeftY = globalFrameData.mSceneViewport.TopLeftY;

    mPassData.mGlobalPassBufferResouce = mPassConstantBufferResource;
    mPassData.mRenderTarget = nullptr; // 기본적으로 후면버퍼를 사용하겠다 라는 의미.
    mPassData.mScissorRect.mLeft = globalFrameData.mSceneViewport.TopLeftX; //  globalFrameData.mSceneViewport.TopLeftX;
    mPassData.mScissorRect.mRight = mPassData.mScissorRect.mLeft + globalFrameData.mSceneViewport.Width;
    mPassData.mScissorRect.mTop = globalFrameData.mSceneViewport.TopLeftY; // globalFrameData.mSceneViewport.TopLeftY;
    mPassData.mScissorRect.mBottom = mPassData.mScissorRect.mTop + globalFrameData.mSceneViewport.Height;

    // Light
    if (executeContext.mLightRenderCommandList.size() != 0)
    {
        GRM::GpuStructuredBufferContext *gpuStructuredBufferContext = static_cast<GRM::GpuStructuredBufferContext *>(
            gpuBufferContextSystem->GetGpuBufferContext(AssetResolver::GetInstance()->GetLightStructuredGpuBufferID()));

        mPassData.mGlobalStructuredBufferResource.gpuResource =
            gpuStructuredBufferContext->mGpuBuffersPerFrame[gpuStructuredBufferContext->mCurrFrameIndex].getResource();
        mPassData.mGlobalStructuredBufferResource.mOffset = 0;
        mPassData.mGlobalStructuredBufferResource.mType = EShaderResourceType::eStructuredBuffer;
        mPassData.mGlobalStructuredBufferResource.mSemantic = EMasterRootBindingSemantic::eLightStructuredBuffer;
    }

    GRM::GRMPtr shadowMapTex = executeContext.renderPassGraph->GetTexture("DirectionalShadowMap");

    BindingGpuResource shadowMapBindingResource;
    shadowMapBindingResource.gpuResource = shadowMapTex.getResource();
    shadowMapBindingResource.mType = Render::EShaderResourceType::eTexture;
    shadowMapBindingResource.mSemantic = EMasterRootBindingSemantic::eShadowMapTexture;
    mPassData.mGlobalPassTexResourceVector.push_back(shadowMapBindingResource);

    // Anim Palette

    if (executeContext.mSkinPaletteSnapshot.size() != 0)
    {
        GRM::GpuStructuredBufferContext *gpuStructuredBufferContext =
            static_cast<GRM::GpuStructuredBufferContext *>(gpuBufferContextSystem->GetGpuBufferContext(
                AssetResolver::GetInstance()->GetSkinPaletteStructuredGpuBufferID()));

        mPassData.mGlobalStructuredBufferResource2.gpuResource =
            gpuStructuredBufferContext->mGpuBuffersPerFrame[gpuStructuredBufferContext->mCurrFrameIndex].getResource();
        mPassData.mGlobalStructuredBufferResource2.mOffset = 0;
        mPassData.mGlobalStructuredBufferResource2.mType = EShaderResourceType::eStructuredBuffer;
        mPassData.mGlobalStructuredBufferResource2.mSemantic = EMasterRootBindingSemantic::eSkinPaletteStructuredBuffer;
    }
}

std::vector<Render::RenderItem> Render::RenderPassMain::BuildRenderItem(const RenderPassExecuteContext &executeContext)
{

    auto renderMaterialResolver = RenderMaterialResolver::GetInstance();

    // renderItem 정적 불투명 메시들에대해서 렌더아이템 생성

    std::vector<Render::RenderItem> renderItemVec;

    for (const auto &command : executeContext.mOpaqueMeshRenderCommandList)
    {

        // staticMesh가 지정되지않아서 무시
        if (command.mMesh == nullptr)
            continue;

        Render::RenderItem renderItem;

        // BuildMeshData();
        if (!BuildRenderItemMeshData(command, renderItem))
        {
            continue;
            // 렌더아이템의 메시구축실패 - >렌더하지않는다.
        }

        uint32_t materialHandle = command.mMaterialHandle;
        auto matIt = executeContext.mMaterialRenderSnapshotTable.find(materialHandle);

        const MaterialRenderSnapshot &materialRenderSnapshot = matIt->second;

        {
            RenderMaterialContext rmc;
            rmc.mGeometryType = command.mGeometryType;
            rmc.mTransparent = false;
            rmc.mShadingModel = materialRenderSnapshot.mShadingModel;

            renderItem.mMaterialID = renderMaterialResolver->Resolve(rmc, ERenderPassType::eMain);
        }
        renderItem.mScissor = mPassData.mScissorRect;

        // binding gpu resource (모두 object단위 gpu resource)
        // buffer, tex

        // gpu material - binding object resource list
        BuildRenderItemBufferGpuResources(command, renderItem.mBindingGpuBufferResourceVector);

        // Build RenderItem Tex BindingResource;
        BuildRenderItemTexGpuResources(materialRenderSnapshot, renderItem.mMaterialID,
                                       renderItem.mBindingGpuTexResourceVector);

        renderItemVec.push_back(std::move(renderItem));
    }

    return renderItemVec;
}

bool Render::RenderPassMain::BuildRenderItemMeshData(const Render::MeshRenderCommand &command,
                                                     Render::RenderItem &renderItem)
{

    const std::vector<CoreAsset::SubMesh> &subMeshVector = command.mMesh->GetSubMeshVector();
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

    Render::MeshGpuResourceContext meshGpuContext = mAssetResolver->GetMeshGpuResourceContext(command.mMesh);

    if (meshGpuContext.mVertexBuffer.getResource() == nullptr)
    {
        mAssetResolver->RequestResolveAsset(command.mMesh);
        return false;
    }

    renderItem.mMeshItem.mIndexBuffer = meshGpuContext.mIndexBuffer.getResource();
    renderItem.mMeshItem.mVertexBuffer = meshGpuContext.mVertexBuffer.getResource();

    return true;
}

void Render::RenderPassMain::BuildRenderItemBufferGpuResources(
    const Render::MeshRenderCommand &command, std::vector<BindingGpuResource> &bindingGpuResourceVector)
{
    // buffer

    // object buffer
    Render::BindingGpuResource bindingGpuResource;

    GRM::GpuConstantBufferContext *gpuBufferContext = static_cast<GRM::GpuConstantBufferContext *>(
        mGpuBufferContextSystem->GetGpuBufferContext(static_cast<uint8_t>(EDefaultGpuBufferType::eConstantObject128)));

    // 일단 버퍼 하나
    uint32_t bufferIndexOffset = gpuBufferContext->mAllocateRange.UseRange(1);

    uint32_t bufferOffset = bufferIndexOffset * gpuBufferContext->mBufferDesc.mElementDataSize;

    MeshObjectData objectData;
    mRenderUploadManager->UploadStaticMeshObjectBuffer(command, objectData);

    // upload
    mGpuResourceManager->UploadBufferData(gpuBufferContext->mGpuBuffer, &objectData, sizeof(objectData), 1,
                                          bufferOffset);
    bindingGpuResource.gpuResource = gpuBufferContext->mGpuBuffer.getResource();
    bindingGpuResource.mOffset = bufferOffset;
    bindingGpuResource.mType = Render::EShaderResourceType::eConstantBuffer;
    bindingGpuResource.mSemantic = EMasterRootBindingSemantic::eObjectConstantBuffer;

    bindingGpuResourceVector.push_back(bindingGpuResource);

    // material buffer ...
    // material buffer는 업로드는 안하고 바인딩만 수행 .
    GRM::GpuConstantBufferContext *matBufferContext =
        static_cast<GRM::GpuConstantBufferContext *>(mGpuBufferContextSystem->GetGpuBufferContext(3));
    bufferIndexOffset = matBufferContext->mAllocateRange.GetCurrentFrameIndex(command.mMaterialHandle);
    bufferOffset = bufferIndexOffset * matBufferContext->mBufferDesc.mElementDataSize;

    Render::BindingGpuResource bindingMatGpuResource;
    bindingMatGpuResource.gpuResource = matBufferContext->mGpuBuffer.getResource();
    bindingMatGpuResource.mOffset = bufferOffset;
    bindingMatGpuResource.mType = Render::EShaderResourceType::eConstantBuffer;
    bindingMatGpuResource.mSemantic = EMasterRootBindingSemantic::eMaterialConstantBuffer;

    bindingGpuResourceVector.push_back(bindingMatGpuResource);
}

void Render::RenderPassMain::BuildRenderItemTexGpuResources(const MaterialRenderSnapshot &materialRenderSnapshot,
                                                            MaterialID gpuMaterialID,
                                                            std::vector<BindingGpuResource> &bindingGpuResourceVector)
{

    const std::vector<CoreAsset::Texture *> &albedoMapList = materialRenderSnapshot.mAlbedoMapList;
    CoreAsset::Texture *albedoTexture =
        albedoMapList.empty() ? static_cast<CoreAsset::Texture *>(
                                    CoreAsset::AssetManager::GetInstance()->GetDefafultDiffuseWhiteMap().Get())
                              : albedoMapList.front();
    CoreAsset::Texture *normalTexture =
        materialRenderSnapshot.mNormalMap != nullptr
            ? materialRenderSnapshot.mNormalMap
            : static_cast<CoreAsset::Texture *>(CoreAsset::AssetManager::GetInstance()->GetDefaultNormalMap().Get());

    // 현재 Main HLSL 계약은 albedo t1 한 장과 normal t2 한 장이다. Asset에 여러 albedo가 있어도
    // 벡터 순서로 다른 register에 밀어 넣지 않고 셰이더가 선언한 첫 albedo만 사용한다.
    const std::pair<CoreAsset::Texture *, EMasterRootBindingSemantic> textureBindings[] = {
        {albedoTexture, EMasterRootBindingSemantic::eAlbedoTexture},
        {normalTexture, EMasterRootBindingSemantic::eNormalTexture}};

    for (const auto &[texture, semantic] : textureBindings)
    {
        Render::BindingGpuResource bindingGpuResource;
        mAssetResolver->RequestResolveAsset(texture);
        bindingGpuResource.gpuResource = mAssetResolver->GetGpuResource(texture).getResource();

        bindingGpuResource.mType = EShaderResourceType::eTexture;
        bindingGpuResource.mSemantic = semantic;
        bindingGpuResourceVector.push_back(std::move(bindingGpuResource));
    }
}
