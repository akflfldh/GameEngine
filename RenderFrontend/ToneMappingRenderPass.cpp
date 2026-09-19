#include "ToneMappingRenderPass.h"
#include <CoreBase/CoreAssert.h>
#include <D3DGpuResourceManager/GRMPtr.h>
#include <D3DGpuResourceManager/GpuBufferContextSystem.h>
#include <D3DGpuResourceManager/GpuSamplerSystem.h>
#include <D3DGpuResourceManager/IGpuResourceManager.h>
#include <RenderFrontend/RenderMaterialResolver.h>
#include <RenderFrontend/RenderPassGraph.h>
#include <RenderSystem/IMaterialManager.h>
#include <RenderSystem/IRenderSystem.h>

Render::ToneMappingRenderPass::ToneMappingRenderPass()
{
    SetBufferID(1);
    RenderMaterialContext rmc;
    rmc.mGeometryType = ERenderGeometryType::eStaticMesh;
    rmc.mTransparent = false;
    mPassGpuMaterialID = RenderMaterialResolver::GetInstance()->Resolve(rmc, ERenderPassType::eToneMapping);

    SetPassName("ToneMapping");
}

Render::ToneMappingRenderPass::~ToneMappingRenderPass() {}

void Render::ToneMappingRenderPass::SetInputSource(const std::string &name)
{
    mInputSource = name;
}

void Render::ToneMappingRenderPass::SetInputSourceTwo(const std::string &name)
{
    mInputSourceTwo = name;
}

void Render::ToneMappingRenderPass::AddToGraph(RenderPassGraph &renderPassGraph,
                                               const RenderPassSetUpData &passSetUpData)
{

    renderPassGraph.RegisterRenderPassCallback(
        GetName(),
        [pPass = this, passSetUpData](RenderPassGraphBuilder &builder)
        {
            RenderResourceDesc outputTargetDesc = {.mWidth = passSetUpData.mWindowWidth,
                                                   .mHeight = passSetUpData.mWindowHeight,
                                                   .mResourceFormat = GRM::ETextureFormat::eR8G8B8A8_UNORM,
                                                   .mRtvFormat = GRM::ETextureFormat::eR8G8B8A8_UNORM_SRGB,
                                                   .mDsvFormat = std::nullopt,
                                                   .mSrvFormat = GRM::ETextureFormat::eR8G8B8A8_UNORM_SRGB,
                                                   .mUsage = GRM::ETextureUsage::eRenderTarget};
            builder.Create(pPass->mOutputTargetName, outputTargetDesc, EResourceState::eRenderTarget);

            builder.Read(pPass->mInputSource, pPass->GetName(), EResourceState::eGenericRead);
            builder.Read(pPass->mInputSourceTwo, pPass->GetName(), EResourceState::eGenericRead);

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
}

void Render::ToneMappingRenderPass::SetGlobalData(const RenderPassExecuteContext &executeContext,
                                                  FrameContext &oFrameContext)
{

    /*
    viewport
    pass constant buffer

    render target


    scissor rect
    */
    static float t = 1.0f;
    t -= 0.01f;
    if (t < 0.0f)
        t = 1.0f;
    ToneMappingConstant passConstantData;
    passConstantData.gExposure = executeContext.mPostProcessingData.mExposure;

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
    oFrameContext.mViewport = executeContext.mGlobalSceneViewport;

    oFrameContext.mRenderTarget = nullptr;
    oFrameContext.mScissorRect.mLeft = executeContext.mGlobalSceneViewport.TopLeftX;
    oFrameContext.mScissorRect.mRight = oFrameContext.mScissorRect.mLeft + executeContext.mGlobalSceneViewport.Width;
    oFrameContext.mScissorRect.mTop = executeContext.mGlobalSceneViewport.TopLeftY;
    oFrameContext.mScissorRect.mBottom = oFrameContext.mScissorRect.mTop + executeContext.mGlobalSceneViewport.Height;
}

void Render::ToneMappingRenderPass::Execute(const RenderPassExecuteContext &executeContext)
{
    IMaterialManager *gpuMaterialManager = IMaterialManager::GetInstance();

    // TODO   SetUpPassData
    FrameContext frameContext;
    SetGlobalData(executeContext, frameContext);
    executeContext.renderSystem->SetUpPassData(executeContext.mCommandContext, frameContext);

    RenderItem renderItem;
    renderItem.mMaterialID = mPassGpuMaterialID; // temp

    // shader resource
    //   gpu material - binding object resource list
    GRM::GRMPtr sceneTex = executeContext.renderPassGraph->GetTexture(mInputSource);
    GRM::GRMPtr sceneTexTwo = executeContext.renderPassGraph->GetTexture(mInputSourceTwo);

    // object x

    // tex
    {
        Render::BindingGpuResource bindingGpuResource;
        bindingGpuResource.gpuResource = sceneTex.getResource();

        CHECK(bindingGpuResource.gpuResource != nullptr, "GrayScale sceneTex Resource is nullptr");

        bindingGpuResource.mType = EShaderResourceType::eTexture;
        renderItem.mBindingGpuTexResourceVector.push_back(std::move(bindingGpuResource));
    }

    // tex
    {
        Render::BindingGpuResource bindingGpuResource;
        bindingGpuResource.gpuResource = sceneTexTwo.getResource();

        CHECK(bindingGpuResource.gpuResource != nullptr, "GrayScale sceneTex Resource is nullptr");

        bindingGpuResource.mType = EShaderResourceType::eTexture;
        renderItem.mBindingGpuTexResourceVector.push_back(std::move(bindingGpuResource));
    }

    executeContext.renderSystem->DrawFullScreen(executeContext.mCommandContext, renderItem);
}
