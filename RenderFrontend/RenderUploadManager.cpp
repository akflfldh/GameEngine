#include "RenderUploadManager.h"

Render::RenderUploadManager *Render::RenderUploadManager::GetInstance()
{
    static RenderUploadManager instance;
    return &instance;
}

Render::RenderUploadManager::RenderUploadManager() {}

Render::RenderUploadManager::~RenderUploadManager() {}

void Render::RenderUploadManager::UploadStaticMeshObjectBuffer(const StaticMeshRenderCommnad &cmd,
                                                               StaticMeshObjectData &data)
{

    data.gWorld = cmd.mTransform;
    data.gWorldInvTrans = data.gWorld.GetInversed().GetTransposed();
}

void Render::RenderUploadManager::UploadDebugColliderBuffer(const StaticMeshRenderCommnad &cmd, DebugColliderData &data)
{

    data.gWorld = cmd.mTransform;
    data.gColor = cmd.mCustomShaderData;
}

void Render::RenderUploadManager::UploadStaticMeshOutlineData(const StaticMeshOutlineRenderCommand &cmd,
                                                              StaticMeshOutlineData &data)
{
    data.gWorld = cmd.mTransform;
    data.gOutlineColor[0] = cmd.mOutlineColorR;
    data.gOutlineColor[1] = cmd.mOutlineColorG;
    data.gOutlineColor[2] = cmd.mOutlineColorB;
    data.gOutlineColor[3] = cmd.mOutlineColorA;
}

void Render::RenderUploadManager::UploadStaticMeshGizmoData(const StaticMeshRenderCommnad &cmd,
                                                            StaticMeshGizmoData &data)
{
    data.gWorld = cmd.mTransform;
    data.gColor = cmd.mCustomShaderData;
}

void Render::RenderUploadManager::UploadDefaultMaterialData(const MaterialRenderSnapshot &snapshot,
                                                            DefaultMaterialData &data)
{
    data.gDiffuseFactor = snapshot.mDiffuseFactor;
    data.gMetallic = snapshot.mMetallic;
    data.gRoughness = snapshot.mRoughness;
    data.gAmbient = snapshot.mAmbient;
}

void Render::RenderUploadManager::UploadDefaultLightData(const LightRenderCommand &cmd, DefaultLightData &data)
{

    data.mDirection = cmd.mDirection;
    data.mFalloffEnd = cmd.mFalloffEnd;
    data.mFalloffStart = cmd.mFalloffStart;
    data.mLightType = (int)cmd.mLightType;
    data.mPosition = cmd.mPosition;
    data.mSpotPower = cmd.mSpotPower;
    data.mStrength = cmd.mStrength;
}

void Render::RenderUploadManager::UploadBillboardData(const BillboardRenderCommand &cmd, BillboardData &data)
{
    data.mSize = cmd.mSize;
    data.mWorld = cmd.mTransform;
}
