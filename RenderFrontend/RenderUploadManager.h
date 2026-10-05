#pragma once

#include <RenderFrontend/RenderFrontendType.h>
namespace Render
{

struct CommonPassData
{
    CoreMath::Matrix4X4 gViewProj;
};

struct MeshObjectData
{
    CoreMath::Matrix4X4 gWorld;
    CoreMath::Matrix4X4 gWorldInvTrans;

    uint32_t gPaletteOffset = 0;
    uint32_t gPaletteCount = 0;
};

struct DebugColliderData
{
    CoreMath::Matrix4X4 gWorld;
    CoreMath::Vector4 gColor;
};

struct StaticMeshOutlineData
{
    CoreMath::Matrix4X4 gWorld;
    CoreMath::Vector4 gOutlineColor;
    // 스키닝 outline 변형에서만 사용하며, 정적 메시 커맨드는 0을 전달한다.
    uint32_t gPaletteOffset = 0;
    uint32_t gPaletteCount = 0;
    uint32_t gPadding[2] = {};
};

struct StaticMeshGizmoData
{
    CoreMath::Matrix4X4 gWorld;
    CoreMath::Vector4 gColor;
};

struct DefaultMaterialData
{
    CoreMath::Vector3 gDiffuseFactor;
    float gMetallic;
    CoreMath::Vector3 gAmbient;
    float gRoughness;
    CoreMath::Vector3 gEmissiveColor;
    float gEmissiveIntensity;
};

struct DefaultLightData
{
    CoreMath::Vector3 mStrength;
    float mFalloffStart;
    CoreMath::Vector3 mDirection;
    float mFalloffEnd;
    CoreMath::Vector3 mPosition;
    float mSpotPower;
    int mLightType;
    CoreMath::Vector3 mPad1;
};

struct BillboardData
{
    CoreMath::Matrix4X4 mWorld;
    CoreMath::Vector2 mSize;
    float mPadding1;
    float mPadding2;
};

struct SkinPaletteData
{
    CoreMath::Matrix4X4 mMatrix;
};

class RenderUploadManager
{

  public:
    static RenderUploadManager *GetInstance();
    RenderUploadManager();
    ~RenderUploadManager();

    // StaticMeshRenderCommand → ObjectStaticData 채워서 업로드
    // void UploadCommonPassData(CommonPassData &data);
    void UploadStaticMeshObjectBuffer(const MeshRenderCommand &cmd, MeshObjectData &data);
    void UploadDebugColliderBuffer(const MeshRenderCommand &cmd, DebugColliderData &data);
    void UploadStaticMeshOutlineData(const MeshOutlineRenderCommand &cmd, StaticMeshOutlineData &data);
    void UploadStaticMeshGizmoData(const MeshRenderCommand &cmd, StaticMeshGizmoData &data);

    void UploadDefaultMaterialData(const MaterialRenderSnapshot &snapshot, DefaultMaterialData &data);

    void UploadDefaultLightData(const LightRenderCommand &cmd, DefaultLightData &data);

    void UploadBillboardData(const BillboardRenderCommand &cmd, BillboardData &data);

    // void UploadPassBuffer(const PassData &passData);
};

} // namespace Render
