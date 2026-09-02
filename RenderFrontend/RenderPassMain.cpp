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

    MainConstnatData mainConstantData;
    // 필요한 pass 데이터
    mainConstantData.mViewProj = globalFrameData.mViewProj;
    mainConstantData.mLightNums = executeContext.mLightRenderCommandList.size();
    mainConstantData.mCameraPosWorld = globalFrameData.mCameraPositionWorld;
    mainConstantData.mAmbientLight = globalFrameData.mAmbientLight;

    uint32_t bufferID = GetBufferID();

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

        gpuStructuredBufferContext->mGpuBuffersPerFrame[gpuStructuredBufferContext->mCurrFrameIndex];

        mPassData.mGlobalStructuredBufferResource.gpuResource =
            gpuStructuredBufferContext->mGpuBuffersPerFrame[gpuStructuredBufferContext->mCurrFrameIndex].getResource();
        mPassData.mGlobalStructuredBufferResource.mOffset = 0;
    }
}

std::vector<Render::RenderItem> Render::RenderPassMain::BuildRenderItem(const RenderPassExecuteContext &executeContext)
{

    auto renderMaterialResolver = RenderMaterialResolver::GetInstance();

    // renderItem 정적 불투명 메시들에대해서 렌더아이템 생성

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

        uint32_t materialHandle = command.mMaterialHandle;
        auto matIt = executeContext.mMaterialRenderSnapshotTable.find(materialHandle);

        const MaterialRenderSnapshot &materialRenderSnapshot = matIt->second;

        if (materialRenderSnapshot.mUseExplicitGpuMat)
        {
            renderItem.mMaterialID = materialRenderSnapshot.mGpuMatID;
        }
        else
        {
            RenderMaterialContext rmc;
            rmc.mGeometryType = ERenderGeometryType::eStaticMesh;
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

bool Render::RenderPassMain::BuildRenderItemMeshData(const Render::StaticMeshRenderCommnad &command,
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

    renderItem.mMeshItem.mIndexBuffer = meshGpuContext.mIndexBuffer.getResource();
    renderItem.mMeshItem.mVertexBuffer = meshGpuContext.mVertexBuffer.getResource();
}

void Render::RenderPassMain::BuildRenderItemBufferGpuResources(
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

    StaticMeshObjectData objectData;
    mRenderUploadManager->UploadStaticMeshObjectBuffer(command, objectData);

    // upload
    mGpuResourceManager->UploadBufferData(gpuBufferContext->mGpuBuffer, &objectData, sizeof(objectData), 1,
                                          bufferOffset);
    bindingGpuResource.gpuResource = gpuBufferContext->mGpuBuffer.getResource();
    bindingGpuResource.mOffset = bufferOffset;
    bindingGpuResource.mType = Render::EShaderResourceType::eConstantBuffer;

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

    bindingGpuResourceVector.push_back(bindingMatGpuResource);
}

void Render::RenderPassMain::BuildRenderItemTexGpuResources(const MaterialRenderSnapshot &materialRenderSnapshot,
                                                            MaterialID gpuMaterialID,
                                                            std::vector<BindingGpuResource> &bindingGpuResourceVector)
{

    // tex

    // diffuse
    std::vector<CoreAsset::Texture *> texList;

    const std::vector<CoreAsset::Texture *> &albedoMapList = materialRenderSnapshot.mAlbedoMapList;

    if (albedoMapList.empty())
    {
        // 기본적으로 하나의 diffuse map은 바인딩

        texList.push_back(static_cast<CoreAsset::Texture *>(
            CoreAsset::AssetManager::GetInstance()->GetDefafultDiffuseWhiteMap().Get()));
    }
    else
    {
        for (auto texContext : albedoMapList)
        {
            texList.push_back(texContext);
        }
    }

    // normal map

    if (materialRenderSnapshot.mNormalMap != nullptr)
    {
        texList.push_back(materialRenderSnapshot.mNormalMap);
    }
    else
    {
        // 디폴트 노멀맵부여
        texList.push_back(
            static_cast<CoreAsset::Texture *>(CoreAsset::AssetManager::GetInstance()->GetDefaultNormalMap().Get()));
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
