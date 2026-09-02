#pragma once

#include <Core/CoreType.h>
#include <Core/IRenderProxyManager.h>
#include <CoreAsset/AssetType.h>
#include <D3DGpuResourceManager/GRMPtr.h>
#include <RenderFrontend/RenderFrontendDllMarco.h>
#include <RenderSystem/RenderType.h>
#include <UiSystem/IUIRenderProxyManager.h>
#include <stdint.h>
#include <vector>

enum class ERenderPipelineTag : uint8_t
{
    eOpaque = 0, // 불투명
    eOpaqueUI    // UI
};

namespace Core
{
class CommandContext;
}
namespace CoreAsset
{
class Texture;
}

namespace Render
{

enum class ERenderPassType : uint8_t
{
    eMain = 0,
    eDepthOnly,
    eShadow,
    eOutlineStencil,
    eOutlineDraw,
    eEditorOverlay,
    ePicking,
    eGrayScale,
    eDebugGrid,
    eBillboard,
    eDebugLine,
    eSkySphere
};

enum class ERenderGeometryType
{
    eStaticMesh = 0,
    eSkinnedMesh,
    eBillboard,
    eDebugLine,
    eUI,
};

struct RenderMaterialContext
{
    ERenderGeometryType mGeometryType = ERenderGeometryType::eStaticMesh;
    CoreAsset::EShadingModel mShadingModel = CoreAsset::EShadingModel::eNone;
    bool mTransparent = false;

    bool operator==(const RenderMaterialContext &other) const
    {

        return mGeometryType == other.mGeometryType && mTransparent == other.mTransparent &&
               mShadingModel == other.mShadingModel;
    };
};

struct UIRenderCommand
{
    // CoreAsset::Material *mUIMaterial;
    CoreAsset::AssetID mUIMaterialID;
    Render::MaterialID mGpuMaterialID;
    bool mUseScissorRect;
    SRECT mScissorRect;

    // std::vector<UI::UIVertex> mVertexList;
    // std::vector<uint32_t> mIndexList;

    uint32_t mVertexStartOffset = 0;
    uint32_t mVertexNum = 0;

    uint32_t mIndexStartOffset = 0;
    uint32_t mIndexNum = 0;
};

struct MaterialRenderSnapshot
{
    bool mMaterialUploadDirtyFlag = false;
    bool mUseExplicitGpuMat = false;
    uint32_t mGpuMatID = 0;

    CoreMath::Vector3 mDiffuseFactor = {1, 1, 1};
    CoreMath::Vector3 mAmbient = {0, 0, 0};
    float mMetallic;
    float mRoughness;
    std::vector<CoreAsset::Texture *> mAlbedoMapList;
    CoreAsset::Texture *mNormalMap = nullptr;
    uint32_t mHandle = 0;
    CoreAsset::EShadingModel mShadingModel = CoreAsset::EShadingModel::eNone;
};

struct LightRenderCommand
{
    Core::ELightType mLightType;
    CoreMath::Vector3 mStrength;
    CoreMath::Vector3 mDirection;
    CoreMath::Vector3 mPosition;
    float mFalloffStart;
    float mFalloffEnd;
    float mSpotPower;
};

struct DebugLineRednerCommand
{
    CoreMath::Vector3 mStart = {0, 0, 0};
    CoreMath::Vector3 mEnd = {0, 0, 0};
    CoreMath::Vector4 mColor = {0, 0, 0, 1.0f};
};

struct RenderCommand
{

    CoreMath::Matrix4X4 mTransform;
    bool mDrawOutline = false;
};

struct BillboardRenderCommand : public RenderCommand
{
    CoreMath::Vector2 mSize;
    CoreAsset ::Texture *mTexture = nullptr;
};

struct MeshRenderCommand : public RenderCommand
{
    uint32_t mMaterialHandle = 0;
};

struct StaticMeshRenderCommnad : public MeshRenderCommand
{
    // std::vector<CoreAsset::Material *> mSubMeshMaterialList;
    CoreAsset::StaticMesh *mStaticMesh = nullptr;
    //  CoreAsset::Material *mMaterial = nullptr;
    int mSubMeshIndex = 0;
    CoreMath::Vector4 mCustomShaderData;
};

struct StaticMeshOutlineRenderCommand : public StaticMeshRenderCommnad
{
    // size_t mStaticMeshRenderCommnadIndex = 0;
    //  StaticMeshRenderCommnad *mStaticMeshRenderCommnadPtr = nullptr;
    float mOutlineColorR = 1.0F;
    float mOutlineColorG = 1.0f;
    float mOutlineColorB = 1.0f;
    float mOutlineColorA = 1.0f;
};

struct SkySphereSnapshot
{
    CoreAsset::Texture *mSkyTexture = nullptr;
    CoreAsset::StaticMesh *mSphereMesh = nullptr;
    CoreMath::Matrix4X4 mTransform;
    CoreMath::Vector3 mTintColor = {1, 1, 1};
    float mIntensity = 1.0f;

    bool mActiveFlag = false;
};
struct RenderPassExecuteContext
{
    class IRenderSystem *renderSystem;

    //    std::vector<StaticMeshRenderCommnad> mStaticMeshRenderCommand;
    std::vector<StaticMeshRenderCommnad> mOpaqueStaticMeshRenderCommandList;
    std::vector<StaticMeshRenderCommnad> mTransparentStaticMeshRenderCommandList;
    std::vector<StaticMeshRenderCommnad> mEditorOverlayStaticMeshRenderCommandList;
    std::vector<BillboardRenderCommand> mBillboardRenderCommandList;
    std::vector<StaticMeshOutlineRenderCommand> mOutlineStaticMeshRenderCommandIndexList;
    std::vector<DebugLineRednerCommand> mDebugLineRenderCommandList;
    std::vector<UIRenderCommand> mUIRenderCommandList;

    //   std::vector<MaterialRenderSnapshot> mMaterialRenderSnapshotList;
    std::unordered_map<uint32_t, MaterialRenderSnapshot> mMaterialRenderSnapshotTable;

    std::unordered_map<CoreAsset::AssetID, MaterialRenderSnapshot> mUIMaterialRenderSnapshotTable;

    SkySphereSnapshot mSkySphereSnapshot;

    std::vector<LightRenderCommand> mLightRenderCommandList;

    class RenderPassGraph *renderPassGraph;
    Render::Viewport mGlobalSceneViewport;
    Core::GlobalFrameData mGlobalFrameData;
    Core::GlobalFrameData mUIGlobalFrameData;

    std::vector<UI::UIVertex> *mUIVertexBuffer = nullptr;
    std::vector<uint32_t> *mUIIndexBuffer = nullptr;

    Core::CommandContext *mCommandContext;
};

struct RenderResourceDesc
{

    uint32_t mWidth;
    uint32_t mHeight;
    GRM::ETextureFormat mFormat;
    GRM::ETextureUsage mUsage;
};

struct PooledRenderResource
{
    GRM::GRMPtr mResource;
    bool mIsInUse;
    uint64_t mLastUsedFrame = 0;
    uint64_t mLastUsedFence = 0;
    RenderResourceDesc mDesc;
    std::string mName;
    bool isMatch(const RenderResourceDesc &rhs) const;
};

} // namespace Render