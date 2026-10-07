#include "RenderPipelineManager.h"
#include "RenderDebugGridPass.h"
#include <Core/LogicalWindow.h>
#include <Core/Map.h>
#include <Core/WindowedFrameController.h>
#include <Core/World.h>
#include <CoreAsset/AssetManager.h>
#include <CoreAsset/Font.h>
#include <CoreAsset/Material.h>
#include <CoreAsset/SkinningMesh.h>
#include <CoreAsset/StaticMesh.h>
#include <CoreBase/CoreAssert.h>
#include <D3DGpuResourceManager/IGpuResourceManager.h>
#include <RenderFrontend/BillboardRenderPass.h>
#include <RenderFrontend/BloomRenderPass.h>
#include <RenderFrontend/DebugLineRenderPass.h>
#include <RenderFrontend/GrayScaleRenderPass.h>
#include <RenderFrontend/ObjectRenderItemBuilder.h>
#include <RenderFrontend/RenderEditOverlayPass.h>
#include <RenderFrontend/RenderOutlinePass.h>
#include <RenderFrontend/RenderPassGraph.h>
#include <RenderFrontend/RenderPassMain.h>
#include <RenderFrontend/RenderPassUI.h>
#include <RenderFrontend/RenderPipeline.h>
#include <RenderFrontend/RenderPipelineOpaque.h>
#include <RenderFrontend/RenderPipelineUI.h>
#include <RenderFrontend/ShadowRenderPass.h>
#include <RenderFrontend/SkySphereRenderPass.h>
#include <RenderFrontend/ToneMappingRenderPass.h>
#include <RenderFrontend/UIRenderItemBuilder.h>
#include <RenderSystem/IRenderSystem.h>
#include <RenderSystem/IWindowRenderManager.h>
#include <UiSystem/UICanvas.h>
#include <UiSystem/UIElement.h>
#include <UiSystem/UIManager.h>
#include <UiSystem/UIRenderableComponent.h>
#include <UiSystem/UITextComponent.h>
#include <algorithm>
#include <sstream>

bool Render::PooledRenderResource::isMatch(const RenderResourceDesc &rhs) const
{

    if (mDesc.mResourceFormat != rhs.mResourceFormat)
        return false;
    if (mDesc.mWidth != rhs.mWidth)
        return false;
    if (mDesc.mHeight != rhs.mHeight)
        return false;
    if (mDesc.mUsage != rhs.mUsage)
        return false;
    if (mDesc.mRtvFormat != rhs.mRtvFormat)
        return false;
    if (mDesc.mDsvFormat != rhs.mDsvFormat)
        return false;
    if (mDesc.mSrvFormat != rhs.mSrvFormat)
        return false;
    if (mDesc.mUavFormat != rhs.mUavFormat)
        return false;
    return true;
}

Render::RenderResourcePool::RenderResourcePool() {}

Render::RenderResourcePool::~RenderResourcePool()
{

    DestoryPool();
}

void Render::RenderResourcePool::Update(uint64_t currentFrame, uint64_t completedFenceValue)
{
    std::lock_guard<std::mutex> lock(mMutex);

    //    auto gpuResourceManager = GRM::IGpuResourceManager::GetInstance();
    for (auto it = mResourceVec.begin(); it != mResourceVec.end();)
    {

        PooledRenderResource *resource = *it;

        // 혹시모를 검사
        if (resource->mIsInUse)
        {
            ++it;
            continue;
        }

        bool isExpired = (currentFrame - resource->mLastUsedFrame) > mResourceExpirationFrame;

        bool isGpuFinished = completedFenceValue > resource->mLastUsedFence;

        if (isExpired && isGpuFinished)
        {
            // GpuResource 해제
            it = mResourceVec.erase(it);
            // 알아서 GRMPtr에서 해제요청
        }
        else
        {
            ++it;
        }
    }
}

Render::PooledRenderResource *Render::RenderResourcePool::Alloc(const RenderResourceDesc &desc)
{
    std::lock_guard<std::mutex> lock(mMutex);

    for (int i = 0; i < mResourceVec.size(); ++i)
    {

        if (mResourceVec[i]->isMatch(desc))
        {
            auto rec = mResourceVec[i];
            rec->mIsInUse = true;
            mResourceVec.erase(mResourceVec.begin() + i);

            if (rec->mResource.getResource()->GetCurrentResourceState() == EResourceState::ePresent)
            {
                int a = 2;
            }

            return rec;
        }
    }

    auto rec = Create(desc);
    rec->mIsInUse = true;
    if (rec->mResource.getResource()->GetCurrentResourceState() == EResourceState::ePresent)
    {
        int a = 2;
    }

    return rec;
}

void Render::RenderResourcePool::Free(PooledRenderResource *resource)
{
    std::lock_guard<std::mutex> lock(mMutex);

    // 반납
    //
    if (resource == nullptr)
        return;

    resource->mIsInUse = false;
    mResourceVec.push_back(resource);
}

void Render::RenderResourcePool::DestoryPool()
{

    for (auto resource : mResourceVec)
    {
        delete resource;
    }
}

Render::PooledRenderResource *Render::RenderResourcePool::Create(const RenderResourceDesc &desc)
{

    // GpuResourceManager  - > CreatTexture() ;
    auto gpuResourceManager = GRM::IGpuResourceManager::GetInstance();

    GRM::TextureDesc textureDesc;
    textureDesc.mTextureUsage = desc.mUsage;
    textureDesc.mScratchImage.mimagesNum = 1;
    textureDesc.mRtvFormat = desc.mRtvFormat;
    textureDesc.mDsvFormat = desc.mDsvFormat;
    textureDesc.mSrvFormat = desc.mSrvFormat;
    textureDesc.mUavFormat = desc.mUavFormat;

    GRM::Image image;
    image.mFormat = desc.mResourceFormat;
    image.mWidth = desc.mWidth;
    image.mHeight = desc.mHeight;
    textureDesc.mScratchImage.mImages.push_back(std::move(image));

    textureDesc.mScratchImage.mMetadata.mWidth = desc.mWidth;
    textureDesc.mScratchImage.mMetadata.mHeight = desc.mHeight;
    textureDesc.mScratchImage.mMetadata.mFormat = desc.mResourceFormat;
    textureDesc.mScratchImage.mMetadata.mDepth = 1;
    textureDesc.mScratchImage.mMetadata.mArraySize = 1;
    textureDesc.mScratchImage.mMetadata.mDimension = GRM::ETextureType::eTexture2D;
    textureDesc.mScratchImage.mMetadata.mMipLevels = 1;
    textureDesc.mOptimizedClearValue.mOptimizedDepthStencilValue.mOptimizedClearDepth = 1.0f;
    textureDesc.mOptimizedClearValue.mOptimizedDepthStencilValue.mOptimizedClearStencil = 0;

    GRM::GRMPtr resource = gpuResourceManager->CreateTexture(textureDesc);

    PooledRenderResource *renderResource = new PooledRenderResource;
    renderResource->mDesc = desc;
    renderResource->mIsInUse = false;
    renderResource->mResource = resource;

    return renderResource;
}

void Render::RenderContext::Reset()
{
    mRenderPassGraph->Reset();
    mRenderPassExecuteContext.mOpaqueMeshRenderCommandList.clear();
    mRenderPassExecuteContext.mTransparentMeshRenderCommandList.clear();
    mRenderPassExecuteContext.mEditorOverlayMeshRenderCommandList.clear();
    mRenderPassExecuteContext.mUIRenderCommandList.clear();
    mRenderPassExecuteContext.mUIIndexBuffer->clear();
    mRenderPassExecuteContext.mOutlineMeshRenderCommandIndexList.clear();
    mRenderPassExecuteContext.mUIVertexBuffer->clear();
    mRenderPassExecuteContext.mLightRenderCommandList.clear();
    mRenderPassExecuteContext.mBillboardRenderCommandList.clear();
    mRenderPassExecuteContext.mDebugLineRenderCommandList.clear();
    mRenderPassExecuteContext.mMaterialRenderSnapshotTable.clear();
    mRenderPassExecuteContext.mUIMaterialRenderSnapshotTable.clear();
    mRenderPassExecuteContext.mSkinPaletteSnapshot.clear();

    mRenderPassExecuteContext.mDirectonalShadowRenderData.mEnabled = false;
    mRenderPassExecuteContext.mDirectonalShadowRenderData.mLightIndex = -1;
    mRenderPassExecuteContext.mDirectonalShadowRenderData.mViewProj = CoreMath::Matrix4X4::Identity;
    mRenderPassExecuteContext.mDirectonalShadowRenderData.mShadowMapSize = 2048.0f;
}

Render::RenderContextPool::RenderContextPool() {}

Render::RenderContextPool::~RenderContextPool()
{
    DestoryPool();
}

void Render::RenderContextPool::Initalize(int poolSize)
{

    for (int i = 0; i < poolSize; ++i)
    {
        mRenderContextVec.push_back(new Render::RenderContext);
        mRenderContextVec[i]->mRenderPassGraph = std::make_unique<RenderPassGraph>();
    }
}

Render::RenderContext *Render::RenderContextPool::Alloc()
{

    // TODO mutex 동기화

    std::lock_guard lock(mMutex);

    Render::RenderContext *renderContext = mRenderContextVec.back();
    mRenderContextVec.pop_back();

    return renderContext;
}

void Render::RenderContextPool::Free(Render::RenderContext *context)
{

    std::lock_guard lock(mMutex);

    context->Reset();
    mRenderContextVec.push_back(context);
    return;
}

void Render::RenderContextPool::DestoryPool()
{

    for (auto resourceContext : mRenderContextVec)
    {
        delete resourceContext;
    }
}

void Render::RenderContextSetPool::Initialize(int poolSize)
{

    for (int i = 0; i < poolSize; ++i)
    {
        mRenderContextSetVec.push_back(new Render::RenderContextSet);
    }
}
Render::RenderContextSet *Render::RenderContextSetPool::Alloc()
{
    std::lock_guard lock(mMutex);

    CHECK(mRenderContextSetVec.size() != 0);

    RenderContextSet *contextSet = mRenderContextSetVec.back();

    mRenderContextSetVec.pop_back();

    return contextSet;
}
void Render::RenderContextSetPool::Free(RenderContextSet *contextSet)
{
    std::lock_guard lock(mMutex);

    contextSet->renderContextList.clear();

    mRenderContextSetVec.push_back(contextSet);
}

Render::RenderPipelineManager *Render::RenderPipelineManager::GetInstance()
{
    static RenderPipelineManager instance;

    return &instance;
}

Render::RenderPipelineManager::RenderPipelineManager()
{
    mRenderContextPool.Initalize();
    mRenderContextSetPool.Initialize();

    mRenderThread.Initialize(&mRenderContextPool, &mRenderContextSetPool);
    mUseThread = true;
}

Render::RenderPipelineManager::~RenderPipelineManager()
{
    //  EndRenderThread();
}

// TODO
// 매프레임 렌더패스들이 그래프를 구축하기전에 이번프레임에서 사용할 외부 TEX 리소스를 등록하는 메서드
void Render::RenderPipelineManager::InitRenderGraph(Core::LogicalWindow *window, RenderContext *renderContext)
{

    // Render::RenderChannelID renderChannelID = window->GetRenderChannelID();
    //  mRenderPassGraph[renderChannelID]->Reset();

    renderContext->mRenderPassGraph->Reset();
}
void Render::RenderPipelineManager::Update(uint64_t currentFrame, uint64_t completedFenceValue)
{
    mRenderResourcePool.Update(currentFrame, completedFenceValue);
}

void Render::RenderPipelineManager::BuildPassGraph(Core::LogicalWindow *window, RenderContext *renderContext)
{
    // 렌더패스를 렌더패스그래프에 등록한다.
    //
    //  조건에따라 일부렌더패스들만 등록

    const Core::WindowRenderConfig &windowRenderConfig = window->GetRenderConfig();

    //  RenderChannelID channelID = window->GetRenderChannelID();

    RenderPassSetUpData renderPassSetUpData;
    RenderPassSetUpData renderPhysicsWindowPassSetUpData;
    std::pair<int, int> windowSize = window->mViewportController.GetWindowSize();
    std::pair<int, int> physicsWindowSize = window->GetOwnerController()->GetWindowSize();

    // logical window size
    renderPassSetUpData.mWindowWidth = windowSize.first;
    renderPassSetUpData.mWindowHeight = windowSize.second;
    renderPhysicsWindowPassSetUpData.mWindowWidth = physicsWindowSize.first;
    renderPhysicsWindowPassSetUpData.mWindowHeight = physicsWindowSize.second;

    const float *backBufferClearColor = window->GetBackBufferClearColor();
    for (int i = 0; i < 4; ++i)
    {
        renderPassSetUpData.mBackBufferClearColor[i] = backBufferClearColor[i];
        renderPhysicsWindowPassSetUpData.mBackBufferClearColor[i] = backBufferClearColor[i];
    }

    RenderPassGraph *renderPassGraph = renderContext->mRenderPassGraph.get();
    std::string renderTargetBackBuffer = "BackBuffer";
    std::string depthStencilBuffer = "DepthStencilBuffer";

    if (windowRenderConfig.bIsOverlay)
    {

        // uipass  ,  backbuffer, depthstencilbuffer
        // no clear

        std::unique_ptr<IRenderPass> mainOpaqueUIPass = std::make_unique<RenderPassUI>();
        mainOpaqueUIPass->SetOutputTarget(renderTargetBackBuffer);
        mainOpaqueUIPass->SetOutputDepthStencil(depthStencilBuffer);
        mainOpaqueUIPass->SetClearRenderTarget(false);

        renderPassGraph->RegisterRenderPass(std::move(mainOpaqueUIPass), mainOpaqueUIPass->GetName(),
                                            renderPassSetUpData);
    }
    else
    {

        // pass
        std::string passName;

        std::string renderTargetTempBufferName = "TempBackBuffer";
        std::string depthStencilTempBuffer = "TempDepthStencilBuffer";
        std::string shadowMapBuffer = "DirectionalShadowMap";

        bool bClearRenderTarget = true;
        // debug grid
        if (windowRenderConfig.bDebugGrid)
        {

            std::unique_ptr<IRenderPass> debugGridPass = std::make_unique<RenderDebugGridPass>();
            debugGridPass->SetOutputTarget(renderTargetTempBufferName);
            debugGridPass->SetOutputDepthStencil(depthStencilTempBuffer);
            debugGridPass->SetClearRenderTarget(bClearRenderTarget);

            renderPassGraph->RegisterRenderPass(std::move(debugGridPass), debugGridPass->GetName(),
                                                renderPassSetUpData);
            bClearRenderTarget = false;
        }

        // shadow pass

        std::unique_ptr<IRenderPass> shadowPass = std::make_unique<ShadowRenderPass>();
        shadowPass->SetOutputDepthStencil(shadowMapBuffer);
        shadowPass->SetClearRenderTarget(false);
        renderPassGraph->RegisterRenderPass(std::move(shadowPass), shadowPass->GetName(), renderPassSetUpData);

        // Main Opaque
        std::unique_ptr<IRenderPass> mainOpaquePass = std::make_unique<RenderPassMain>();
        mainOpaquePass->SetOutputTarget(renderTargetTempBufferName);
        mainOpaquePass->SetOutputDepthStencil(depthStencilTempBuffer);
        mainOpaquePass->SetClearRenderTarget(bClearRenderTarget);
        if (bClearRenderTarget)
            bClearRenderTarget = false;

        renderPassGraph->RegisterRenderPass(std::move(mainOpaquePass), mainOpaquePass->GetName(), renderPassSetUpData);

        // Sky Sphere
        std::unique_ptr<IRenderPass> skySpherePass = std::make_unique<SkySphereRenderPass>();
        skySpherePass->SetOutputTarget(renderTargetTempBufferName);
        skySpherePass->SetOutputDepthStencil(depthStencilTempBuffer);
        skySpherePass->SetClearRenderTarget(false);

        renderPassGraph->RegisterRenderPass(std::move(skySpherePass), skySpherePass->GetName(), renderPassSetUpData);

        // Bloom horizontal
        std::string bloomHorizontalOutputBufferName = "BloomHoriOutput";
        std::unique_ptr<BloomRenderPass> bloomBrightHoriPass = std::make_unique<BloomRenderPass>();
        bloomBrightHoriPass->SetPassName("BloomHorizontal");
        bloomBrightHoriPass->SetInputTexSource(renderTargetTempBufferName);
        bloomBrightHoriPass->SetOutputTarget(bloomHorizontalOutputBufferName);
        renderPassGraph->RegisterRenderPass(std::move(bloomBrightHoriPass), bloomBrightHoriPass->GetName(),
                                            renderPassSetUpData);

        // Bloom vertical
        std::string bloomVerticalOutputBufferName = "BloomVerOutput";
        std::unique_ptr<BloomRenderPass> bloomVerPass = std::make_unique<BloomRenderPass>();
        bloomVerPass->SetBloomStage(Render::BloomRenderPass::EBloomStage::eVertical);
        bloomVerPass->SetPassName("BloomVertical");
        bloomVerPass->SetInputTexSource(bloomHorizontalOutputBufferName);
        bloomVerPass->SetOutputTarget(bloomVerticalOutputBufferName);
        renderPassGraph->RegisterRenderPass(std::move(bloomVerPass), bloomVerPass->GetName(), renderPassSetUpData);

        // Tone Mapping
        std::unique_ptr<ToneMappingRenderPass> toneMappingPass = std::make_unique<ToneMappingRenderPass>();
        toneMappingPass->SetInputSource(renderTargetTempBufferName);
        toneMappingPass->SetInputSourceTwo(bloomVerticalOutputBufferName); // 두개 바인딩가능하게해야수정할것
        toneMappingPass->SetOutputTarget(renderTargetBackBuffer);
        toneMappingPass->SetOutputDepthStencil(depthStencilBuffer);
        toneMappingPass->SetClearRenderTarget(true);

        renderPassGraph->RegisterRenderPass(std::move(toneMappingPass), toneMappingPass->GetName(),
                                            renderPassSetUpData);

        if (window->GetWorld())
        {
            // 패스 생성도 명령 생성과 동일한 맵 목록을 사용해야 비참여 맵의 outline이 섞이지 않는다.
            const auto &renderingMaps = window->GetWorld()->GetRenderingMaps();
            const bool hasOutline =
                std::any_of(renderingMaps.begin(), renderingMaps.end(),
                            [](const Map *map)
                            {
                                return map != nullptr && !ObjectRenderItemBuilder::GetInstance()
                                                              ->GetOutlineRenderProxyList(map->GetRenderID())
                                                              .empty();
                            });
            if (hasOutline)
            {
                std::unique_ptr<IRenderPass> outlinePass = std::make_unique<RenderOutlinePass>();
                outlinePass->SetOutputTarget(renderTargetBackBuffer);
                outlinePass->SetOutputDepthStencil(depthStencilBuffer);
                outlinePass->SetClearRenderTarget(false);
                renderPassGraph->RegisterRenderPass(std::move(outlinePass), outlinePass->GetName(),
                                                    renderPhysicsWindowPassSetUpData);
            }
        }

        // 이후로도 깊이판정은 일단 임시 백버퍼 사용
        //  UI Opaque
        std::unique_ptr<IRenderPass> mainOpaqueUIPass = std::make_unique<RenderPassUI>();
        mainOpaqueUIPass->SetOutputTarget(renderTargetBackBuffer);
        mainOpaqueUIPass->SetOutputDepthStencil(depthStencilBuffer);
        mainOpaqueUIPass->SetClearRenderTarget(false);

        renderPassGraph->RegisterRenderPass(std::move(mainOpaqueUIPass), mainOpaqueUIPass->GetName(),
                                            renderPhysicsWindowPassSetUpData);

        // billboardRenderPass
        std::unique_ptr<IRenderPass> billboardPass = std::make_unique<BillboardRenderPass>();
        billboardPass->SetOutputTarget(renderTargetBackBuffer);
        billboardPass->SetOutputDepthStencil(depthStencilBuffer);
        billboardPass->SetClearRenderTarget(false);

        renderPassGraph->RegisterRenderPass(std::move(billboardPass), billboardPass->GetName(),
                                            renderPhysicsWindowPassSetUpData);

        // debug line render pass
        std::unique_ptr<DebugLineRenderPass> debugLinePass = std::make_unique<DebugLineRenderPass>();
        debugLinePass->SetOutputTarget(renderTargetBackBuffer);
        debugLinePass->SetOutputDepthStencil(depthStencilBuffer);
        debugLinePass->SetClearRenderTarget(false);

        renderPassGraph->RegisterRenderPass(std::move(debugLinePass), debugLinePass->GetName(),
                                            renderPhysicsWindowPassSetUpData);

        // editor overlay pass
        std::unique_ptr<RenderEditOverlayPass> editorOverlayPass = std::make_unique<RenderEditOverlayPass>();
        editorOverlayPass->SetOutputTarget(renderTargetBackBuffer);
        editorOverlayPass->SetOutputDepthStencil(depthStencilBuffer);
        editorOverlayPass->SetClearRenderTarget(false);

        std::string editorOverlayPassName = editorOverlayPass->GetName();

        renderPassGraph->RegisterRenderPass(std::move(editorOverlayPass), editorOverlayPassName,
                                            renderPhysicsWindowPassSetUpData);

        //// Gray sacle Pass
        // std::unique_ptr<GrayScaleRenderPass> graySaclePass = std::make_unique<GrayScaleRenderPass>();
        // graySaclePass->SetInputSource(renderTargetTempBufferName);
        // graySaclePass->SetOutputTarget(renderTargetBackBuffer);
        // graySaclePass->SetOutputDepthStencil(depthStencilBuffer);
        // graySaclePass->SetClearRenderTarget(true);

        // std::string grayScalePassName = graySaclePass->GetName();

        // renderPassGraph->RegisterRenderPass(std::move(graySaclePass), grayScalePassName, renderPassSetUpData);
    }

    renderPassGraph->Compile();
}

void Render::RenderPipelineManager::ImportResource(Core::LogicalWindow *window, RenderContext *renderContext,
                                                   int backBufferIndex)
{
    IWindowRenderManager *windowRenderManager = IWindowRenderManager::GetInstance();

    RenderPassGraph *renderPassGraph = renderContext->mRenderPassGraph.get();

    // RenderChannelID renderChannelID = window->GetRenderChannelID();

    // back buffer import
    GRM::GRMPtr backBufffer = windowRenderManager->GetSwapchainBackBuffer(window->GetWindowHandle(), backBufferIndex);
    // 외부 리소스 등록
    // Back buffer
    renderPassGraph->Import("BackBuffer", backBufffer, EResourceState::eRenderTarget);

    GRM::GRMPtr depthStencilBuffer = windowRenderManager->GetDepthStencilBuffer(window->GetWindowHandle());
    // DepthStencil buffer
    renderPassGraph->Import("DepthStencilBuffer", depthStencilBuffer, EResourceState::eWriteDepthStencil);
}

void Render::RenderPipelineManager::ExcuteRenderPassGraph(Core::LogicalWindow *window, RenderContext *renderContext)
{
    // RenderChannelID renderChannelID = window->GetRenderChannelID();

    UI::UICanvasID canvasID = window->GetActiveCanvasID();
    // 먼저 렌더파이프라인-렌더패스들에대해서 globalFrameData를 설정해서
    // 전역데이터를 준비하도록,

    RenderPassExecuteContext &renderPassExecuteContext = renderContext->mRenderPassExecuteContext;
    renderPassExecuteContext.mGlobalSceneViewport = window->GetGlobalSceneViewport();
    renderPassExecuteContext.mGlobalFrameData = window->GetGlobalFrameData();
    // ObjectRenderItemBuilder 는 향후 이름이 Object렌더프록시매니저로바뀌고 역할도 제한될거다.;

    if (window->GetWorld())
        CreateRenderCommands(window->GetWorld(), renderPassExecuteContext);

    std::vector<UI::UIVertex> &UIVertexBuffer = *renderPassExecuteContext.mUIVertexBuffer;
    std::vector<uint32_t> &UIIndexBuffer = *renderPassExecuteContext.mUIIndexBuffer;

    if (canvasID != InvaildUICanvasID)
    {

        const std::vector<UI::UIRenderProxy *> &uiRenderProxyList =
            UI::UIManager::GetInstance()->GetCanvas(canvasID)->GetRenderProxyList();

        // TODO
        // UIRenderProxy - >UIRenderCommand 생성      -- 이부분이 렌더/게임 로직이 분리되는 부분 (멀티스레드)

        uint32_t vertexTotalNum = 0;
        uint32_t indexTotalNum = 0;
        for (auto uiRenderProxy : uiRenderProxyList)
        {
            uint32_t num = uiRenderProxy->mRenderableComponent->GetVertexNum();
            if (num == 0)
                continue;

            indexTotalNum += uiRenderProxy->mRenderableComponent->GetIndexNum();
            vertexTotalNum += num;
        }

        uint32_t vertexBufferOffset = UIVertexBuffer.size();
        UIVertexBuffer.resize(UIVertexBuffer.size() + vertexTotalNum);

        uint32_t indexBufferOffset = UIIndexBuffer.size();
        UIIndexBuffer.resize(UIIndexBuffer.size() + indexTotalNum);

        std::vector<UIRenderCommand> renderCommandList;

        uint32_t vn = 0;
        for (int i = 0; i < uiRenderProxyList.size(); ++i)
        {
            auto renderProxy = uiRenderProxyList[i];
            UIRenderCommand renderCommand;

            //  renderCommand.mUIMaterial = renderProxy->mRenderableComponent->GetUIMeshComponentPtr()->mUIMaterial;

            CoreAsset::Material *material = renderProxy->mRenderableComponent->GetUIMeshComponentPtr()->mUIMaterial;
            renderCommand.mUIMaterialID = material->GetID();

            renderCommand.mRole = renderProxy->mRenderableComponent->GetRenderRole();

            if (renderCommand.mRole == UI::UIRenderRole::eImage)
            {

                auto &albedoTexList = material->GetAlbedoTexResourceList();

                if (albedoTexList.empty() == false)
                    renderCommand.mMatSnapshot.mTextureAssetID = albedoTexList.back().mTexture.GetAssetID();
            }
            else if (renderCommand.mRole == UI::UIRenderRole::eFont)
            {

                auto textCom = static_cast<UI::UITextComponent *>(renderProxy->mRenderableComponent);

                auto font = textCom->GetFont();
                font->GetGlyphAltas();

                renderCommand.mMatSnapshot.mColor = textCom->GetColor().ConvertVector3();
                renderCommand.mMatSnapshot.mTextureAssetID = font->GetGlyphAltas().GetAssetID();
            }

            renderCommand.mUseScissorRect = renderProxy->mRenderableComponent->GetOwnerUIElement()->GetUseScissorRect();
            if (renderCommand.mUseScissorRect)
            {
                renderCommand.mScissorRect =
                    renderProxy->mRenderableComponent->GetOwnerUIElement()->GetScissorRectRegion();
            }
            else
            {
                renderCommand.mScissorRect = {0, 0, 0, 0};
            }

            renderCommand.mVertexNum = renderProxy->mRenderableComponent->GetVertexNum();

            // 버텍스개수가 0이면 생략
            if (renderCommand.mVertexNum == 0)
                continue;

            vn += renderCommand.mVertexNum;

            renderProxy->mRenderableComponent->GetVertices(&UIVertexBuffer[vertexBufferOffset]);
            renderCommand.mVertexStartOffset = vertexBufferOffset;
            vertexBufferOffset += renderCommand.mVertexNum;

            renderCommand.mIndexNum = renderProxy->mRenderableComponent->GetIndexNum();
            renderProxy->mRenderableComponent->GetIndices(&UIIndexBuffer[indexBufferOffset]);
            renderCommand.mIndexStartOffset = indexBufferOffset;
            indexBufferOffset += renderCommand.mIndexNum;

            renderCommandList.push_back(std::move(renderCommand));
        }

        if (vn != vertexTotalNum)
        {
            int a = 2;
        }

        renderPassExecuteContext.mUIRenderCommandList = std::move(renderCommandList);
    }
    renderPassExecuteContext.renderPassGraph = renderContext->mRenderPassGraph.get();
    renderPassExecuteContext.renderSystem = IRenderSystem::GetInstance();
    renderPassExecuteContext.mUIGlobalFrameData = window->GetUIGlobalFrameData();
}

void Render::RenderPipelineManager::CreateRenderCommands(World *world, RenderPassExecuteContext &executeContext)
{
    if (world == nullptr)
        return;

    // 환경 설정은 현재 맵이 기준이며, 추가 렌더링 맵마다 하늘/노출을 덮어쓰지 않는다.
    BuildSkysphereSnapshot(world, executeContext);
    BuildPostProcessingSnapshot(world, executeContext);

    // Map별 프록시 그룹을 한 뷰의 snapshot에 누적한다. 메모리에 남아 있는 비참여 맵은 조회하지 않는다.
    for (const Map *map : world->GetRenderingMaps())
        CreateMapRenderCommands(map, executeContext);

    // 모든 맵의 광원을 모은 뒤 한 번만 선택해야 light index가 최종 조명 목록을 가리킨다.
    executeContext.mDirectonalShadowRenderData.mEnabled = false;
    // directonal shadow light data 생성
    /*
        평행광의 위치는 카메라의 위치로부터 계산하다.

        그림자 구현시 현재 평행광의 위치를
     * 항상 원점을
     * 봐라보는게아니라 그 카메라를 따라가야할듯
        에디터에서 개발시에는 에디터의
     * 카메라를 따라가고
 플레이시에는 플레이카메라를 따라가야하는거지

    */
    for (size_t i = 0; i < executeContext.mLightRenderCommandList.size(); ++i)
    {
        if (executeContext.mLightRenderCommandList[i].mLightType == Core::ELightType::eDirectional)
        {

            executeContext.mDirectonalShadowRenderData.mEnabled = true;
            executeContext.mDirectonalShadowRenderData.mLightIndex = i;
            const CoreMath::Vector3 shadowCenter = executeContext.mGlobalFrameData.mCameraPositionWorld;
            const float shadowDistance = 1000.0f;
            CoreMath::Vector3 lightPosition =
                shadowCenter - executeContext.mLightRenderCommandList[i].mDirection * shadowDistance;

            CoreMath::Matrix4X4 view = CoreMath::Matrix4X4::MakeLookAtLH(lightPosition, shadowCenter,
                                                                         executeContext.mLightRenderCommandList[i].mUp);

            CoreMath::Matrix4X4 proj = CoreMath::Matrix4X4::MakeOrthographicLH(-1000, 1000, -1000, 1000, 1.0, 10000);

            CoreMath::Matrix4X4 viewProj = proj * view;

            executeContext.mDirectonalShadowRenderData.mViewProj = viewProj.GetTransposed();

            break;
        }
    }
}

void Render::RenderPipelineManager::CreateMapRenderCommands(const Map *map, RenderPassExecuteContext &executeContext)
{

    if (map == nullptr)
        return;

    uint32_t renderID = map->GetRenderID();

    // TODO 렌더
    //
    // 커맨드를 분류하는 단계로 수정해야한다.

    // 정적불투명 렌더커맨드
    // 정적투명
    // 정적에디터오버레이커맨드
    //  ... .
    auto *proxyContext = ObjectRenderItemBuilder::GetInstance()->GetRenderProxyContext(renderID);

    if (proxyContext == nullptr)
        return;

    const auto &lightProxyVec = proxyContext->mLightProxyList;
    // light Command 생성
    for (const auto proxy : lightProxyVec)
    {
        // 꺼진 조명은 GPU 업로드와 광원 개수, 평행광 그림자 선택에서 함께 제외한다.
        // 프록시 등록은 Owner의 active가, 발광 여부는 LightComponent가 각각 담당한다.
        if (proxy == nullptr || !proxy->mLightEnabled)
            continue;

        LightRenderCommand cmd;
        cmd.mDirection = proxy->mDirection;
        cmd.mRight = proxy->mRight;
        cmd.mUp = proxy->mUp;
        cmd.mFalloffEnd = proxy->mFalloffEnd;
        cmd.mFalloffStart = proxy->mFalloffStart;
        cmd.mLightType = proxy->mLightType;
        cmd.mPosition = proxy->mPosition;
        cmd.mSpotPower = proxy->mSpotPower;
        cmd.mStrength = proxy->mStrength;
        cmd.mSpotPower = proxy->mSpotPower;

        executeContext.mLightRenderCommandList.push_back(cmd);
    }

    // outline

    for (const auto proxy : proxyContext->mDrawOutlineProxyList)
    {
    }

    // billboard Command 생성

    for (const auto proxy : proxyContext->mBillboardProxyList)
    {
        BillboardRenderCommand cmd;
        cmd.mDrawOutline = false;
        cmd.mTexture = proxy->mTexture;
        cmd.mTransform = proxy->mTransform;
        cmd.mSize = proxy->mSize;

        executeContext.mBillboardRenderCommandList.push_back(cmd);
    }

    // debug line command 생성(복사)
    // 추가 맵의 debug line도 누적한다. 대입하면 앞서 수집한 맵의 선이 사라진다.
    executeContext.mDebugLineRenderCommandList.insert(executeContext.mDebugLineRenderCommandList.end(),
                                                      proxyContext->mDebugLineRenderCommandList.begin(),
                                                      proxyContext->mDebugLineRenderCommandList.end());

    // 분류된 Command 생성

    for (size_t i = 0; i < proxyContext->mRenderProxyList.size(); ++i)
    {
        auto renderProxy = proxyContext->mRenderProxyList[i];

        switch (renderProxy->mRenderProxyType)
        {
        case Core::ERenderProxyType::eStaticMesh:
        {
            std::vector<MeshRenderCommand> renderCommandList;
            // 공통만 처리

            Core::StaticMeshRenderProxy *staticMeshRenderProxy =
                static_cast<Core::StaticMeshRenderProxy *>(renderProxy);
            BuildMeshRenderCommands(renderCommandList, staticMeshRenderProxy, staticMeshRenderProxy->mStaticMesh,
                                    executeContext);

            for (size_t i = 0; i < staticMeshRenderProxy->mSubMeshMaterialList.size(); ++i)
            {
                if (staticMeshRenderProxy->mSubMeshOutlineFlagList[i])
                {
                    MeshOutlineRenderCommand outlineCommand;
                    outlineCommand.mGeometryType = renderCommandList[i].mGeometryType;
                    outlineCommand.mMaterialHandle = renderCommandList[i].mMaterialHandle;
                    outlineCommand.mMesh = renderCommandList[i].mMesh;
                    outlineCommand.mTransform = renderCommandList[i].mTransform;
                    outlineCommand.mSubMeshIndex = renderCommandList[i].mSubMeshIndex;

                    executeContext.mOutlineMeshRenderCommandIndexList.push_back(outlineCommand);
                }
            }

            if (staticMeshRenderProxy->mIsEditorOverlay)
            {
                executeContext.mEditorOverlayMeshRenderCommandList.insert(
                    executeContext.mEditorOverlayMeshRenderCommandList.end(), renderCommandList.begin(),
                    renderCommandList.end());
            }
            else
            {

                executeContext.mOpaqueMeshRenderCommandList.insert(executeContext.mOpaqueMeshRenderCommandList.end(),
                                                                   renderCommandList.begin(), renderCommandList.end());
            }
        }
        break;
        case Core::ERenderProxyType::eSkinningMesh:
        {

            // palette snapshot
            Core::SkeletalMeshRenderProxy *skeletalMeshRenderProxy =
                static_cast<Core::SkeletalMeshRenderProxy *>(renderProxy);
            // rendercommand  palette offset 설정
            uint32_t skinPaletteOffset = executeContext.mSkinPaletteSnapshot.size();
            uint32_t skinPaletteCount = skeletalMeshRenderProxy->mFinalMatrixList.size();

            if (skinPaletteCount == 0)
            { // renderCommand생성하지않는다.
                break;
            }
            // palette snapshot
            executeContext.mSkinPaletteSnapshot.insert(executeContext.mSkinPaletteSnapshot.end(),
                                                       skeletalMeshRenderProxy->mFinalMatrixList.begin(),
                                                       skeletalMeshRenderProxy->mFinalMatrixList.end());

            std::vector<MeshRenderCommand> renderCommandList;
            // 공통만 처리

            BuildMeshRenderCommands(renderCommandList, skeletalMeshRenderProxy, skeletalMeshRenderProxy->mSkinningMesh,
                                    executeContext);

            // palette snapshot
            for (auto &renderCommand : renderCommandList)
            {
                renderCommand.mSkinPaletteOffset = skinPaletteOffset;
                renderCommand.mSkinPaletteCount = skinPaletteCount;
            }

            // 분류해서 넣어야한다.
            for (size_t i = 0;
                 i < renderCommandList.size() && i < skeletalMeshRenderProxy->mSubMeshOutlineFlagList.size(); ++i)
            {
                if (skeletalMeshRenderProxy->mSubMeshOutlineFlagList[i])
                {
                    MeshOutlineRenderCommand outlineCommand;
                    // 기반 커맨드를 통째로 복사해 이 submesh의 palette offset/count도 outline에 보존한다.
                    static_cast<MeshRenderCommand &>(outlineCommand) = renderCommandList[i];

                    executeContext.mOutlineMeshRenderCommandIndexList.push_back(outlineCommand);
                }
            }

            if (skeletalMeshRenderProxy->mIsEditorOverlay)
            {
                executeContext.mEditorOverlayMeshRenderCommandList.insert(
                    executeContext.mEditorOverlayMeshRenderCommandList.end(), renderCommandList.begin(),
                    renderCommandList.end());
            }
            else
            {
                executeContext.mOpaqueMeshRenderCommandList.insert(executeContext.mOpaqueMeshRenderCommandList.end(),
                                                                   renderCommandList.begin(), renderCommandList.end());
            }
        }
        break;
        }
    }
    // DrawAABB처럼 한 프레임만 존재하는 디버그 메시도 일반 메시와 동일한 command snapshot 경로를 사용한다.
    // 일반 proxy 반복문과 분리해야 persistent proxy 개수와 무관하게 temp proxy를 정확히 한 번 처리할 수 있다.
    for (Core::RenderProxy *renderProxy : proxyContext->mTempRenderProxyList)
    {
        if (renderProxy == nullptr)
            continue;

        switch (renderProxy->mRenderProxyType)
        {
        case Core::ERenderProxyType::eStaticMesh:
        {
            Core::StaticMeshRenderProxy *staticMeshRenderProxy =
                static_cast<Core::StaticMeshRenderProxy *>(renderProxy);

            std::vector<MeshRenderCommand> renderCommandList;
            BuildMeshRenderCommands(renderCommandList, staticMeshRenderProxy, staticMeshRenderProxy->mStaticMesh,
                                    executeContext);

            for (size_t subMeshIndex = 0; subMeshIndex < renderCommandList.size() &&
                                          subMeshIndex < staticMeshRenderProxy->mSubMeshOutlineFlagList.size();
                 ++subMeshIndex)
            {
                if (staticMeshRenderProxy->mSubMeshOutlineFlagList[subMeshIndex])
                {
                    const MeshRenderCommand &renderCommand = renderCommandList[subMeshIndex];
                    MeshOutlineRenderCommand outlineCommand;
                    outlineCommand.mGeometryType = renderCommand.mGeometryType;
                    outlineCommand.mMaterialHandle = renderCommand.mMaterialHandle;
                    outlineCommand.mMesh = renderCommand.mMesh;
                    outlineCommand.mTransform = renderCommand.mTransform;
                    outlineCommand.mSubMeshIndex = renderCommand.mSubMeshIndex;

                    executeContext.mOutlineMeshRenderCommandIndexList.push_back(std::move(outlineCommand));
                }
            }

            if (staticMeshRenderProxy->mIsEditorOverlay)
            {
                executeContext.mEditorOverlayMeshRenderCommandList.insert(
                    executeContext.mEditorOverlayMeshRenderCommandList.end(), renderCommandList.begin(),
                    renderCommandList.end());
            }
            else
            {
                executeContext.mOpaqueMeshRenderCommandList.insert(executeContext.mOpaqueMeshRenderCommandList.end(),
                                                                   renderCommandList.begin(), renderCommandList.end());
            }
        }
        break;
        case Core::ERenderProxyType::eSkinningMesh:
        {
            Core::SkeletalMeshRenderProxy *skeletalMeshRenderProxy =
                static_cast<Core::SkeletalMeshRenderProxy *>(renderProxy);

            const uint32_t skinPaletteCount = static_cast<uint32_t>(skeletalMeshRenderProxy->mFinalMatrixList.size());
            if (skinPaletteCount == 0)
                break;

            const uint32_t skinPaletteOffset = static_cast<uint32_t>(executeContext.mSkinPaletteSnapshot.size());
            executeContext.mSkinPaletteSnapshot.insert(executeContext.mSkinPaletteSnapshot.end(),
                                                       skeletalMeshRenderProxy->mFinalMatrixList.begin(),
                                                       skeletalMeshRenderProxy->mFinalMatrixList.end());

            std::vector<MeshRenderCommand> renderCommandList;
            BuildMeshRenderCommands(renderCommandList, skeletalMeshRenderProxy, skeletalMeshRenderProxy->mSkinningMesh,
                                    executeContext);

            for (MeshRenderCommand &renderCommand : renderCommandList)
            {
                renderCommand.mSkinPaletteOffset = skinPaletteOffset;
                renderCommand.mSkinPaletteCount = skinPaletteCount;
            }

            for (size_t subMeshIndex = 0; subMeshIndex < renderCommandList.size() &&
                                          subMeshIndex < skeletalMeshRenderProxy->mSubMeshOutlineFlagList.size();
                 ++subMeshIndex)
            {
                if (!skeletalMeshRenderProxy->mSubMeshOutlineFlagList[subMeshIndex])
                    continue;

                MeshOutlineRenderCommand outlineCommand;
                static_cast<MeshRenderCommand &>(outlineCommand) = renderCommandList[subMeshIndex];
                executeContext.mOutlineMeshRenderCommandIndexList.push_back(std::move(outlineCommand));
            }

            if (skeletalMeshRenderProxy->mIsEditorOverlay)
            {
                executeContext.mEditorOverlayMeshRenderCommandList.insert(
                    executeContext.mEditorOverlayMeshRenderCommandList.end(), renderCommandList.begin(),
                    renderCommandList.end());
            }
            else
            {
                executeContext.mOpaqueMeshRenderCommandList.insert(executeContext.mOpaqueMeshRenderCommandList.end(),
                                                                   renderCommandList.begin(), renderCommandList.end());
            }
        }
        break;
        }
    }
}
void Render::RenderPipelineManager::BuildSkysphereSnapshot(World *world, RenderPassExecuteContext &executeContext)
{

    if (world == nullptr)
        return;

    Map *map = world->GetCurrentMap();

    if (map == nullptr)
        return;

    const Core::SkySphereSettings &skySphereSettings = map->GetSkySphereSettings();

    if (skySphereSettings.mEnable == false)
    {
        executeContext.mSkySphereSnapshot.mActiveFlag = false;
        return;
    }
    CoreAsset::Texture *skyTexture = CoreAsset::AssetManager::GetInstance()
                                         ->GetAsset<CoreAsset::Texture>(skySphereSettings.mTexID)
                                         .As<CoreAsset::Texture>();
    if (skyTexture == nullptr)
    {
        executeContext.mSkySphereSnapshot.mActiveFlag = false;
        return;
    }
    executeContext.mSkySphereSnapshot.mSkyTexture = skyTexture;
    executeContext.mSkySphereSnapshot.mSphereMesh = CoreAsset::AssetManager::GetInstance()
                                                        ->GetAsset<CoreAsset::StaticMesh>("Engine/SkySphere")
                                                        .As<CoreAsset::StaticMesh>();

    executeContext.mSkySphereSnapshot.mTransform =
        CoreMath::Matrix4X4::MakeTranslation(executeContext.mGlobalFrameData.mCameraPositionWorld) *
        CoreMath::Matrix4X4::MakeScale(skySphereSettings.mRadius);

    executeContext.mSkySphereSnapshot.mActiveFlag = true;
}

void Render::RenderPipelineManager::BuildPostProcessingSnapshot(World *world, RenderPassExecuteContext &executeContext)
{

    if (world == nullptr)
        return;

    Map *map = world->GetCurrentMap();

    if (map == nullptr)
        return;

    const Core::PostProcessingSettings &postProcessingSettings = map->GetPostProcessingSettings();
    executeContext.mPostProcessingData.mExposure = postProcessingSettings.mExposure;
}

void Render::RenderPipelineManager::BuildMeshRenderCommands(std::vector<MeshRenderCommand> &oMeshRenderCommands,
                                                            Core::MeshRenderProxy *meshRenderProxy,
                                                            CoreAsset::Mesh *mesh,
                                                            RenderPassExecuteContext &executeContext)
{

    std::unordered_map<uint32_t, MaterialRenderSnapshot> &materialSnapshotTable =
        executeContext.mMaterialRenderSnapshotTable;

    for (int matIndex = 0; matIndex < meshRenderProxy->mSubMeshMaterialList.size(); ++matIndex)
    {
        MeshRenderCommand renderCommand;
        renderCommand.mMesh = mesh;

        if (meshRenderProxy->mRenderProxyType == Core::ERenderProxyType::eStaticMesh)
            renderCommand.mGeometryType = Render::ERenderGeometryType::eStaticMesh;
        else if (meshRenderProxy->mRenderProxyType == Core::ERenderProxyType::eSkinningMesh)
            renderCommand.mGeometryType = Render::ERenderGeometryType::eSkinnedMesh;

        CoreAsset::Material *material = meshRenderProxy->mSubMeshMaterialList[matIndex];
        // renderCommand.mMaterial = staticMeshRenderProxy->mSubMeshMaterialList[matIndex];
        renderCommand.mTransform = meshRenderProxy->mTransform;
        renderCommand.mDrawOutline = meshRenderProxy->mDrawOutline;
        renderCommand.mCustomShaderData = meshRenderProxy->mCustomShaderData;
        renderCommand.mSubMeshIndex = matIndex;

        // auto material = renderCommand.mMaterial;
        uint32_t materialHandle = material->GetMaterialHandle();
        renderCommand.mMaterialHandle = materialHandle;

        // 머터리얼 수집

        //  materialHandleTable[materialHandle] = material;

        if (materialSnapshotTable.find(materialHandle) == materialSnapshotTable.end())
        {

            materialSnapshotTable[materialHandle] = GetMaterialSnapshot(material);

            material->ClearUploadDirty();
        }

        oMeshRenderCommands.push_back(std::move(renderCommand));
    }
}

Render::MaterialRenderSnapshot Render::RenderPipelineManager::GetMaterialSnapshot(CoreAsset::Material *material) const
{
    MaterialRenderSnapshot materialRenderSnapshot;
    materialRenderSnapshot.mHandle = material->GetMaterialHandle();
    materialRenderSnapshot.mDiffuseFactor = material->GetDiffuseColor() * material->GetDiffuseFactor();
    materialRenderSnapshot.mMetallic = material->GetMetallic();
    materialRenderSnapshot.mRoughness = material->GetRoughness();
    materialRenderSnapshot.mUseExplicitGpuMat = material->GetUseExplicitGpuMaterial();
    materialRenderSnapshot.mUVTiling = material->GetUVTiling();
    materialRenderSnapshot.mUVRotationPivot = material->GetUVRotationPivot();
    materialRenderSnapshot.mUVRotation = material->GetUVRotation();
    materialRenderSnapshot.mShadingModel = material->GetShadingMode();
    materialRenderSnapshot.mAmbient = material->GetAmbient();
    materialRenderSnapshot.mEmissiveColor = material->GetEmissiveColor();
    materialRenderSnapshot.mEmissiveIntensity = material->GetEmissiveIntensity();
    materialRenderSnapshot.mMaterialAssetID = material->GetID();

    for (const auto &texContext : material->GetAlbedoTexResourceList())
    {
        materialRenderSnapshot.mAlbedoMapList.push_back(texContext.mTexture.As<CoreAsset::Texture>());
    }

    if (material->HasNormalMap())
    {
        materialRenderSnapshot.mNormalMap = material->GetNormalTexResource().mTexture.As<CoreAsset::Texture>();
    }
    if (material->GetUploadDirty())
    {
        materialRenderSnapshot.mMaterialUploadDirtyFlag = true;
    }

    return materialRenderSnapshot;
}

void Render::RenderPipelineManager::Execute(const std::vector<Core::LogicalWindow *> &logicalWindowList,
                                            WindowHandle windowHandle, int frameIndex, uint32_t frameFenceValue,
                                            bool lastExecute, uint64_t frameTotalCount)
{

    IRenderSystem *renderSystem = IRenderSystem::GetInstance();
    IWindowRenderManager *windowRenderManager = IWindowRenderManager::GetInstance();

    RenderContextSet *renderContextSet = mRenderContextSetPool.Alloc();
    renderContextSet->mWindowHandle = windowHandle;
    renderContextSet->mFenceValue = frameFenceValue;
    renderContextSet->mFrameIndex = frameIndex;
    renderContextSet->mLastFrameContextSet = lastExecute;
    renderContextSet->mBackbufferIndex = windowRenderManager->GetNextSwapchainBackBufferIndex(windowHandle);
    windowRenderManager->IncrementNextSwapchainBackBufferIndex(windowHandle);

    for (int i = 0; i < logicalWindowList.size(); ++i)
    {

        RenderContext *renderContext = mRenderContextPool.Alloc();
        renderContext->mRenderPassExecuteContext.mUIVertexBuffer = &renderContextSet->mUIVertexBuffer;
        renderContext->mRenderPassExecuteContext.mUIIndexBuffer = &renderContextSet->mUIIndexBuffer;

        renderContext->mRenderPassExecuteContext.mUIGlobalFrameData = logicalWindowList[i]->GetUIGlobalFrameData();
        renderContext->mRenderPassExecuteContext.mGlobalFrameData = logicalWindowList[i]->GetGlobalFrameData();

        renderContext->mRenderPassGraph->SetFenceValue(renderContextSet->mFenceValue);
        renderContext->mRenderPassGraph->SetFrameCount(frameTotalCount);

        InitRenderGraph(logicalWindowList[i], renderContext);

        ImportResource(logicalWindowList[i], renderContext, renderContextSet->mBackbufferIndex);

        BuildPassGraph(logicalWindowList[i], renderContext);

        ExcuteRenderPassGraph(logicalWindowList[i], renderContext);

        renderContextSet->renderContextList.push_back(renderContext);
    }

    if (mUseThread)
    {
        mRenderThread.PushRenderContextSet(renderContextSet);
        // PushRenderContextSetQueue(renderContextSet);
    }
    else
    {
        for (int i = 0; i < renderContextSet->renderContextList.size(); ++i)
        {
            RenderContext *renderContext = renderContextSet->renderContextList[i];
            renderContextSet->renderContextList[i]->mRenderPassGraph->Execute(renderContext->mRenderPassExecuteContext);
            mRenderContextPool.Free(renderContext);
        }

        mRenderContextSetPool.Free(renderContextSet);
    }
}

void Render::RenderPipelineManager::EndFrame() {}

int Render::RenderPipelineManager::WindowResize(void *windowHandle)
{
    mRenderThread.FlushAndStop(windowHandle);
    // RenderThread가 끝낼때까지 기다린다.

    return IRenderSystem::GetInstance()->WindowResize(WindowHandle(windowHandle));
}

Render::RenderContext *Render::RenderPipelineManager::GetFreeRenderContext()
{
    // TODO 동기화

    return nullptr;
}

void Render::RenderPipelineManager::ReturnRenderContext(RenderContext *renderContext)
{
    // TODO 동기화
}

void Render::RenderPipelineManager::EndRenderThread()
{
    if (mUseThread)
    {
        mRenderThread.EndThread();
    }
}

// void Render::RenderPipelineManager::PushRenderContextSetQueue(RenderContextSet *renderContextSet)
//{
//     // TODO 동기화
//
//     std::unique_lock lock(mRenderContextQueueMutex);
//
//     mRenderContextSetQueue.push(renderContextSet);
//
//     // 기다리는 RenderThread깨우기
//     // 조건변수
//
//     mCV_Render.notify_one();
// }

// void Render::RenderPipelineManager::StartRenderThread()
//{
//     mRenderThread = std::thread(&Render::RenderPipelineManager::RenderThreadLoop, this);
// }
// void Render::RenderPipelineManager::EndRenderThread()
//{
//
//     mRenderThreadRunning = false;
//
//     // 강제 알림을 보내 자고있는 렌더스레드를 꺠운다.
//
//     // 그리고 기다린다.
//     //
//     if (mRenderThread.joinable())
//     {
//         mRenderThread.join();
//     }
// }
