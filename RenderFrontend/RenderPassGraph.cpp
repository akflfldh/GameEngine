#include "RenderPassGraph.h"

#include <CoreBase/CoreAssert.h>
#include <CoreDevice/D3DCoreDevice.h>
#include <D3DGpuResourceManager/GpuBufferContextSystem.h>
#include <D3DGpuResourceManager/IGpuResourceManager.h>
#include <Logger/Logger.h>
#include <RenderFrontend/AssetResolver.h>
#include <RenderFrontend/RenderPipelineManager.h>
#include <RenderFrontend/RenderUploadManager.h>
#include <RenderSystem/IRenderSystem.h>
#include <queue>
Render::RenderPassGraph *Render::RenderPassGraph::GetInstance()
{

    static RenderPassGraph instance;
    return &instance;
}

Render::RenderPassGraph::RenderPassGraph() : mFenceValue(-1), mFrameCount(-1) {}

Render::RenderPassGraph::~RenderPassGraph() {}

void Render::RenderPassGraph::RegisterRenderPassCallback(
    const std::string &renderPassName, std::function<void(RenderPassGraphBuilder &)> setUpCallback,
    std::function<void(const RenderPassExecuteContext &)> executeCallback)
{
    // 개정전
    //   mRenderPassCallbackTable[renderPassName].first = setUpCallback;
    //   mRenderPassCallbackTable[renderPassName].second = executeCallback;

    // 개정
    mRenderPassCallbackList.push_back({setUpCallback, executeCallback});

    // 개정전
    //   mRenderPassNodeTable[renderPassName] = std::make_unique<RenderPassNode>();
    //   mRenderPassNodeTable[renderPassName]->mPassName = renderPassName;
    //   mRenderPassNodeTable[renderPassName]->executeCallback = executeCallback;

    // 개정
    //   mRenderPassNodeList.push_back(std::make_unique<RenderPassNode>());
    //  mRenderPassNodeList.back()->mPassName = renderPassName;
    mRenderPassNodeList.back()->executeCallback = executeCallback;

    // 테이블에 등록은 해놓아야한다
    // Write, Read에서  사용
    //   mRenderPassNodeTable[renderPassName] = mRenderPassNodeList.back().get();
}

void Render::RenderPassGraph::RegisterRenderPass(std::unique_ptr<Render::IRenderPass> &&renderPass,
                                                 const std::string &renderPassName,
                                                 const RenderPassSetUpData &passSetUpData)
{
    mRenderPassNodeList.push_back(std::make_unique<RenderPassNode>());

    RenderPassNode *passNode = mRenderPassNodeList.back().get();
    passNode->mPassName = renderPassName;
    passNode->mRenderPass = std::move(renderPass);
    mRenderPassNodeTable[renderPassName] = mRenderPassNodeList.back().get();

    passNode->mRenderPass->AddToGraph(*this, passSetUpData);
}

GRM::GRMPtr Render::RenderPassGraph::GetTexture(const std::string &texName) const
{
    std::unordered_map<std::string, std::pair<GRM::GRMPtr, RenderResourceDesc>>::const_iterator it =
        mTexResourceTable.find(texName);
    if (it == mTexResourceTable.cend())
        return nullptr;
    else
        return it->second.first;
}

void Render::RenderPassGraph::Compile()
{
    // 순회하면서  FrameGraphResourceNode 생성
    RenderPassGraphBuilder builder(this);
    // for (auto &renderPassCallback : mRenderPassCallbackTable)
    //{
    //     mCurrPassName = renderPassCallback.first;

    //    auto setUpCallback = renderPassCallback.second.first;
    //    setUpCallback(builder);
    //}

    for (auto &renderPassCallback : mRenderPassCallbackList)
    {
        auto &resourceSetUpCallback = renderPassCallback.first;
        resourceSetUpCallback(builder); // Write ,Read 호출됨 .0
    }

    AllocResourceAll();

    /* 간선그래프는 완성됬다.*/
    /*
        간접리스트를 구축하고 ,진입차수를 계산한다.
    */

    // PassNode의 의존성 리스트를 구축
    for (auto &resourceNode : mResourceNodeTable)
    {

        auto &resourceVersionList = resourceNode.second->mResourceVersionList;
        for (int i = 0; i < resourceVersionList.size(); ++i)
        {

            auto &currentResourceVersion = resourceVersionList[i];

            RenderPassNode *producer = currentResourceVersion->pProducer;
            const std::vector<RenderPassNode *> &consumers = currentResourceVersion->Consumers;

            // 소비자(read)들의 의존성
            for (auto consumerPassNode : consumers)
            {
                producer->mSuccessorRenderPassNodeList.insert(consumerPassNode);
                consumerPassNode->mPrepredecessorRenderPassNodeList.insert(producer);
            }

            // 그리고 그 다음 version의 생산자도 의존하는 소비자로서 넣어주어야한다,
            //
            // 다음버전이 있다면
            if (resourceVersionList.size() > (i + 1))
            {
                RenderPassNode *nextProducer = resourceVersionList[i + 1]->pProducer;

                if (consumers.size() == 0)
                {
                    // write - write 인경우
                    // 다음생산자는 이전버전의 생산자에 의존
                    producer->mSuccessorRenderPassNodeList.insert(nextProducer);
                    nextProducer->mPrepredecessorRenderPassNodeList.insert(producer);
                }
                else
                {
                    // write -read -write 인경우
                    // 다음 생산자는 이전버전의 소비자들에 의존하는거임

                    for (auto consumerPassNode : consumers)
                    {
                        consumerPassNode->mSuccessorRenderPassNodeList.insert(nextProducer);
                        nextProducer->mPrepredecessorRenderPassNodeList.insert(consumerPassNode);
                    }
                }
            }
        }
    }

    // 진입차수 초기화 & 노드큐 초기 셋팅
    /*
    1. 진입차수가 0인 노드들을 큐에 넣고 하나씩 꺼내면서 후임노드들의 진입차수를 1감소 시키고 후임노드들중
   진입차수가 0 이된 노드들을 큐에 다시 삽입
    2. 1번동작을 반복
*/
    // 맨처음 0 진입차수를 가지는 노드들을 큐에 삽입 .
    std::queue<RenderPassNode *> renderPassNodeQueue;
    /*   for (auto &renderPassNode : mRenderPassNodeTable)
       {
           renderPassNode.second->mInDegree = renderPassNode.second->mPrepredecessorRenderPassNodeList.size();
           if (renderPassNode.second->mInDegree == 0)
           {
               renderPassNodeQueue.push(renderPassNode.second.get());
           }
       }*/

    // 개정
    for (auto &renderPassNode : mRenderPassNodeList)
    {
        renderPassNode->mInDegree = renderPassNode->mPrepredecessorRenderPassNodeList.size();
        if (renderPassNode->mInDegree == 0)
        {
            renderPassNodeQueue.push(renderPassNode.get());
        }
    }

    while (!renderPassNodeQueue.empty())
    {

        RenderPassNode *renderPassNode = renderPassNodeQueue.front();
        renderPassNodeQueue.pop();
        mOrderedRenderPassNodeList.push_back(renderPassNode);

        // 이후노드들의 진입차수 1감소, 0이된 노드들 큐에 추가

        for (auto successor : renderPassNode->mSuccessorRenderPassNodeList)
        {
            successor->mInDegree--;
            if (successor->mInDegree == 0)
            {
                renderPassNodeQueue.push(successor);
            }
        }
    }

    // 순환구조 실수를 했을경우에 간단한 에러검사

    if (mOrderedRenderPassNodeList.size() != mRenderPassNodeTable.size())
    {
        LOG_MESSAGE_ERROR("RenderPassGraph", "순환 의존성이 감지되어 프레임 그래프 컴파일에 실패했습니다.");
        // 문제가 있는 프레임은 더 이상 렌더링을 진행하지 않도록 처리
        mOrderedRenderPassNodeList.clear();
    }

    // render pass들의 resource barrier리스트 구축
    //

    std::unordered_map<FrameGraphResourceNode *, EResourceState> resourceStateTable;

    // 리소스들의 초기상태 설정
    for (auto &resourceNode : mResourceNodeTable)
    {
        resourceStateTable[resourceNode.second.get()] = resourceNode.second->mInitState;
    }

    // 정렬된 pass들을 순회하면서 각 사용하는 리소스들에대해 barrier를 구성한다.
    for (auto &renderPassNode : mOrderedRenderPassNodeList)
    {

        // input resource
        for (auto &resourceUsage : renderPassNode->mInputLists)
        {
            FrameGraphResourceNode *originalFrameGraphResourceNode = resourceUsage.mVersion->mOriginalResourceNode;
            EResourceState beforeState = resourceStateTable[originalFrameGraphResourceNode];
            EResourceState afterState = resourceUsage.mRequiredState;

            if (beforeState != afterState)
            {
                // barrier 구축
                GRM::IGpuResource *gpuResource =
                    mTexResourceTable[originalFrameGraphResourceNode->Name].first.getResource();

                // 해당 PassNode가 수행되기전 수행할 배리어를 등록한다.
                renderPassNode->PreBarrier.push_back({gpuResource, beforeState, afterState});

                // 계속 배리어 구축을 위해 상태 업데이트
                resourceStateTable[originalFrameGraphResourceNode] = afterState;
            }
            mResourceNodeTable[originalFrameGraphResourceNode->Name]->mLastState = afterState;
        }

        // output resource
        for (auto &resourceUsage : renderPassNode->mOutputLists)
        {
            EResourceState beforeState = resourceStateTable[resourceUsage.mVersion->mOriginalResourceNode];
            EResourceState afterState = resourceUsage.mRequiredState;

            if (beforeState == EResourceState::ePresent)
            {
                int a = 2;
            }

            if (beforeState != afterState)
            {
                // barrier 구축
                GRM::IGpuResource *gpuResource =
                    mTexResourceTable[resourceUsage.mVersion->mOriginalResourceNode->Name].first.getResource();

                renderPassNode->PreBarrier.push_back({gpuResource, beforeState, afterState});

                resourceStateTable[resourceUsage.mVersion->mOriginalResourceNode] = afterState;
            }
            mResourceNodeTable[resourceUsage.mVersion->mOriginalResourceNode->Name]->mLastState = afterState;
        }
    }

    int a2 = 2;
}

void Render::RenderPassGraph::Execute(const RenderPassExecuteContext &renderPassExecuteContext)
{

    // MaterialUpload
    UploadMaterialData(renderPassExecuteContext);
    UploadLightData(renderPassExecuteContext);

    Render::RECT ScreenViewportRect;
    /*ScreenViewportRect.mLeft = renderPassExecuteContext.mGlobalFrameData.mSceneViewport.TopLeftX;
    ScreenViewportRect.mRight =
        ScreenViewportRect.mLeft + renderPassExecuteContext.mGlobalFrameData.mSceneViewport.Width;
    ScreenViewportRect.mTop = renderPassExecuteContext.mGlobalFrameData.mSceneViewport.TopLeftY;
    ScreenViewportRect.mBottom =
        ScreenViewportRect.mTop + renderPassExecuteContext.mGlobalFrameData.mSceneViewport.Height;*/

    ScreenViewportRect.mLeft = renderPassExecuteContext.mGlobalSceneViewport.TopLeftX;
    ScreenViewportRect.mRight = ScreenViewportRect.mLeft + renderPassExecuteContext.mGlobalSceneViewport.Width;
    ScreenViewportRect.mTop = renderPassExecuteContext.mGlobalSceneViewport.TopLeftY;
    ScreenViewportRect.mBottom = ScreenViewportRect.mTop + renderPassExecuteContext.mGlobalSceneViewport.Height;

    Render::RECT viewportRect = ScreenViewportRect;
    Render::IRenderSystem *renderSystem = Render::IRenderSystem::GetInstance();
    for (auto renderPassNode : mOrderedRenderPassNodeList)
    {
        // 해당 pass 에대한 resource barrier 를 수행요청
        // rendersystem에게? 아니면 gpuResourceManager에게

        if (renderPassNode->mRenderPass->GetUseDefaultViewport() == false)
        {
            viewportRect = renderPassNode->mRenderPass->GetViewport();
        }
        else
        {
            viewportRect = ScreenViewportRect;
        }
        // 렌더시스템에게 요청
        // 렌더시스템은 해당 Barrier를 수행
        // gpuResourceManager에는 상태변경정보만 알림 (요청x)
        renderSystem->ExecuteResourceBarriers(renderPassExecuteContext.mCommandContext, renderPassNode->PreBarrier);

        // TODO render target set
        GRM::IGpuResource *renderTarget = nullptr;

        // render target clear

        if (renderPassNode->mRenderTargetInfo.mFrameGraphResourceVersion)
        {
            renderTarget = mTexResourceTable[renderPassNode->mRenderTargetInfo.mFrameGraphResourceVersion
                                                 ->mOriginalResourceNode->Name]
                               .first.getResource();
        }

        if (renderTarget && renderPassNode->mRenderTargetInfo.bClear)
        {
            renderSystem->ClearRenderTarget(renderPassExecuteContext.mCommandContext, renderTarget,
                                            renderPassNode->mRenderTargetInfo.mClearColor, viewportRect);
        }

        // depthstencil clear
        GRM::IGpuResource *depthStencil = nullptr;

        if (renderPassNode->mDepthStencilInfo.mFrameGraphResourceVersion)
        {
            depthStencil = mTexResourceTable[renderPassNode->mDepthStencilInfo.mFrameGraphResourceVersion
                                                 ->mOriginalResourceNode->Name]
                               .first.getResource();
        }

        if (depthStencil && renderPassNode->mDepthStencilInfo.bClear)
        {
            renderSystem->ClearDepthStencil(renderPassExecuteContext.mCommandContext, depthStencil,
                                            renderPassNode->mDepthStencilInfo.mClearValue, viewportRect);
        }

        // set

        // rendertarget을 설정한다.
        renderSystem->SetRenderTarget(renderPassExecuteContext.mCommandContext, renderTarget, depthStencil);

        renderPassNode->executeCallback(renderPassExecuteContext);

#ifdef _DEBUG
        auto *d3dDevice = static_cast<Core::D3DCoreDevice *>(Core::CoreDevice::GetInstance())->mDevice.Get();
        HRESULT reason = d3dDevice->GetDeviceRemovedReason();
        if (FAILED(reason))
        {
            CHECK(false, "Device removed after pass");
        }
#endif
    }

    GRM::IGpuResourceManager *gpuResourceManager = GRM::IGpuResourceManager::GetInstance();
    for (auto &resourceNode : mResourceNodeTable)
    {

        gpuResourceManager->SetResourceState(mTexResourceTable[resourceNode.first].first.getResource(),
                                             resourceNode.second->mLastState);
    }
}

void Render::RenderPassGraph::Reset()
{
    mRenderPassCallbackTable.clear();
    mRenderPassCallbackList.clear();
    mResourceNodeTable.clear();
    mRenderPassNodeTable.clear();
    mOrderedRenderPassNodeList.clear();
    mRenderPassNodeList.clear();
    mTexResourceTable.clear();

    mCurrPassName = "";

    for (int i = 0; i < mAllocatedResources.size(); ++i)
    {
        if (mAllocatedResources[i]->mResource.getResource()->GetCurrentResourceState() == EResourceState::ePresent)
        {
            int a = 2;
        }

        RenderPipelineManager::GetInstance()->mRenderResourcePool.Free(mAllocatedResources[i]);
    }

    mAllocatedResources.clear();
}

void Render::RenderPassGraph::Import(const std::string &texName, GRM::GRMPtr ptr, EResourceState initState)
{
    mResourceNodeTable[texName] = std::make_unique<FrameGraphResourceNode>();
    mResourceNodeTable[texName]->Name = texName;
    mResourceNodeTable[texName]->mInitState = initState;

    mTexResourceTable[texName].first = ptr;
}

void Render::RenderPassGraph::SetFrameCount(uint64_t count)
{

    mFrameCount = count;
}

void Render::RenderPassGraph::SetFenceValue(uint64_t value)
{

    mFenceValue = value;
}

void Render::RenderPassGraph::Write(const std::string &texName, const GRM::TextureDesc &texDesc)
{
    // if (mResourceNodeTable[texName] == nullptr)
    //{
    //     mResourceNodeTable[texName] = std::make_unique<FrameGraphResourceNode>();
    //     mResourceNodeTable[texName]->Name = texName;
    //     mResourceNodeTable[texName]->pProducer = mRenderPassNodeTable[mCurrPassName].get();
    // }

    // GRM::IGpuResourceManager *gpuResourceManager = GRM::IGpuResourceManager::GetInstance();

    //// 다른 패스의 Read가 먼저호출되서 resourceNode 빈테이블 요소만 존재할수도있다.

    //// resourceTable은 존재하지않을거다 ,write 호출전에는
    // std::unordered_map<std::string, std::pair<GRM::GRMPtr, GRM::TextureDesc>>::iterator it =
    //     mTexResourceTable.find(texName);
    // if (it == mTexResourceTable.end())
    //{

    //    // 생성요청 gpuResourceManager에게 ;
    //    // mTexResourceTable[texName].first = nullptr;

    //    // 생성할때 depth용이냐, renderTarget용이냐 구분할필요가있다.
    //    // 그리고 특히 write할떄 이게 그 구분이 중요하다.
    //    mTexResourceTable[texName].first = gpuResourceManager->CreateTexture(texDesc);
    //    mTexResourceTable[texName].second = texDesc;
    //}
    // else
    //{
    //    // 존재하지만 속성이 다른경우
    //    // 뭐 텍스처니깐 일반적으로 크기 , format정도? 다르면 기존것 제거이후 재생성

    //    if (mTexResourceTable[texName].second.mTextureUsage != texDesc.mTextureUsage ||
    //        mTexResourceTable[texName].second.mScratchImage.mMetadata != texDesc.mScratchImage.mMetadata)
    //    {

    //        mTexResourceTable[texName].first = gpuResourceManager->CreateTexture(texDesc);
    //        mTexResourceTable[texName].second = texDesc;
    //    }
    //}

    //// 위상정렬
    //// node- rendePass, read, write tex 나열
    ////  write하는 node들이 앞순서
}
void Render::RenderPassGraph::Create(const std::string &texName, const RenderResourceDesc &Desc,
                                     EResourceState initState)
{
    auto it = mResourceNodeTable.find(texName);

    if (it != mResourceNodeTable.end())
    {

        // TODO Debug시 Desc도 비교 하는코드를 넣어보자
        //
        //

        return;
    }

    // 지연 생성 모든패스들의 설정이 완료된후 생성
    //  PooledRenderResource *resource = RenderPipelineManager::GetInstance()->mRenderResourcePool.Alloc(Desc);

    mResourceNodeTable[texName] = std::make_unique<FrameGraphResourceNode>();
    mResourceNodeTable[texName]->Name = texName;
    mResourceNodeTable[texName]->mInitState = initState;

    mTexResourceTable[texName].first = nullptr;
    mTexResourceTable[texName].second = Desc;
    // resource->mResource;

    // mAllocatedResources.push_back(resource);

    return;
}
void Render::RenderPassGraph::SetRenderTarget(const std::string &texName, const std::string &passName, bool bClear,
                                              const float *color)
{
    std::unique_ptr<FrameGraphResourceVersion> ResourceNewVersion = std::make_unique<FrameGraphResourceVersion>();
    ResourceNewVersion->pProducer = mRenderPassNodeTable[passName];
    ResourceNewVersion->mState = EResourceState::eRenderTarget;
    ResourceNewVersion->mOriginalResourceNode = mResourceNodeTable[texName].get();

    mRenderPassNodeTable[passName]->mOutputLists.push_back({ResourceNewVersion.get(), EResourceState::eRenderTarget});
    // render target info set
    mRenderPassNodeTable[passName]->mRenderTargetInfo.mFrameGraphResourceVersion = ResourceNewVersion.get();
    mRenderPassNodeTable[passName]->mRenderTargetInfo.bClear = bClear;
    if (bClear)
    {
        for (int i = 0; i < 4; ++i)
            mRenderPassNodeTable[passName]->mRenderTargetInfo.mClearColor[i] = color[i];
    }

    mResourceNodeTable[texName]->mResourceVersionList.push_back(std::move(ResourceNewVersion));
}
void Render::RenderPassGraph::SetDepthStencil(const std::string &texName, const std::string &passName, bool bClear,
                                              float clearValue, bool bDepthWrite)
{
    std::unique_ptr<FrameGraphResourceVersion> ResourceNewVersion = std::make_unique<FrameGraphResourceVersion>();
    ResourceNewVersion->pProducer = mRenderPassNodeTable[passName];

    EResourceState resourceState = EResourceState::eWriteDepthStencil;
    if (bDepthWrite)
        resourceState = EResourceState::eWriteDepthStencil;

    ResourceNewVersion->mState = resourceState;
    ResourceNewVersion->mOriginalResourceNode = mResourceNodeTable[texName].get();

    mRenderPassNodeTable[passName]->mOutputLists.push_back({ResourceNewVersion.get(), ResourceNewVersion->mState});

    mRenderPassNodeTable[passName]->mDepthStencilInfo.mFrameGraphResourceVersion = ResourceNewVersion.get();
    mRenderPassNodeTable[passName]->mDepthStencilInfo.bClear = bClear;
    if (bClear)
    {
        mRenderPassNodeTable[passName]->mDepthStencilInfo.mClearValue = clearValue;
    }
    mResourceNodeTable[texName]->mResourceVersionList.push_back(std::move(ResourceNewVersion));
}
void Render::RenderPassGraph::Write(const std::string &texName, const std::string &passName,
                                    EResourceState resourceState)
{
    // 동일한 리소스에 write할때마다 버전을 높여서 기록해야함
    // 그래야 그래프가 올바르게 구축됨

    std::unique_ptr<FrameGraphResourceVersion> ResourceNewVersion = std::make_unique<FrameGraphResourceVersion>();
    ResourceNewVersion->pProducer = mRenderPassNodeTable[passName];
    ResourceNewVersion->mState = resourceState;
    ResourceNewVersion->mOriginalResourceNode = mResourceNodeTable[texName].get();

    mRenderPassNodeTable[passName]->mOutputLists.push_back({ResourceNewVersion.get(), resourceState});
    mResourceNodeTable[texName]->mResourceVersionList.push_back(std::move(ResourceNewVersion));
};

void Render::RenderPassGraph::Read(const std::string &texName, const std::string &passName,
                                   EResourceState resourceState)
{
    if (mResourceNodeTable[texName] == nullptr)
    {
        CHECK(mResourceNodeTable[texName] == nullptr, "%s ResourceNode is not exist", texName.c_str());

        // OR Creat
    }

    mTexResourceTable[texName].second.mUsage = GRM::ETextureUsage::eRenderTargetShaderResource;

    FrameGraphResourceVersion *resourceVersion = mResourceNodeTable[texName]->mResourceVersionList.back().get();
    resourceVersion->Consumers.push_back(mRenderPassNodeTable[passName]);

    mRenderPassNodeTable[passName]->mInputLists.push_back({resourceVersion, resourceState});
}

void Render::RenderPassGraph::AllocResourceAll()
{
    for (auto &tex : mTexResourceTable)
    {
        const std::string &texName = tex.first;
        EResourceState initState = mResourceNodeTable[texName]->mInitState;

        std::pair<GRM::GRMPtr, Render::RenderResourceDesc> &resource = tex.second;

        if (resource.first.getResource() == nullptr)
        {
            PooledRenderResource *renderResource =
                RenderPipelineManager::GetInstance()->mRenderResourcePool.Alloc(resource.second);

            renderResource->mName = texName;
            mAllocatedResources.push_back(renderResource);
            renderResource->mLastUsedFence = mFenceValue;
            renderResource->mLastUsedFrame = mFrameCount;

            mResourceNodeTable[texName]->mInitState =
                renderResource->mResource.getResource()->GetCurrentResourceState();

            if (mResourceNodeTable[texName]->mInitState == EResourceState::ePresent)
            {
                int a2 = 2;
            }

            mTexResourceTable[texName].first = renderResource->mResource;
        }
    }
}

void Render::RenderPassGraph::UploadMaterialData(const RenderPassExecuteContext &renderPassExecuteContext)
{
    auto gpuResourceManager = GRM::IGpuResourceManager::GetInstance();
    auto bufferContextSystem = GRM::GpuBufferContextSystem::GetInstance();
    auto uploadManager = Render::RenderUploadManager::GetInstance();
    GRM::GpuConstantBufferContext *bufferContext =
        static_cast<GRM::GpuConstantBufferContext *>(bufferContextSystem->GetGpuBufferContext(3));
    for (const auto &e : renderPassExecuteContext.mMaterialRenderSnapshotTable)
    {

        uint32_t materialHandle = e.first;

        const MaterialRenderSnapshot &materialRenderSnapshot = e.second;

        // 상대적오프셋 0 1,2,3 으로부터 절대적 오프셋(프레임을계산한)  오프셋을 얻고 * elementsize 해서
        // bufferOffsetSize를 얻자

        size_t bufferOffset = 0;

        bufferOffset = bufferContext->mAllocateRange.GetCurrentFrameIndex(materialHandle);
        size_t bufferSizeOffset = bufferOffset * bufferContext->mBufferDesc.mElementDataSize;

        DefaultMaterialData data;
        uploadManager->UploadDefaultMaterialData(materialRenderSnapshot, data);

        gpuResourceManager->UploadBufferData(bufferContext->mGpuBuffer, &data, sizeof(data), 1, bufferSizeOffset);
    }
}

void Render::RenderPassGraph::UploadLightData(const RenderPassExecuteContext &renderPassExecuteContext)
{

    if (renderPassExecuteContext.mLightRenderCommandList.size() == 0)
        return;

    auto gpuResourceManager = GRM::IGpuResourceManager::GetInstance();
    auto bufferContextSystem = GRM::GpuBufferContextSystem::GetInstance();
    auto uploadManager = Render::RenderUploadManager::GetInstance();
    GRM::GpuStructuredBufferContext *bufferContext = static_cast<GRM::GpuStructuredBufferContext *>(
        bufferContextSystem->GetGpuBufferContext(AssetResolver::GetInstance()->GetLightStructuredGpuBufferID()));
    GRM::GRMPtr buffer = bufferContext->mGpuBuffersPerFrame[bufferContext->mCurrFrameIndex];

    std::vector<DefaultLightData> dataList(renderPassExecuteContext.mLightRenderCommandList.size());

    for (size_t i = 0; i < dataList.size(); ++i)
    {
        const Render::LightRenderCommand &cmd = renderPassExecuteContext.mLightRenderCommandList[i];
        uploadManager->UploadDefaultLightData(cmd, dataList[i]);
    }

    gpuResourceManager->UploadBufferData(buffer, dataList.data(), sizeof(DefaultLightData), dataList.size(), 0);
}

Render::RenderPassGraphBuilder::RenderPassGraphBuilder(RenderPassGraph *renderPassGraph)
    : mRenderPassGraph(renderPassGraph)
{
}

Render::RenderPassGraphBuilder::~RenderPassGraphBuilder() {}

void Render::RenderPassGraphBuilder::Create(const std::string &texName, const RenderResourceDesc &Desc,
                                            EResourceState initState)
{
    mRenderPassGraph->Create(texName, Desc, initState);
}
void Render::RenderPassGraphBuilder::SetRenderTarget(const std::string &texName, const std::string &passName,
                                                     bool bClear, const float *color)
{
    mRenderPassGraph->SetRenderTarget(texName, passName, bClear, color);
}

void Render::RenderPassGraphBuilder::SetDepthStencil(const std::string &texName, const std::string &passName,
                                                     bool bClear, float clearValue, bool bDepthWrite)
{
    mRenderPassGraph->SetDepthStencil(texName, passName, bClear, clearValue, bDepthWrite);
}

void Render::RenderPassGraphBuilder::Write(const std::string &texName, const std::string &passName,
                                           EResourceState state)
{
    mRenderPassGraph->Write(texName, passName, state);
}

void Render::RenderPassGraphBuilder::Read(const std::string &texName, const std::string &passName,
                                          EResourceState resourceState)
{
    mRenderPassGraph->Read(texName, passName, resourceState);
}

// GRM::TextureDesc texDesc;
// texDesc.mTextureUsage = GRM::ETextureUsage::eRenderTargetShaderResource;
// texDesc.mScratchImage.mMetadata.mWidth = 2048;
// texDesc.mScratchImage.mMetadata.mHeight = 2048;
// texDesc.mScratchImage.mMetadata.mDimension = GRM::ETextureType::eTexture2D;
// texDesc.mScratchImage.mMetadata.mFormat = GRM::ETextureFormat::eR16G16B16A16_UNORM;
// texDesc.mScratchImage.mMetadata.mDepth = 1;
// texDesc.mScratchImage.mMetadata.mArraySize = 1;
// texDesc.mScratchImage.mMetadata.mMipLevels = 1;

// texDesc.mScratchImage.mMetadata.mMiscFlags;