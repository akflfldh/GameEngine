#pragma once

#include <Core/CoreDllExport.h>
#include <Core/CoreType.h>
#include <CoreMath/Geometry.h>
#include <stdint.h>
#include <vector>

namespace CoreAsset
{
class StaticMesh;
class Material;
class Texture;
} // namespace CoreAsset

namespace Core
{

class Map;
enum class ERenderProxyType : uint8_t
{
    eStaticMesh = 0,
    eSkinningMesh,
    eBillboard
};

struct LightProxy
{
    uint32_t mRenderID = 0;
    Core::ELightType mLightType;
    CoreMath::Vector3 mStrength;
    CoreMath::Vector3 mDirection;
    CoreMath::Vector3 mPosition;
    float mFalloffStart;
    float mFalloffEnd;
    float mSpotPower;
};

struct RenderProxy
{
    CoreMath::Matrix4X4 mTransform;
    // Core::Map *mMap = nullptr;
    uint32_t mRenderID = 0;
    bool mDrawOutline = false;
    ERenderProxyType mRenderProxyType;
    virtual ~RenderProxy() = default;
};

struct MeshRenderProxy : public RenderProxy
{
    std::vector<CoreAsset::Material *> mSubMeshMaterialList;
    std::vector<bool> mSubMeshOutlineFlagList;
};

struct StaticMeshRenderProxy : public MeshRenderProxy
{
    CoreAsset::StaticMesh *mStaticMesh = nullptr;
    StaticMeshRenderProxy()
    {
        mRenderProxyType = ERenderProxyType::eStaticMesh;
    }
    CoreMath::Vector4 mCustomShaderData;

    bool mIsEditorOverlay = false;
    virtual ~StaticMeshRenderProxy() = default;
};

struct BillboardRenderProxy : public RenderProxy
{
    CoreMath::Vector2 mSize;
    CoreAsset::Texture *mTexture = nullptr;
    bool mDepthTest = true;
    BillboardRenderProxy()
    {
        mRenderProxyType = ERenderProxyType::eBillboard;
    }
};

class CORE_API_LIB IRenderProxyManager
{
  public:
    static IRenderProxyManager *GetInstance();

    virtual void Update() = 0;
    virtual void EndFrame() = 0;

    // GetInstance()/호출전 가장먼저 호출할것
    static void SetRenderProxyManager(IRenderProxyManager *renderProxyManager);

    virtual void RegisterRenderProxy(RenderProxy *renderProxy) = 0;
    virtual void UnRegisterRenderProxy(RenderProxy *renderProxy) = 0;
    virtual void RegisterLightProxy(LightProxy *lightProxy) = 0;
    virtual void UnRegisterLightProxy(LightProxy *lightProxy) = 0;
    virtual void RegisterBillboardProxy(BillboardRenderProxy *billboardProxy) = 0;
    virtual void UnRegisterBillboardProxy(BillboardRenderProxy *billboardProxy) = 0;

    virtual void SetProxyDrawOutline(Core::RenderProxy *renderProxy, bool bDraw) {};

    virtual void DrawAABB(const CoreMath::AABB &aabb, uint32_t renderID,
                          const CoreMath::Vector4 &color = {1, 0, 0, 0}) {};

    virtual void DrawLine(uint32_t renderID, const CoreMath::Vector3 &start, const CoreMath::Vector3 &end,
                          const CoreMath::Vector4 &color) {};

    virtual void DrawArrow(const CoreMath::Vector3 &startPosWorld, uint32_t renderID) {};

    /*
            DrawLine
            DrawArrow
            DrawWireSphere
            DrawWireCone
    */

  private:
    static IRenderProxyManager *mRenderProxyManager;
};

} // namespace Core