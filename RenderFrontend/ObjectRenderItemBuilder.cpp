#include "ObjectRenderItemBuilder.h"
#include <Core/Map.h>
#include <Core/World.h>
#include <CoreAsset/AssetManager.h>
#include <CoreAsset/Material.h>
#include <CoreAsset/StaticMesh.h>
#include <D3DGpuResourceManager/GpuBufferContextSystem.h>
#include <D3DGpuResourceManager/GpuSamplerSystem.h>
#include <D3DGpuResourceManager/IGpuResourceManager.h>
#include <Logger/Logger.h>
#include <RenderFrontend/AssetResolver.h>
#include <RenderSystem/IMaterialManager.h>
#include <RenderSystem/IRenderSystem.h>
#include <RenderSystem/RenderType.h>
#include <algorithm>
#include <assert.h>
#include <string>
#include <vector>

Render::ObjectRenderItemBuilder *Render::ObjectRenderItemBuilder::mInstance = nullptr;

Render::ObjectRenderItemBuilder *Render::ObjectRenderItemBuilder::GetInstance()
{
    if (mInstance == nullptr)
    {
        LOG_MESSAGE_ERROR("ReenderFrontend", "ObjectRenderItemBuilder Instance not exsitence");
        assert(0);
    }

    return mInstance;
}

Render::ObjectRenderItemBuilder::ObjectRenderItemBuilder(Render::IRenderSystem *renderSystem,
                                                         GRM::IGpuResourceManager *gpuResourceManager,
                                                         AssetResolver *assetResolver)
    : mRenderSystem(renderSystem), mGpuResourceManager(gpuResourceManager), mAssetResolver(assetResolver)
{
    if (mInstance != nullptr)
    {
        LOG_MESSAGE_ERROR("RenderFrontend", "only one ObjectRenderItemBuilder Instance can exsit");
        assert(0);
    }

    mInstance = this;
}

Render::ObjectRenderItemBuilder::~ObjectRenderItemBuilder()
{

    DestroyRenderProxyList();
    mInstance = nullptr;
}

void Render::ObjectRenderItemBuilder::Update() {}

void Render::ObjectRenderItemBuilder::EndFrame()
{

    ClearTempProxyList();
    ClearDebugLineCommandList();
}

void Render::ObjectRenderItemBuilder::RegisterRenderContext(uint32_t renderID)
{
    if (mRenderProxyListTable.find(renderID) == mRenderProxyListTable.end())
    {
        mRenderProxyListTable[renderID] = std::make_unique<RenderProxyContext>();
    }
}

void Render::ObjectRenderItemBuilder::RegisterRenderProxy(Core::RenderProxy *renderProxy)
{
    if (renderProxy == nullptr)
        return;

    RegisterRenderContext(renderProxy->mRenderID);

    mRenderProxyListTable[renderProxy->mRenderID]->mRenderProxyList.push_back(renderProxy);

    // mRenderProxyList.push_back(renderProxy);
}

void Render::ObjectRenderItemBuilder::UnRegisterRenderProxy(Core::RenderProxy *renderProxy)
{
    if (renderProxy == nullptr)
        return;

    {
        std::vector<Core::RenderProxy *> &renderProxyList =
            mRenderProxyListTable[renderProxy->mRenderID]->mRenderProxyList;
        std::vector<Core::RenderProxy *>::iterator it =
            std::find(renderProxyList.begin(), renderProxyList.end(), renderProxy);

        if (it != renderProxyList.end())
        {
            std::iter_swap(renderProxyList.end() - 1, it);
            renderProxyList.pop_back();
        }
    }

    //{
    //    std::vector<Core::RenderProxy *>::iterator it =
    //        std::find(mRenderProxyList.begin(), mRenderProxyList.end(), renderProxy);

    //    if (it != mRenderProxyList.end())
    //    {
    //        std::swap(mRenderProxyList.back(), *it);
    //        mRenderProxyList.pop_back();
    //    }
    //}

    if (renderProxy->mDrawOutline)
    {
        SetProxyDrawOutline(renderProxy, false);
    }
}

void Render::ObjectRenderItemBuilder::RegisterLightProxy(Core::LightProxy *lightProxy)
{

    if (lightProxy == nullptr)
        return;
    RegisterRenderContext(lightProxy->mRenderID);

    mRenderProxyListTable[lightProxy->mRenderID]->mLightProxyList.push_back(lightProxy);
}

void Render::ObjectRenderItemBuilder::UnRegisterLightProxy(Core::LightProxy *lightProxy)
{

    if (lightProxy == nullptr)
        return;

    std::vector<Core::LightProxy *> &lightProxyList = mRenderProxyListTable[lightProxy->mRenderID]->mLightProxyList;

    auto it = std::find(lightProxyList.begin(), lightProxyList.end(), lightProxy);
    if (it != lightProxyList.end())
    {
        std::iter_swap(it, lightProxyList.end() - 1);
        lightProxyList.pop_back();
    }
}

void Render::ObjectRenderItemBuilder::RegisterBillboardProxy(Core::BillboardRenderProxy *billboardProxy)
{

    if (billboardProxy == nullptr)
        return;

    RegisterRenderContext(billboardProxy->mRenderID);

    mRenderProxyListTable[billboardProxy->mRenderID]->mBillboardProxyList.push_back(billboardProxy);
}

void Render::ObjectRenderItemBuilder::UnRegisterBillboardProxy(Core::BillboardRenderProxy *billboardProxy)
{

    if (billboardProxy == nullptr)
        return;

    std::vector<Core::BillboardRenderProxy *> &proxyList =
        mRenderProxyListTable[billboardProxy->mRenderID]->mBillboardProxyList;

    auto it = std::find(proxyList.begin(), proxyList.end(), billboardProxy);
    if (it != proxyList.end())
    {
        std::iter_swap(it, proxyList.end() - 1);
        proxyList.pop_back();
    }
}

void Render::ObjectRenderItemBuilder::SetProxyDrawOutline(Core::RenderProxy *renderProxy, bool bDraw)
{

    if (renderProxy == nullptr)
        return;
    RegisterRenderContext(renderProxy->mRenderID);

    auto &outlineRenderProxyList = mRenderProxyListTable[renderProxy->mRenderID]->mDrawOutlineProxyList;

    if (bDraw)
    {
        if (std::find(outlineRenderProxyList.begin(), outlineRenderProxyList.end(), renderProxy) ==
            outlineRenderProxyList.end())
        {
            outlineRenderProxyList.push_back(renderProxy);
        }
    }
    else
    {
        auto it = std::find(outlineRenderProxyList.begin(), outlineRenderProxyList.end(), renderProxy);
        if (it != outlineRenderProxyList.end())
        {
            std::iter_swap(it, outlineRenderProxyList.end() - 1);
            outlineRenderProxyList.pop_back();
        }
    }
}

const std::vector<Core::RenderProxy *> &Render::ObjectRenderItemBuilder::GetRenderProxyList(uint32_t renderID) const
{

    auto it = mRenderProxyListTable.find(renderID);

    if (it != mRenderProxyListTable.cend())
        return it->second->mRenderProxyList;

    return mEmptyRenderProxyList;
    // return mRenderProxyListTable[map.GetID()];
}
Render::RenderProxyContext *Render::ObjectRenderItemBuilder::GetRenderProxyContext(uint32_t renderID) const
{

    auto it = mRenderProxyListTable.find(renderID);

    if (it != mRenderProxyListTable.cend())
        return it->second.get();

    return nullptr;
    // TODO: 여기에 return 문을 삽입합니다.
}
const std::vector<Core::RenderProxy *> &Render::ObjectRenderItemBuilder::GetOutlineRenderProxyList(
    uint32_t renderID) const
{

    auto it = mRenderProxyListTable.find(renderID);

    if (it != mRenderProxyListTable.cend())
        return it->second->mDrawOutlineProxyList;

    return mEmptyRenderProxyList;

    // TODO: 여기에 return 문을 삽입합니다.
}
void Render::ObjectRenderItemBuilder::DrawAABB(const CoreMath::AABB &aabb, uint32_t renderID,
                                               const CoreMath::Vector4 &color)
{
    auto assetManager = CoreAsset::AssetManager::GetInstance();
    Core::StaticMeshRenderProxy *staticRenderProxy = GetStaticMeshRenderProxy();
    // cube staticMesh
    staticRenderProxy->mStaticMesh =
        static_cast<CoreAsset::StaticMesh *>(assetManager->GetAsset<CoreAsset::StaticMesh>("Engine/Cube").Get());

    staticRenderProxy->mSubMeshMaterialList.resize(staticRenderProxy->mStaticMesh->GetSubMeshVector().size());
    staticRenderProxy->mSubMeshOutlineFlagList.resize(staticRenderProxy->mSubMeshMaterialList.size(), false);
    // wireframe material
    staticRenderProxy->mSubMeshMaterialList[0] = static_cast<CoreAsset::Material *>(
        assetManager->GetAsset<CoreAsset::Material>("Engine/DefaultColliderMaterial").Get());
    staticRenderProxy->mRenderProxyType = Core::ERenderProxyType::eStaticMesh;
    // editoroverlay
    staticRenderProxy->mIsEditorOverlay = true;
    staticRenderProxy->mRenderID = renderID;
    staticRenderProxy->mCustomShaderData = color;

    CoreMath::Vector3 size = aabb.mMax - aabb.mMin;
    CoreMath::Vector3 pos = (aabb.mMax + aabb.mMin) / 2;

    staticRenderProxy->mTransform = CoreMath::Matrix4X4::MakeTransform(pos, {0, 0, 0, 1}, size);

    mRenderProxyListTable[renderID]->mTempRenderProxyList.push_back(staticRenderProxy);
}

void Render::ObjectRenderItemBuilder::DrawLine(uint32_t renderID, const CoreMath::Vector3 &start,
                                               const CoreMath::Vector3 &end, const CoreMath::Vector4 &color)
{

    auto tableIt = mRenderProxyListTable.find(renderID);

    if (tableIt == mRenderProxyListTable.end())
        return;

    DebugLineRednerCommand rmd;
    rmd.mStart = start;
    rmd.mEnd = end;
    rmd.mColor = color;
    tableIt->second->mDebugLineRenderCommandList.push_back(rmd);
}

void Render::ObjectRenderItemBuilder::ClearTempProxyList()
{

    for (auto &table : mRenderProxyListTable)
    {
        for (auto proxy : table.second->mTempRenderProxyList)
        {
            ReleaseRenderProxy(proxy);
        }

        table.second->mTempRenderProxyList.clear();
    }
}

void Render::ObjectRenderItemBuilder::ClearDebugLineCommandList()
{

    for (auto &table : mRenderProxyListTable)
    {
        table.second->mDebugLineRenderCommandList.clear();
    }
}

Core::StaticMeshRenderProxy *Render::ObjectRenderItemBuilder::GetStaticMeshRenderProxy()
{
    Core::StaticMeshRenderProxy *proxy = nullptr;

    if (mStaticMeshRenderProxyPool.size() == 0)
    {
        // 생성
        proxy = new Core::StaticMeshRenderProxy;
        return proxy;
    }
    proxy = mStaticMeshRenderProxyPool.back();
    mStaticMeshRenderProxyPool.pop_back();

    return proxy;
}

void Render::ObjectRenderItemBuilder::ReleaseRenderProxy(Core::RenderProxy *proxy)
{

    if (proxy == nullptr)
        return;

    if (proxy->mRenderProxyType == Core::ERenderProxyType::eStaticMesh)
    {
        mStaticMeshRenderProxyPool.push_back(static_cast<Core::StaticMeshRenderProxy *>(proxy));
    }
}

void Render::ObjectRenderItemBuilder::DestroyRenderProxyList()
{
    ClearTempProxyList();
    ClearDebugLineCommandList();
    for (auto proxy : mStaticMeshRenderProxyPool)
    {
        delete proxy;
    }

    mStaticMeshRenderProxyPool.clear();
}
