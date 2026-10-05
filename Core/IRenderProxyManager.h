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
class SkinningMesh;
} // namespace CoreAsset

class Map;
namespace Core
{

enum class ERenderProxyType : uint8_t
{
    eStaticMesh = 0,
    eSkinningMesh,
    eBillboard
};

/// LightComponent가 소유하고 RenderFrontend가 비소유 포인터로 참조하는 CPU 조명 데이터다.
/// 발광 상태는 렌더 명령 생성에만 사용하며 GPU 조명 버퍼의 레이아웃에는 포함하지 않는다.
struct LightProxy
{
    uint32_t mRenderID = 0;
    Core::ELightType mLightType;
    bool mLightEnabled = true;
    CoreMath::Vector3 mStrength;
    CoreMath::Vector3 mDirection;
    CoreMath::Vector3 mRight;
    CoreMath::Vector3 mUp;
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
    CoreMath::Vector4 mCustomShaderData;
    bool mIsEditorOverlay = false;
};

struct StaticMeshRenderProxy : public MeshRenderProxy
{
    CoreAsset::StaticMesh *mStaticMesh = nullptr;
    StaticMeshRenderProxy()
    {
        mRenderProxyType = ERenderProxyType::eStaticMesh;
    }

    virtual ~StaticMeshRenderProxy() = default;
};

/// SkeletalMeshComponent가 RenderFrontend에 전달하는 CPU 측 렌더 경계 데이터다.
/// SkinningMesh와 Material 에셋의 수명은 소유하지 않으며, 인스턴스별 최종 행렬은 컴포넌트의
/// mutable buffer를 직접 참조하지 않도록 값으로 복사해 보관한다.
struct SkeletalMeshRenderProxy : public MeshRenderProxy
{
    CoreAsset::SkinningMesh *mSkinningMesh = nullptr;
    std::vector<CoreMath::Matrix4X4> mFinalMatrixList;

    SkeletalMeshRenderProxy()
    {
        mRenderProxyType = ERenderProxyType::eSkinningMesh;
    }
    virtual ~SkeletalMeshRenderProxy() = default;
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
