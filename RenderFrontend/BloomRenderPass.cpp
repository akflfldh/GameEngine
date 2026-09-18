#include "BloomRenderPass.h"
#include <D3DGpuResourceManager/GpuBufferContextSystem.h>
#include <D3DGpuResourceManager/IGpuResourceManager.h>
#include <RenderFrontend/RenderMaterialResolver.h>
#include <RenderFrontend/RenderPassGraph.h>
#include <RenderSystem/IRenderSystem.h>

Render::BloomRenderPass::BloomRenderPass()
{
    SetPassName("Bloom");

    RenderMaterialContext rmc = {};
    mGpuMaterialID = RenderMaterialResolver::GetInstance()->Resolve(rmc, ERenderPassType::eBloomHorizontal);
}

Render::BloomRenderPass::~BloomRenderPass() {}

void Render::BloomRenderPass::AddToGraph(RenderPassGraph &renderPassGraph, const RenderPassSetUpData &passSetUpData)
{

    mConstantData.gWidth = passSetUpData.mWindowWidth / 2;
    mConstantData.gHeight = passSetUpData.mWindowHeight / 2;

    renderPassGraph.RegisterRenderPassCallback(
        GetName(),
        [pPass = this, passSetUpData](RenderPassGraphBuilder &builder)
        {
            RenderResourceDesc outputTargetDesc = {.mWidth = (uint32_t)pPass->mConstantData.gWidth,
                                                   .mHeight = (uint32_t)pPass->mConstantData.gHeight,
                                                   .mResourceFormat = GRM::ETextureFormat::eR16G16B16A16_FLOAT,
                                                   .mRtvFormat = std::nullopt,
                                                   .mDsvFormat = std::nullopt,
                                                   .mSrvFormat = GRM::ETextureFormat::eR16G16B16A16_FLOAT,
                                                   .mUavFormat = GRM::ETextureFormat::eR16G16B16A16_FLOAT,
                                                   .mUsage = GRM::ETextureUsage::eUnorderedAccessResource |
                                                             GRM::ETextureUsage::eShaderResource};

            builder.Create(pPass->mOutputTargetName, outputTargetDesc, EResourceState::eUnorderedAccess);

            builder.Read(pPass->mInputTexName, pPass->GetName(), EResourceState::eNonPixelShaderResource);

            // RenderResourceDesc outputDepthStencilDesc = {.mWidth = (uint32_t)pPass->GetViewport().mRight,
            //                                              // passSetUpData.mWindowWidth,
            //                                              .mHeight = (uint32_t)pPass->GetViewport().mBottom,
            //                                              // passSetUpData.mWindowHeight,
            //                                              .mResourceFormat = GRM::ETextureFormat::eR32_TYPELESS,
            //                                              .mRtvFormat = std::nullopt,
            //                                              .mDsvFormat = GRM::ETextureFormat::eD32_FLOAT,
            //                                              .mSrvFormat = GRM::ETextureFormat::eR32_FLOAT,
            //                                              .mUsage = GRM::ETextureUsage::eDepthStencil |
            //                                                        GRM::ETextureUsage::eShaderResource};

            builder.Write(pPass->mOutputTargetName, pPass->GetName(), EResourceState::eUnorderedAccess);

            //  builder.Create(pPass->mOutputDepthStencilName, outputDepthStencilDesc,
            //  EResourceState::eWriteDepthStencil);

            // RenderTarget 설정은없다.
            /*       builder.SetRenderTarget(pPass->mOutputTargetName, pPass->GetName(), pPass->mClearRenderTarget,
                                           passSetUpData.mBackBufferClearColor);*/

            /*  builder.SetDepthStencil(
                  pPass->mOutputDepthStencilName, pPass->GetName(),
                  DepthStencilClearDesc{.mClearDepth = true, .mClearStencil = false, .mDepth = 1.0f, .mStencil = 0},
                  true);*/
        },
        [pPass = this](const RenderPassExecuteContext &executeContext) { pPass->Execute(executeContext); });
}

void Render::BloomRenderPass::Execute(const RenderPassExecuteContext &renderPassExecuteContext)
{

    auto renderSystem = renderPassExecuteContext.renderSystem;

    //   renderSystem->SetUpPassData(renderPassExecuteContext.mCommandContext, mPassData);

    ComputeDispatchItem computeDispatchItem;
    computeDispatchItem.mMaterialID = mGpuMaterialID;

    mConstantData.gThreshold = 1.0f;
    // buffer

    uint32_t bufferID = static_cast<uint8_t>(EDefaultGpuBufferType::eConstantPass256);
    //    GetBufferID();

    // 여기는 공통으로 올리수있음

    GRM::IGpuResourceManager *gpuResourceManager = GRM::IGpuResourceManager::GetInstance();

    GRM::GpuBufferContextSystem *gpuBufferContextSystem = GRM::GpuBufferContextSystem::GetInstance();

    GRM::GpuConstantBufferContext *gpuBufferContext =
        static_cast<GRM::GpuConstantBufferContext *>(gpuBufferContextSystem->GetGpuBufferContext(bufferID));

    uint32_t bufferIndexOffset = gpuBufferContext->mAllocateRange.UseRange(1);

    uint32_t bufferSizeOffset = bufferIndexOffset * gpuBufferContext->mBufferDesc.mElementDataSize;

    gpuResourceManager->UploadBufferData(gpuBufferContext->mGpuBuffer, &mConstantData, sizeof(mConstantData), 1,
                                         bufferSizeOffset);

    BindingGpuResource constantBufferBinding;
    constantBufferBinding.gpuResource = gpuBufferContext->mGpuBuffer.getResource();
    constantBufferBinding.mOffset = bufferSizeOffset;
    computeDispatchItem.mConstantBuffer = constantBufferBinding;

    // input tex
    GRM::GRMPtr inputTex = renderPassExecuteContext.renderPassGraph->GetTexture(mInputTexName);
    GRM::GRMPtr outputTex = renderPassExecuteContext.renderPassGraph->GetTexture(GetOutputTargetName());

    // object x

    // tex
    {

        // CHECK(bindingGpuResource.gpuResource != nullptr, "GrayScale sceneTex Resource is nullptr");
        computeDispatchItem.mInputTexture = inputTex.getResource();
        computeDispatchItem.mOutputTexture = outputTex.getResource();
    }

    // 각각 다르게 computeDisaptchItem 조정해야함.
    if (mStage == EBloomStage::eVertical)
    {
        computeDispatchItem.mGroupCountX = mConstantData.gWidth;
        computeDispatchItem.mGroupCountY = (mConstantData.gHeight + 255) / 256;

        renderSystem->Dispatch(renderPassExecuteContext.mCommandContext, computeDispatchItem);
    }
    else
    {

        computeDispatchItem.mGroupCountX = (mConstantData.gWidth + 255) / 256;
        computeDispatchItem.mGroupCountY = mConstantData.gHeight;
        renderSystem->Dispatch(renderPassExecuteContext.mCommandContext, computeDispatchItem);
    }
}

void Render::BloomRenderPass::SetInputTexSource(const std::string &name)
{

    mInputTexName = name;
}

void Render::BloomRenderPass::SetBloomStage(EBloomStage staga)
{

    mStage = staga;

    RenderMaterialContext rmc = {};
    ERenderPassType passType =
        mStage == EBloomStage::eVertical ? ERenderPassType::eBloomVertical : ERenderPassType::eBloomHorizontal;
    mGpuMaterialID = RenderMaterialResolver::GetInstance()->Resolve(rmc, passType);
}
