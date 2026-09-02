#include "RenderFrontend/UIRenderItemBuilder.h"
#include "D3DGpuResourceManager/GpuSamplerSystem.h"
#include <Core/ViewportController.h>
#include <CoreAsset/Material.h>
#include <CoreBase/CoreAssert.h>
#include <D3DGpuResourceManager/IGpuResourceManager.h>
#include <Logger/Logger.h>
#include <RenderFrontend/AssetResolver.h>
#include <RenderFrontend/RenderPassGraph.h>
#include <RenderFrontend/RenderPipelineManager.h>
#include <RenderSystem/IMaterialManager.h>
#include <RenderSystem/RenderType.h>
#include <UISystem/UICanvas.h>
#include <UISystem/UIElement.h>
#include <UISystem/UIManager.h>
#include <UISystem/UIRectTransform.h>
#include <UISystem/UIRenderableComponent.h>
#include <UISystem/UIResizeGizmoRenderableComponent.h>
#include <algorithm>
#include <assert.h>
#include <glm/glm.hpp>
#include <queue>
#include <stack>
#include <vector>

Render::UIRenderItemBuilder *Render::UIRenderItemBuilder::mInstance = nullptr;

Render::UIRenderItemBuilder *Render::UIRenderItemBuilder::GetInstance()
{

    CHECK(mInstance != nullptr, "UI렌더아이템빌더 인스턴스가 생성되지않았습니다");

    return mInstance;
}

Render::UIRenderItemBuilder::UIRenderItemBuilder(Render::IRenderSystem *renderSystem, UI::UIManager *uiManager,
                                                 GRM::IGpuResourceManager *gpuResourceManager,
                                                 Render::AssetResolver *assetResolver)
    : mRenderSystem(renderSystem), mUiManager(uiManager), mGpuResourceManager(gpuResourceManager),
      mAssetResolver(assetResolver), mCurrVertexBufferOffset(0), mCurrIndexBufferOffset(0)
{
    CHECK(mInstance == nullptr, "UI렌더아이템빌더 생성자 두번호출하였습니다. 한번만 호출해야합니다");

    // TODO 프레임별 버퍼필요 3개
    mVertexBufferGpuPerFrame.resize(3);
    mIndexBufferGpuPerFrame.resize(3);

    for (int i = 0; i < 3; ++i)
    {
        GRM::BufferDesc vertexBufferDesc;

        vertexBufferDesc.mBufferMemoryAccess = GRM::EBufferMemoryAccess::eCpuWriteOnly;
        vertexBufferDesc.mBufferUsage = GRM::EBufferUsage::eVertexBuffer;
        vertexBufferDesc.mElementDataNum = 4 * 10000;
        vertexBufferDesc.mElementDataSize = sizeof(UI::UIVertex);
        vertexBufferDesc.mBufferSize = 4 * 10000 * vertexBufferDesc.mElementDataSize;
        vertexBufferDesc.mData = nullptr;
        mVertexBufferGpuPerFrame[i] = mGpuResourceManager->CreateBuffer(vertexBufferDesc);

        GRM::BufferDesc indexBufferDesc;

        indexBufferDesc.mBufferMemoryAccess = GRM::EBufferMemoryAccess::eCpuWriteOnly;
        indexBufferDesc.mBufferUsage = GRM::EBufferUsage::eIndexBuffer;
        indexBufferDesc.mElementDataNum = 6 * 10000;
        indexBufferDesc.mElementDataSize = sizeof(uint32_t);
        indexBufferDesc.mBufferSize = 6 * 10000 * indexBufferDesc.mElementDataSize;
        indexBufferDesc.mData = nullptr;
        mIndexBufferGpuPerFrame[i] = mGpuResourceManager->CreateBuffer(indexBufferDesc);
    }
    mInstance = this;

    SetRenderProxyManager(this);
}

Render::UIRenderItemBuilder::~UIRenderItemBuilder() {}

void Render::UIRenderItemBuilder::EndFrame()
{

    //  mUIElementContextVectorPerRenderChannel.clear();
}

void Render::UIRenderItemBuilder::RegisterRenderProxy(UI::UIRenderProxy *renderProxy)
{

    if (renderProxy == nullptr)
        return;

    mUIRenderProxyListTable[renderProxy->mCanvas->GetID()].push_back(renderProxy);
}
void Render::UIRenderItemBuilder::UnRegisterRenderProxy(UI::UIRenderProxy *renderProxy)
{

    if (renderProxy == nullptr)
        return;

    UI::UICanvasID canvasID = renderProxy->mCanvas->GetID();
    auto it =
        std::find(mUIRenderProxyListTable[canvasID].begin(), mUIRenderProxyListTable[canvasID].end(), renderProxy);

    if (it != mUIRenderProxyListTable[canvasID].end())
    {
        if (mUIRenderProxyListTable[canvasID].size() == 1)
        {
            mUIRenderProxyListTable[canvasID].erase(it);
        }
        else
        {
            int currIndex = it - mUIRenderProxyListTable[canvasID].begin();
            int endIndex = mUIRenderProxyListTable[canvasID].size() - 1;
            std::swap(mUIRenderProxyListTable[canvasID][currIndex], mUIRenderProxyListTable[canvasID][endIndex]);

            mUIRenderProxyListTable[canvasID].pop_back();
        }
    }
}

void Render::UIRenderItemBuilder::UploadStart(GRM::GRMPtr &oVertexBuffer, GRM::GRMPtr &oIndexBuffer)
{
    oVertexBuffer = mVertexBufferGpuPerFrame[mCurrFrame];
    oIndexBuffer = mIndexBufferGpuPerFrame[mCurrFrame];
    // reset
}

void Render::UIRenderItemBuilder::UploadVertexBuffer(const std::vector<UIRenderCommand> &renderCommandList,
                                                     uint32_t &oVertexOffset, uint32_t &oIndexOffset,
                                                     uint32_t &oVertexNum, uint32_t &oIndexNum,
                                                     const std::vector<UI::UIVertex> &vertexBuffer,
                                                     const std::vector<uint32_t> &indexBuffer)
{

    std::vector<UI::UIVertex> totalVertexVec;
    std::vector<uint32_t> totalIndexVec;

    for (const auto &uiRenderCommand : renderCommandList)
    {
        size_t vertexNum = uiRenderCommand.mVertexNum;

        // base offset before insertion
        size_t vertexBaseOffset = totalVertexVec.size();

        uint32_t vertexBufferStartOffset = uiRenderCommand.mVertexStartOffset;
        // insert new vertices (no prior resize)
        totalVertexVec.insert(totalVertexVec.end(), vertexBuffer.begin() + vertexBufferStartOffset,
                              vertexBuffer.begin() + vertexBufferStartOffset + vertexNum);

        size_t indexNum = uiRenderCommand.mIndexNum;
        // base index offset before insertion
        size_t indexBaseOffset = totalIndexVec.size();

        uint32_t indexBufferStartOffset = uiRenderCommand.mIndexStartOffset;
        // insert new indices
        totalIndexVec.insert(totalIndexVec.end(), indexBuffer.begin() + indexBufferStartOffset,
                             indexBuffer.begin() + indexBufferStartOffset + indexNum);

        // adjust only the newly inserted indices
        for (size_t i = indexBaseOffset; i < indexBaseOffset + indexNum; ++i)
        {
            // add vertexBaseOffset to each index
            totalIndexVec[i] = static_cast<uint32_t>(totalIndexVec[i] + vertexBaseOffset);
        }
    }

    // bufferOffset은 바이트 단위로 전달해야 한다.
    size_t vertexByteOffset = static_cast<size_t>(mCurrVertexBufferOffset) * sizeof(UI::UIVertex);
    bool ret = mGpuResourceManager->UploadBufferData(mVertexBufferGpuPerFrame[mCurrFrame], totalVertexVec.data(),
                                                     sizeof(UI::UIVertex), totalVertexVec.size(), vertexByteOffset);
    if (!ret)
    {
        LOG_MESSAGE_ERROR("UIRenderItemBuilder", "Vertex buffer upload failed");
    }

    size_t indexByteOffset = static_cast<size_t>(mCurrIndexBufferOffset) * sizeof(uint32_t);
    ret = mGpuResourceManager->UploadBufferData(mIndexBufferGpuPerFrame[mCurrFrame], totalIndexVec.data(),
                                                sizeof(uint32_t), totalIndexVec.size(), indexByteOffset);
    if (!ret)
    {
        LOG_MESSAGE_ERROR("UIRenderItemBuilder", "Index buffer upload failed");
    }

    // 반환값은 요소(버텍스/인덱스) 오프셋으로 유지
    oVertexOffset = mCurrVertexBufferOffset;
    oIndexOffset = mCurrIndexBufferOffset;
    oVertexNum = static_cast<uint32_t>(totalVertexVec.size());
    oIndexNum = static_cast<uint32_t>(totalIndexVec.size());

    mCurrVertexBufferOffset += static_cast<uint32_t>(totalVertexVec.size());
    mCurrIndexBufferOffset += static_cast<uint32_t>(totalIndexVec.size());
}
void Render::UIRenderItemBuilder::UploadIndexBuffer(const std::vector<UIRenderCommand> &renderProxyList) {}
void Render::UIRenderItemBuilder::UploadEnd(GRM::GRMPtr &vertexPtr, GRM::GRMPtr &indexPtr) {}

std::vector<UI::UIRenderProxy *> Render::UIRenderItemBuilder::GetRenderProxyList(UI::UICanvasID canvasID) const
{
    auto it = mUIRenderProxyListTable.find(canvasID);

    if (it == mUIRenderProxyListTable.cend())
    {
        return {};
    }

    return it->second;
}

void Render::UIRenderItemBuilder::ResetBufferOffset(int nextFrame)
{

    mCurrVertexBufferOffset = 0;
    mCurrIndexBufferOffset = 0;
    mCurrFrame = nextFrame;
}