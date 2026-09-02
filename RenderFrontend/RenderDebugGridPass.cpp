#include "RenderDebugGridPass.h"
#include <D3DGpuResourceManager/GpuBufferContextSystem.h>
#include <D3DGpuResourceManager/IGpuResourceManager.h>
#include <RenderFrontend/RenderPassGraph.h>
#include <RenderSystem/IRenderSystem.h>

#include <RenderFrontend/AssetResolver.h>
#include <RenderFrontend/RenderMaterialResolver.h>

Render::RenderDebugGridPass::RenderDebugGridPass()
{
    SetPassName("DebugGridPass");
    SetBufferID(1);
    mUseDefaultViewport = false;
    // mPassMatID = Render::AssetResolver::GetInstance()->GetPassGpuMaterialID(DebugGridMat);

    RenderMaterialContext rmc;
    rmc.mGeometryType = ERenderGeometryType::eStaticMesh;
    rmc.mTransparent = true;

    mPassMatID = Render::RenderMaterialResolver::GetInstance()->Resolve(rmc, ERenderPassType::eDebugGrid);

    /*   if (bInit)
       {
           auto gpuResourceManager = GRM::IGpuResourceManager::GetInstance();

           GRM::BufferDesc vertexBufferDesc;
           vertexBufferDesc.
           GRM::GRMPtr vertexPtr = gpuResourceManager->CreateBuffer(vertexBufferDesc);

           GRM::BufferDesc indexBufferDesc;
           GRM::GRMPtr indexPtr = gpuResourceManager->CreateBuffer(indexBufferDesc);



           bInit = false;
       }*/
}

Render::RenderDebugGridPass::~RenderDebugGridPass() {}

void Render::RenderDebugGridPass::AddToGraph(RenderPassGraph &renderPassGraph, const RenderPassSetUpData &passSetUpData)
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

void Render::RenderDebugGridPass::Execute(const RenderPassExecuteContext &renderPassExecuteContext)
{
    RenderPassExecuteContext passExecuteContext = renderPassExecuteContext;

    IRenderSystem *renderSystem = renderPassExecuteContext.renderSystem;

    SetGlobalData(renderPassExecuteContext.mGlobalFrameData, renderPassExecuteContext);
    renderSystem->SetUpPassData(renderPassExecuteContext.mCommandContext,
                                mPassData); // target binding , 전역 pass buffer binding 등등이

    RenderItem renderItem;
    renderItem.mMaterialID = mPassMatID; // temp
    renderItem.mScissor = mPassData.mScissorRect;

    // 렌더아이템리스트 전달 draw
    renderSystem->DrawFullScreen(renderPassExecuteContext.mCommandContext, renderItem);
}

void Render::RenderDebugGridPass::SetGlobalData(const Core::GlobalFrameData &globalFrameData,
                                                const RenderPassExecuteContext &executeContext)
{

    // pass buffer
    DebugGridConstnatData constantData;
    constantData.mViewProj = globalFrameData.mViewProj;
    constantData.mCameraPosWorld = globalFrameData.mCameraPositionWorld;

    uint32_t bufferID = GetBufferID();

    // 여기는 공통으로 올리수있음

    GRM::IGpuResourceManager *gpuResourceManager = GRM::IGpuResourceManager::GetInstance();

    GRM::GpuBufferContextSystem *gpuBufferContextSystem = GRM::GpuBufferContextSystem::GetInstance();

    GRM::GpuConstantBufferContext *gpuBufferContext =
        static_cast<GRM::GpuConstantBufferContext *>(gpuBufferContextSystem->GetGpuBufferContext(bufferID));

    uint32_t bufferIndexOffset = gpuBufferContext->mAllocateRange.UseRange(1);

    uint32_t bufferSizeOffset = bufferIndexOffset * gpuBufferContext->mBufferDesc.mElementDataSize;

    gpuResourceManager->UploadBufferData(gpuBufferContext->mGpuBuffer, &constantData, sizeof(DebugGridConstnatData), 1,
                                         bufferSizeOffset);

    mPassConstantBufferResource.gpuResource = gpuBufferContext->mGpuBuffer.getResource();
    mPassConstantBufferResource.mOffset = bufferSizeOffset; //
    // bufferIndexOffset;
    mPassConstantBufferResource.mType = Render::EShaderResourceType::eConstantBuffer;

    // passData
    mPassData.mViewport = globalFrameData.mSceneViewport;
    mPassData.mViewport.TopLeftX = globalFrameData.mSceneViewport.TopLeftX;
    mPassData.mViewport.TopLeftY = globalFrameData.mSceneViewport.TopLeftY;

    mPassData.mGlobalPassBufferResouce = mPassConstantBufferResource;
    mPassData.mRenderTarget = nullptr; // 기본적으로 후면버퍼를 사용하겠다 라는 의미.
    mPassData.mScissorRect.mLeft = globalFrameData.mSceneViewport.TopLeftX; //  globalFrameData.mSceneViewport.TopLeftX;
    mPassData.mScissorRect.mRight = mPassData.mScissorRect.mLeft + globalFrameData.mSceneViewport.Width;
    mPassData.mScissorRect.mTop = globalFrameData.mSceneViewport.TopLeftY; // globalFrameData.mSceneViewport.TopLeftY;
    mPassData.mScissorRect.mBottom = mPassData.mScissorRect.mTop + globalFrameData.mSceneViewport.Height;
}
//
// std::vector<Render::RenderItem> Render::RenderDebugGridPass::BuildRenderItem(
//    const RenderPassExecuteContext &executeContext)
//{
//}