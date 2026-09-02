#pragma once

#include <Core/IRenderProxyManager.h>
#include <CoreAsset/AssetType.h>
#include <CoreMath/Geometry.h>
#include <RenderFrontend/RenderFrontendDllMarco.h>
#include <RenderFrontend/RenderFrontendType.h>
#include <RenderSystem/IRenderSystem.h>
#include <RenderSystem/RenderType.h>
#include <memory>
#include <unordered_map>
#include <vector>

namespace CoreAsset
{
class Mesh;
class Material;

} // namespace CoreAsset

namespace GRM
{
class IGpuResourceManager;

}

namespace Core
{
class ViewportController;
struct RenderProxy;
struct StaticMeshRenderProxy;

} // namespace Core
class World;
class Map;
namespace Render
{

class AssetResolver;

struct RenderProxyContext
{
    std::vector<Core::RenderProxy *> mRenderProxyList;
    std::vector<Core::RenderProxy *> mDrawOutlineProxyList;
    std::vector<Core::RenderProxy *> mTempRenderProxyList;
    std::vector<Core::LightProxy *> mLightProxyList;
    std::vector<Core::BillboardRenderProxy *> mBillboardProxyList;
    std::vector<Render::DebugLineRednerCommand> mDebugLineRenderCommandList; // 한프레임 라이프
};

class RENDER_FRONTEND_API ObjectRenderItemBuilder : public Core::IRenderProxyManager
{
  public:
    static ObjectRenderItemBuilder *GetInstance();
    ObjectRenderItemBuilder(Render::IRenderSystem *renderSystem, GRM::IGpuResourceManager *gpuResourceManager,
                            AssetResolver *assetResolver);
    ~ObjectRenderItemBuilder();

    ObjectRenderItemBuilder(const ObjectRenderItemBuilder &) = delete;
    ObjectRenderItemBuilder &operator=(const ObjectRenderItemBuilder &) = delete;

    virtual void Update() override;
    virtual void EndFrame() override;

    void RegisterRenderContext(uint32_t renderID);

    // 하는일이동일하여 통합할것
    void RegisterRenderProxy(Core::RenderProxy *renderProxy);
    void UnRegisterRenderProxy(Core::RenderProxy *renderProxy);

    void RegisterLightProxy(Core::LightProxy *lightProxy);
    void UnRegisterLightProxy(Core::LightProxy *lightProxy);

    void RegisterBillboardProxy(Core::BillboardRenderProxy *billboardProxy);
    void UnRegisterBillboardProxy(Core::BillboardRenderProxy *billboardProxy);

    virtual void SetProxyDrawOutline(Core::RenderProxy *renderProxy, bool bDraw) override;

    const std::vector<Core::RenderProxy *> &GetRenderProxyList(uint32_t renderID) const;
    RenderProxyContext *GetRenderProxyContext(uint32_t renderID) const;

    const std::vector<Core::RenderProxy *> &GetOutlineRenderProxyList(uint32_t renderID) const;

    void DrawAABB(const CoreMath::AABB &aabb, uint32_t renderID,
                  const CoreMath::Vector4 &color = {1, 0, 0, 0}) override;
    virtual void DrawLine(uint32_t renderID, const CoreMath::Vector3 &start, const CoreMath::Vector3 &end,
                          const CoreMath::Vector4 &color) override;

  private:
    void ClearTempProxyList();
    void ClearDebugLineCommandList();

    Core::StaticMeshRenderProxy *GetStaticMeshRenderProxy();
    void ReleaseRenderProxy(Core::RenderProxy *proxy);

    void DestroyRenderProxyList();

  private:
    static ObjectRenderItemBuilder *mInstance;
    Render::IRenderSystem *mRenderSystem;
    GRM::IGpuResourceManager *mGpuResourceManager;
    AssetResolver *mAssetResolver;

    std::vector<Core::RenderProxy *> mRenderProxyList;
    std::vector<Core::RenderProxy *> mEmptyRenderProxyList;

    std::vector<Core::RenderProxy *> mTempRenderProxyList;

    std::vector<Core::StaticMeshRenderProxy *> mStaticMeshRenderProxyPool;

    // Render ID : RenderProxyList
    std::unordered_map<uint32_t, std::unique_ptr<RenderProxyContext>> mRenderProxyListTable;

    std::vector<Render::RenderItem> mRenderItemVector;
};

} // namespace Render
