#pragma once

#include <RenderFrontend/RenderFrontendType.h>
namespace Render
{

struct CommonPassData
{
    CoreMath::Matrix4X4 gViewProj;
};

struct StaticMeshObjectData
{
    CoreMath::Matrix4X4 gWorld;
    CoreMath::Matrix4X4 gWorldInvTrans;
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

class RenderUploadManager
{

  public:
    static RenderUploadManager *GetInstance();
    RenderUploadManager();
    ~RenderUploadManager();

    // StaticMeshRenderCommand → ObjectStaticData 채워서 업로드
    // void UploadCommonPassData(CommonPassData &data);
    void UploadStaticMeshObjectBuffer(const StaticMeshRenderCommnad &cmd, StaticMeshObjectData &data);
    void UploadDebugColliderBuffer(const StaticMeshRenderCommnad &cmd, DebugColliderData &data);
    void UploadStaticMeshOutlineData(const StaticMeshOutlineRenderCommand &cmd, StaticMeshOutlineData &data);
    void UploadStaticMeshGizmoData(const StaticMeshRenderCommnad &cmd, StaticMeshGizmoData &data);

    void UploadDefaultMaterialData(const MaterialRenderSnapshot &snapshot, DefaultMaterialData &data);

    void UploadDefaultLightData(const LightRenderCommand &cmd, DefaultLightData &data);

    void UploadBillboardData(const BillboardRenderCommand &cmd, BillboardData &data);

    // void UploadPassBuffer(const PassData &passData);
};

} // namespace Render