#include "RenderPipelineManager.h"
#include "RenderDebugGridPass.h"
#include <Core/LogicalWindow.h>
#include <Core/Map.h>
#include <Core/World.h>
#include <CoreAsset/AssetManager.h>
#include <CoreAsset/Material.h>
#include <CoreAsset/StaticMesh.h>
#include <CoreBase/CoreAssert.h>
#include <D3DGpuResourceManager/IGpuResourceManager.h>
#include <RenderFrontend/BillboardRenderPass.h>
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
#include <RenderFrontend/SkySphereRenderPass.h>
#include <RenderFrontend/UIRenderItemBuilder.h>
#include <RenderSystem/IRenderSystem.h>
#include <RenderSystem/IWindowRenderManager.h>
#include <UiSystem/UICanvas.h>
#include <UiSystem/UIElement.h>
#include <UiSystem/UIManager.h>
#include <UiSystem/UIRenderableComponent.h>
#include <sstream>

bool Render::PooledRenderResource::isMatch(const RenderResourceDesc &rhs) const
{

    if (mDesc.mFormat != rhs.mFormat)
        return false;
    if (mDesc.mWidth != rhs.mWidth)
        return false;
    if (mDesc.mHeight != rhs.mHeight)
        return false;
    if (mDesc.mUsage != rhs.mUsage)
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

    GRM::Image image;
    image.mFormat = desc.mFormat;
    image.mWidth = desc.mWidth;
    image.mHeight = desc.mHeight;
    textureDesc.mScratchImage.mImages.push_back(std::move(image));

    textureDesc.mScratchImage.mMetadata.mWidth = desc.mWidth;
    textureDesc.mScratchImage.mMetadata.mHeight = desc.mHeight;
    textureDesc.mScratchImage.mMetadata.mFormat = desc.mFormat;
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
    mRenderPassExecuteContext.mOpaqueStaticMeshRenderCommandList.clear();
    mRenderPassExecuteContext.mTransparentStaticMeshRenderCommandList.clear();
    mRenderPassExecuteContext.mEditorOverlayStaticMeshRenderCommandList.clear();
    mRenderPassExecuteContext.mUIRenderCommandList.clear();
    mRenderPassExecuteContext.mUIIndexBuffer->clear();
    mRenderPassExecuteContext.mOutlineStaticMeshRenderCommandIndexList.clear();
    mRenderPassExecuteContext.mUIVertexBuffer->clear();
    mRenderPassExecuteContext.mLightRenderCommandList.clear();
    mRenderPassExecuteContext.mBillboardRenderCommandList.clear();
    mRenderPassExecuteContext.mDebugLineRenderCommandList.clear();
    mRenderPassExecuteContext.mMaterialRenderSnapshotTable.clear();
    mRenderPassExecuteContext.mUIMaterialRenderSnapshotTable.clear();
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
    std::pair<int, int> windowSize = window->mViewportController.GetWindowSize();

    renderPassSetUpData.mWindowWidth = windowSize.first;
    renderPassSetUpData.mWindowHeight = windowSize.second;

    const float *backBufferClearColor = window->GetBackBufferClearColor();
    for (int i = 0; i < 4; ++i)
    {
        renderPassSetUpData.mBackBufferClearColor[i] = backBufferClearColor[i];
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

        if (window->GetMap())
        {

            if (ObjectRenderItemBuilder::GetInstance()
                    ->GetOutlineRenderProxyList(window->GetWorld()->GetRenderID())
                    .size() != 0)
            {
                std::unique_ptr<IRenderPass> outlinePass = std::make_unique<RenderOutlinePass>();
                outlinePass->SetOutputTarget(renderTargetTempBufferName);
                outlinePass->SetOutputDepthStencil(depthStencilTempBuffer);
                outlinePass->SetClearRenderTarget(false);
                renderPassGraph->RegisterRenderPass(std::move(outlinePass), outlinePass->GetName(),
                                                    renderPassSetUpData);
            }
        }

        // UI Opaque
        std::unique_ptr<IRenderPass> mainOpaqueUIPass = std::make_unique<RenderPassUI>();
        mainOpaqueUIPass->SetOutputTarget(renderTargetTempBufferName);
        mainOpaqueUIPass->SetOutputDepthStencil(depthStencilTempBuffer);
        mainOpaqueUIPass->SetClearRenderTarget(false);

        renderPassGraph->RegisterRenderPass(std::move(mainOpaqueUIPass), mainOpaqueUIPass->GetName(),
                                            renderPassSetUpData);

        // billboardRenderPass
        std::unique_ptr<IRenderPass> billboardPass = std::make_unique<BillboardRenderPass>();
        billboardPass->SetOutputTarget(renderTargetTempBufferName);
        billboardPass->SetOutputDepthStencil(depthStencilTempBuffer);
        billboardPass->SetClearRenderTarget(false);

        renderPassGraph->RegisterRenderPass(std::move(billboardPass), billboardPass->GetName(), renderPassSetUpData);

        // debug line render pass
        std::unique_ptr<DebugLineRenderPass> debugLinePass = std::make_unique<DebugLineRenderPass>();
        debugLinePass->SetOutputTarget(renderTargetTempBufferName);
        debugLinePass->SetOutputDepthStencil(depthStencilTempBuffer);
        debugLinePass->SetClearRenderTarget(false);

        renderPassGraph->RegisterRenderPass(std::move(debugLinePass), debugLinePass->GetName(), renderPassSetUpData);

        // editor overlay pass
        std::unique_ptr<RenderEditOverlayPass> editorOverlayPass = std::make_unique<RenderEditOverlayPass>();
        editorOverlayPass->SetOutputTarget(renderTargetTempBufferName);
        editorOverlayPass->SetOutputDepthStencil(depthStencilTempBuffer);
        editorOverlayPass->SetClearRenderTarget(false);

        std::string editorOverlayPassName = editorOverlayPass->GetName();

        renderPassGraph->RegisterRenderPass(std::move(editorOverlayPass), editorOverlayPassName, renderPassSetUpData);

        // Gray sacle Pass
        std::unique_ptr<GrayScaleRenderPass> graySaclePass = std::make_unique<GrayScaleRenderPass>();
        graySaclePass->SetInputSource(renderTargetTempBufferName);
        graySaclePass->SetOutputTarget(renderTargetBackBuffer);
        graySaclePass->SetOutputDepthStencil(depthStencilBuffer);
        graySaclePass->SetClearRenderTarget(true);

        std::string grayScalePassName = graySaclePass->GetName();

        renderPassGraph->RegisterRenderPass(std::move(graySaclePass), grayScalePassName, renderPassSetUpData);
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
            //= renderCommandList[i];

            //  renderCommand.mUIMaterial = renderProxy->mRenderableComponent->GetUIMeshComponentPtr()->mUIMaterial;

            CoreAsset::Material *material = renderProxy->mRenderableComponent->GetUIMeshComponentPtr()->mUIMaterial;
            renderCommand.mUIMaterialID = material->GetID();
            renderCommand.mGpuMaterialID = material->GetGpuMaterialID();

            if (renderPassExecuteContext.mUIMaterialRenderSnapshotTable.find(renderCommand.mUIMaterialID) ==
                renderPassExecuteContext.mUIMaterialRenderSnapshotTable.end())
            {

                MaterialRenderSnapshot materialRenderSnapshot;
                materialRenderSnapshot.mDiffuseFactor = material->GetDiffuseColor() * material->GetDiffuseFactor();
                materialRenderSnapshot.mMetallic = material->GetMetallic();
                materialRenderSnapshot.mRoughness = material->GetRoughness();
                materialRenderSnapshot.mUseExplicitGpuMat = material->GetUseExplicitGpuMaterial();
                materialRenderSnapshot.mGpuMatID = material->GetGpuMaterialID();
                materialRenderSnapshot.mShadingModel = material->GetShadingMode();

                for (const auto &texContext : material->GetAlbedoTexResourceList())
                {
                    materialRenderSnapshot.mAlbedoMapList.push_back(texContext.mTexture.As<CoreAsset::Texture>());
                }

                if (material->HasNormalMap())
                {
                    materialRenderSnapshot.mNormalMap =
                        material->GetNormalTexResource().mTexture.As<CoreAsset::Texture>();
                }
                // if (material->GetUploadDirty())
                // {
                //     materialRenderSnapshot.mMaterialUploadDirtyFlag = true;
                //  }

                renderPassExecuteContext.mUIMaterialRenderSnapshotTable[renderCommand.mUIMaterialID] =
                    materialRenderSnapshot;

                // material->ClearUploadDirty();
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

    uint32_t renderID = world->GetRenderID();

    // TODO 렌더
    //
    // 커맨드를 분류하는 단계로 수정해야한다.

    // 정적불투명 렌더커맨드
    // 정적투명
    // 정적에디터오버레이커맨드
    //  ... .
    const auto &proxyVec = ObjectRenderItemBuilder::GetInstance()->GetRenderProxyList(renderID);

    auto *proxyContext = ObjectRenderItemBuilder::GetInstance()->GetRenderProxyContext(renderID);

    if (proxyContext == nullptr)
        return;

    const auto &lightProxyVec = proxyContext->mLightProxyList;
    // light Command 생성
    for (const auto proxy : lightProxyVec)
    {
        LightRenderCommand cmd;
        cmd.mDirection = proxy->mDirection;
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
    executeContext.mDebugLineRenderCommandList = proxyContext->mDebugLineRenderCommandList;

    // SkySphere snapShot
    BuildSkysphereSnapshot(world, executeContext);
    // 분류된 Command 생성

    //<material handle, material >
    // std::unordered_map<uint32_t, CoreAsset::Material *> materialHandleTable;
    std::unordered_map<uint32_t, MaterialRenderSnapshot> &materialSnapshotTable =
        executeContext.mMaterialRenderSnapshotTable;

    for (size_t i = 0; i < proxyContext->mRenderProxyList.size(); ++i)
    {
        auto renderProxy = proxyContext->mRenderProxyList[i];

        switch (renderProxy->mRenderProxyType)
        {
        case Core::ERenderProxyType::eStaticMesh:
        {
            // TODO staticRenderItem 생성
            Core::StaticMeshRenderProxy *staticMeshRenderProxy =
                static_cast<Core::StaticMeshRenderProxy *>(renderProxy);

            for (int matIndex = 0; matIndex < staticMeshRenderProxy->mSubMeshMaterialList.size(); ++matIndex)
            {
                StaticMeshRenderCommnad renderCommand;
                renderCommand.mStaticMesh = staticMeshRenderProxy->mStaticMesh;

                CoreAsset::Material *material = staticMeshRenderProxy->mSubMeshMaterialList[matIndex];
                // renderCommand.mMaterial = staticMeshRenderProxy->mSubMeshMaterialList[matIndex];
                renderCommand.mTransform = staticMeshRenderProxy->mTransform;
                renderCommand.mDrawOutline = staticMeshRenderProxy->mDrawOutline;
                renderCommand.mCustomShaderData = staticMeshRenderProxy->mCustomShaderData;
                renderCommand.mSubMeshIndex = matIndex;

                // auto material = renderCommand.mMaterial;
                uint32_t materialHandle = material->GetMaterialHandle();
                renderCommand.mMaterialHandle = materialHandle;

                // 머터리얼 수집

                //  materialHandleTable[materialHandle] = material;

                if (materialSnapshotTable.find(materialHandle) == materialSnapshotTable.end())
                {
                    // 더티 머터리얼 수집
                    MaterialRenderSnapshot materialRenderSnapshot;
                    materialRenderSnapshot.mHandle = materialHandle;
                    materialRenderSnapshot.mDiffuseFactor = material->GetDiffuseColor() * material->GetDiffuseFactor();
                    materialRenderSnapshot.mMetallic = material->GetMetallic();
                    materialRenderSnapshot.mRoughness = material->GetRoughness();
                    materialRenderSnapshot.mUseExplicitGpuMat = material->GetUseExplicitGpuMaterial();
                    materialRenderSnapshot.mGpuMatID = material->GetGpuMaterialID();
                    materialRenderSnapshot.mShadingModel = material->GetShadingMode();
                    materialRenderSnapshot.mAmbient = material->GetAmbient();

                    for (const auto &texContext : material->GetAlbedoTexResourceList())
                    {
                        materialRenderSnapshot.mAlbedoMapList.push_back(texContext.mTexture.As<CoreAsset::Texture>());
                    }

                    if (material->HasNormalMap())
                    {
                        materialRenderSnapshot.mNormalMap =
                            material->GetNormalTexResource().mTexture.As<CoreAsset::Texture>();
                    }
                    if (material->GetUploadDirty())
                    {
                        materialRenderSnapshot.mMaterialUploadDirtyFlag = true;
                    }

                    materialSnapshotTable[materialHandle] = materialRenderSnapshot;

                    material->ClearUploadDirty();
                }

                if (staticMeshRenderProxy->mSubMeshOutlineFlagList[matIndex])
                {
                    StaticMeshOutlineRenderCommand outlineCommand;
                    outlineCommand.mMaterialHandle = renderCommand.mMaterialHandle;
                    outlineCommand.mStaticMesh = renderCommand.mStaticMesh;
                    outlineCommand.mTransform = renderCommand.mTransform;
                    outlineCommand.mSubMeshIndex = renderCommand.mSubMeshIndex;

                    executeContext.mOutlineStaticMeshRenderCommandIndexList.push_back(outlineCommand);
                }

                // 분류해서 넣어야한다.

                if (staticMeshRenderProxy->mIsEditorOverlay)
                {
                    executeContext.mEditorOverlayStaticMeshRenderCommandList.push_back(std::move(renderCommand));
                }
                else
                {
                    executeContext.mOpaqueStaticMeshRenderCommandList.push_back(std::move(renderCommand));
                }
            }
            break;
        }
        case Core::ERenderProxyType::eSkinningMesh:
        {
        }
        break;
        }
    }

    for (size_t i = 0; i < proxyContext->mTempRenderProxyList.size(); ++i)
    {
        auto renderProxy = proxyContext->mTempRenderProxyList[i];

        switch (renderProxy->mRenderProxyType)
        {
        case Core::ERenderProxyType::eStaticMesh:
        {
            // TODO staticRenderItem 생성
            Core::StaticMeshRenderProxy *staticMeshRenderProxy =
                static_cast<Core::StaticMeshRenderProxy *>(renderProxy);

            for (int matIndex = 0; matIndex < staticMeshRenderProxy->mSubMeshMaterialList.size(); ++matIndex)
            {
                StaticMeshRenderCommnad renderCommand;
                renderCommand.mStaticMesh = staticMeshRenderProxy->mStaticMesh;

                CoreAsset::Material *material = staticMeshRenderProxy->mSubMeshMaterialList[matIndex];

                renderCommand.mMaterialHandle = material->GetMaterialHandle();
                renderCommand.mTransform = staticMeshRenderProxy->mTransform;
                renderCommand.mDrawOutline = staticMeshRenderProxy->mDrawOutline;
                renderCommand.mCustomShaderData = staticMeshRenderProxy->mCustomShaderData;

                if (materialSnapshotTable.find(renderCommand.mMaterialHandle) == materialSnapshotTable.end())
                {
                    // 더티 머터리얼 수집
                    MaterialRenderSnapshot materialRenderSnapshot;
                    materialRenderSnapshot.mHandle = renderCommand.mMaterialHandle;
                    materialRenderSnapshot.mDiffuseFactor = material->GetDiffuseColor() * material->GetDiffuseFactor();
                    materialRenderSnapshot.mMetallic = material->GetMetallic();
                    materialRenderSnapshot.mRoughness = material->GetRoughness();
                    materialRenderSnapshot.mUseExplicitGpuMat = material->GetUseExplicitGpuMaterial();
                    materialRenderSnapshot.mGpuMatID = material->GetGpuMaterialID();
                    materialRenderSnapshot.mShadingModel = material->GetShadingMode();
                    materialRenderSnapshot.mAmbient = material->GetAmbient();

                    for (const auto &texContext : material->GetAlbedoTexResourceList())
                    {
                        materialRenderSnapshot.mAlbedoMapList.push_back(texContext.mTexture.As<CoreAsset::Texture>());
                    }

                    if (material->HasNormalMap())
                    {
                        materialRenderSnapshot.mNormalMap =
                            material->GetNormalTexResource().mTexture.As<CoreAsset::Texture>();
                    }
                    if (material->GetUploadDirty())
                    {
                        materialRenderSnapshot.mMaterialUploadDirtyFlag = true;
                    }
                    materialSnapshotTable[renderCommand.mMaterialHandle] = materialRenderSnapshot;
                    material->ClearUploadDirty();
                }

                if (staticMeshRenderProxy->mSubMeshOutlineFlagList[matIndex])
                {
                    StaticMeshOutlineRenderCommand outlineCommand;
                    outlineCommand.mMaterialHandle = renderCommand.mMaterialHandle;
                    outlineCommand.mStaticMesh = renderCommand.mStaticMesh;
                    outlineCommand.mTransform = renderCommand.mTransform;
                    outlineCommand.mSubMeshIndex = renderCommand.mSubMeshIndex;

                    executeContext.mOutlineStaticMeshRenderCommandIndexList.push_back(outlineCommand);
                }

                // 분류해서 넣어야한다.

                if (staticMeshRenderProxy->mIsEditorOverlay)
                {
                    executeContext.mEditorOverlayStaticMeshRenderCommandList.push_back(std::move(renderCommand));
                }
                else
                {
                    executeContext.mOpaqueStaticMeshRenderCommandList.push_back(std::move(renderCommand));
                }
            }
            break;
        }
        case Core::ERenderProxyType::eSkinningMesh:
        {
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

void Render::RenderPipelineManager::Execute(const std::vector<Core::LogicalWindow *> &logicalWindowList,
                                            WindowHandle windowHandle, int frameIndex, uint32_t frameFenceValue,
                                            bool lastExecute, int backBufferIndex, uint64_t frameTotalCount)
{

    IRenderSystem *renderSystem = IRenderSystem::GetInstance();
    IWindowRenderManager *windowRenderManager = IWindowRenderManager::GetInstance();

    RenderContextSet *renderContextSet = mRenderContextSetPool.Alloc();
    renderContextSet->mWindowHandle = windowHandle;
    renderContextSet->mFenceValue = frameFenceValue;
    renderContextSet->mFrameIndex = frameIndex;
    renderContextSet->mLastFrameContextSet = lastExecute;
    renderContextSet->mBackbufferIndex = backBufferIndex;

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
