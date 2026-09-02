#include "DebugLineRenderPass.h"
#include "RenderFrontend/RenderPassGraph.h"
#include <D3DGpuResourceManager/GpuBufferContextSystem.h>
#include <D3DGpuResourceManager/IGpuResourceManager.h>
#include <RenderFrontend/AssetResolver.h>
#include <RenderFrontend/RenderMaterialResolver.h>
#include <RenderSystem/IRenderSystem.h>

Render::DebugLineRenderPass::DebugLineRenderPass()
{

    SetPassName("DebugLineRenderPass");
    SetBufferID(1);
    //    mGpuMaterialID = AssetResolver::GetInstance()->GetPassGpuMaterialID(DebugLineMat);

    RenderMaterialContext rmc;
    rmc.mGeometryType = ERenderGeometryType::eDebugLine;
    rmc.mTransparent = false;
    mGpuMaterialID = RenderMaterialResolver::GetInstance()->Resolve(rmc, ERenderPassType::eDebugLine);
}

Render::DebugLineRenderPass::~DebugLineRenderPass() {}

void Render::DebugLineRenderPass::AddToGraph(RenderPassGraph &renderPassGraph, const RenderPassSetUpData &passSetUpData)
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

void Render::DebugLineRenderPass::Execute(const RenderPassExecuteContext &renderPassExecuteContext)
{

    auto renderSystem = renderPassExecuteContext.renderSystem;

    FrameContext frameContext;
    SetGlobalData(renderPassExecuteContext.mGlobalFrameData, renderPassExecuteContext, frameContext);
    renderSystem->SetUpPassData(renderPassExecuteContext.mCommandContext, frameContext);

    renderSystem->Draw(renderPassExecuteContext.mCommandContext, BuildRenderItem(renderPassExecuteContext));
}
void Render::DebugLineRenderPass::SetGlobalData(const Core::GlobalFrameData &globalFrameData,
                                                const RenderPassExecuteContext &executeContext,
                                                FrameContext &oFrameContext)
{

    DebugLineConstantPassData constantPassData;
    constantPassData.mViewProj = globalFrameData.mViewProj;

    auto gpuBufferContextSystem = GRM::GpuBufferContextSystem::GetInstance();
    auto gpuResourceManager = GRM::IGpuResourceManager::GetInstance();

    GRM::GpuConstantBufferContext *gpuBufferContext =
        static_cast<GRM::GpuConstantBufferContext *>(gpuBufferContextSystem->GetGpuBufferContext(GetBufferID()));

    uint32_t bufferIndexOffset = gpuBufferContext->mAllocateRange.UseRange(1);
    uint32_t bufferSizeOffset = bufferIndexOffset * gpuBufferContext->mBufferDesc.mElementDataSize;
    gpuResourceManager->UploadBufferData(gpuBufferContext->mGpuBuffer, &constantPassData, sizeof(constantPassData), 1,
                                         bufferSizeOffset);

    BindingGpuResource mPassConstantBufferResouce;

    mPassConstantBufferResouce.gpuResource = gpuBufferContext->mGpuBuffer.getResource();
    mPassConstantBufferResouce.mOffset = bufferSizeOffset; //
    // bufferIndexOffset;
    mPassConstantBufferResouce.mType = Render::EShaderResourceType::eConstantBuffer;
    // viewport설정

    oFrameContext.mGlobalPassBufferResouce = mPassConstantBufferResouce;
    oFrameContext.mViewport = globalFrameData.mSceneViewport;
    oFrameContext.mViewport.TopLeftX = globalFrameData.mSceneViewport.TopLeftX;
    oFrameContext.mViewport.TopLeftY = globalFrameData.mSceneViewport.TopLeftY;

    oFrameContext.mRenderTarget = nullptr; // 기본적으로 후면버퍼를 사용하겠다 라는 의미.
    oFrameContext.mScissorRect.mLeft =
        globalFrameData.mSceneViewport.TopLeftX; //  globalFrameData.mSceneViewport.TopLeftX;
    oFrameContext.mScissorRect.mRight = oFrameContext.mScissorRect.mLeft + globalFrameData.mSceneViewport.Width;
    oFrameContext.mScissorRect.mTop =
        globalFrameData.mSceneViewport.TopLeftY; // globalFrameData.mSceneViewport.TopLeftY;
    oFrameContext.mScissorRect.mBottom = oFrameContext.mScissorRect.mTop + globalFrameData.mSceneViewport.Height;
}

std::vector<Render::RenderItem> Render::DebugLineRenderPass::BuildRenderItem(
    const RenderPassExecuteContext &executeContext)
{
    if (executeContext.mDebugLineRenderCommandList.empty())
        return {};

    auto gpuResourceManager = GRM::IGpuResourceManager::GetInstance();
    // 일단 렌더아이템 하나로 충분
    RenderItem renderItem;

    std::vector<CoreAsset::LineVertex> lineVertexList;
    for (const auto &rmd : executeContext.mDebugLineRenderCommandList)
    {
        // start vertex
        CoreAsset::LineVertex startVertex;
        startVertex.mPos = rmd.mStart;
        startVertex.mColor = rmd.mColor;

        CoreAsset::LineVertex endVertex;
        endVertex.mPos = rmd.mEnd;
        endVertex.mColor = rmd.mColor;

        lineVertexList.push_back(startVertex);
        lineVertexList.push_back(endVertex);
    }

    // vertex buffer upload
    // vertex buffer 필요
    GRM::IGpuResource *vertexBuffer = GetVertexBuffer(lineVertexList.size());
    gpuResourceManager->UploadBufferData(vertexBuffer, lineVertexList.data(), sizeof(CoreAsset::LineVertex),
                                         lineVertexList.size());

    renderItem.mMeshItem.mVertexBuffer = vertexBuffer;
    renderItem.mMeshItem.mIndexBuffer = nullptr;
    renderItem.mMeshItem.mVertexNum = lineVertexList.size();
    renderItem.mMeshItem.mVertexOffset = 0;

    renderItem.mMaterialID = mGpuMaterialID;
    renderItem.mDrawType = EDrawType::eVertex;
    renderItem.mPrimitiveTopology = EPrimitiveTopology::eLineList;

    return {renderItem};
}

GRM::IGpuResource *Render::DebugLineRenderPass::GetVertexBuffer(size_t vertexNum)
{

    if (mMaxVertexNum < vertexNum)
    {
        auto gpuResourceManager = GRM::IGpuResourceManager::GetInstance();

        // create new buffer
        GRM::BufferDesc vertexBufferDesc;
        vertexBufferDesc.mBufferMemoryAccess = GRM::EBufferMemoryAccess::eCpuWriteOnly;
        vertexBufferDesc.mBufferUsage = GRM::EBufferUsage::eVertexBuffer;
        vertexBufferDesc.mElementDataNum = vertexNum;
        vertexBufferDesc.mElementDataSize = sizeof(CoreAsset::LineVertex);
        vertexBufferDesc.mBufferSize = vertexBufferDesc.mElementDataSize * vertexBufferDesc.mElementDataNum;
        mVertexBuffer = gpuResourceManager->CreateBuffer(vertexBufferDesc);
        mMaxVertexNum = vertexNum;
    }

    return mVertexBuffer.getResource();
}
