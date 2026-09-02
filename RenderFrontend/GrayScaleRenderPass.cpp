#include "GrayScaleRenderPass.h"
#include <CoreBase/CoreAssert.h>
#include <D3DGpuResourceManager/GRMPtr.h>
#include <D3DGpuResourceManager/GpuBufferContextSystem.h>
#include <D3DGpuResourceManager/GpuSamplerSystem.h>
#include <D3DGpuResourceManager/IGpuResourceManager.h>
#include <RenderFrontend/AssetResolver.h>
#include <RenderFrontend/RenderMaterialResolver.h>
#include <RenderFrontend/RenderPassGraph.h>
#include <RenderSystem/IMaterialManager.h>
#include <RenderSystem/IRenderSystem.h>

Render::GrayScaleRenderPass::GrayScaleRenderPass()
{
    SetBufferID(1);
    //    mPassGpuMaterialID = Render::AssetResolver::GetInstance()->GetPassGpuMaterialID(GrayScaleMat);

    RenderMaterialContext rmc;
    rmc.mGeometryType = ERenderGeometryType::eStaticMesh;
    rmc.mTransparent = true;
    mPassGpuMaterialID = RenderMaterialResolver::GetInstance()->Resolve(rmc, ERenderPassType::eGrayScale);
}

Render::GrayScaleRenderPass::~GrayScaleRenderPass() {}

void Render::GrayScaleRenderPass::SetInputSource(const std::string &name)
{
    mInputSource = name;
}

void Render::GrayScaleRenderPass::AddToGraph(RenderPassGraph &renderPassGraph, const RenderPassSetUpData &passSetUpData)
{

    renderPassGraph.RegisterRenderPassCallback(
        GetName(),
        [pPass = this, passSetUpData](RenderPassGraphBuilder &builder)
        {
            RenderResourceDesc outputTargetDesc = {passSetUpData.mWindowWidth, passSetUpData.mWindowHeight,
                                                   GRM::ETextureFormat::eR8G8B8A8_UNORM,
                                                   GRM::ETextureUsage::eRenderTarget};
            builder.Create(pPass->mOutputTargetName, outputTargetDesc, EResourceState::eRenderTarget);

            builder.Read(pPass->mInputSource, pPass->GetName(), EResourceState::eGenericRead);

            RenderResourceDesc outputDepthStencilDesc = {passSetUpData.mWindowWidth, passSetUpData.mWindowHeight,
                                                         GRM::ETextureFormat::eD24_UNORM_S8_UINT,
                                                         GRM::ETextureUsage::eDepthStencil};

            builder.Create(pPass->mOutputDepthStencilName, outputDepthStencilDesc, EResourceState::eWriteDepthStencil);

            builder.SetRenderTarget(pPass->mOutputTargetName, pPass->GetName(), pPass->mClearRenderTarget,
                                    passSetUpData.mBackBufferClearColor);
            builder.SetDepthStencil(pPass->mOutputDepthStencilName, pPass->GetName(), true, 1.0f, true);
        },
        [pPass = this](const RenderPassExecuteContext &executeContext) { pPass->Execute(executeContext); });
}

void Render::GrayScaleRenderPass::SetGlobalData(const Core::GlobalFrameData &globalFrameData,
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
    GrayScaleConstantPass passConstantData;
    passConstantData.mGrayScale = 1.0f;

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

    oFrameContext.mRenderTarget = nullptr;
    oFrameContext.mScissorRect.mLeft = globalFrameData.mSceneViewport.TopLeftX;
    oFrameContext.mScissorRect.mRight = oFrameContext.mScissorRect.mLeft + globalFrameData.mSceneViewport.Width;
    oFrameContext.mScissorRect.mTop = globalFrameData.mSceneViewport.TopLeftY;
    oFrameContext.mScissorRect.mBottom = oFrameContext.mScissorRect.mTop + globalFrameData.mSceneViewport.Height;
}

void Render::GrayScaleRenderPass::Execute(const RenderPassExecuteContext &executeContext)
{
    IMaterialManager *gpuMaterialManager = IMaterialManager::GetInstance();

    // TODO   SetUpPassData
    FrameContext frameContext;
    SetGlobalData(executeContext.mUIGlobalFrameData, frameContext);
    executeContext.renderSystem->SetUpPassData(executeContext.mCommandContext, frameContext);

    RenderItem renderItem;
    renderItem.mMaterialID = mPassGpuMaterialID; // temp

    // shader resource
    //   gpu material - binding object resource list
    GRM::GRMPtr sceneTex = executeContext.renderPassGraph->GetTexture(mInputSource);

    // object x

    // tex
    {
        Render::BindingGpuResource bindingGpuResource;
        bindingGpuResource.gpuResource = sceneTex.getResource();

        CHECK(bindingGpuResource.gpuResource != nullptr, "GrayScale sceneTex Resource is nullptr");

        bindingGpuResource.mType = EShaderResourceType::eTexture;
        renderItem.mBindingGpuTexResourceVector.push_back(std::move(bindingGpuResource));
    }

    //// sampler
    //{
    //    Render::BindingGpuResource bindingGpuResource;
    //    bindingGpuResource.gpuResource = mGpuSamplerSystem->GetGpuSampler(1).getResource();
    //    bindingGpuResource.mType = EShaderResourceType::eSampler;
    //    bindingGpuResource.mName = bindingShaderResourceInfoSet.mObjectSamplerShaderResourceInfoVector[0].mName;
    //    renderItem.mBindingGpuResourceVector.push_back(std::move(bindingGpuResource));
    //}

    executeContext.renderSystem->DrawFullScreen(executeContext.mCommandContext, renderItem);
}
