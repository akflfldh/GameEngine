#pragma once

#include <CommonHeader/GpuTypes.h>
#include <CoreAsset/CoreAssetDLLMacro.h>
#include <CoreBase/Arch.h>
#include <CoreBase/Blitable_Trait.h>
#include <CoreMath/CoreMath.h>
#include <array>
#include <string>

namespace CoreAsset
{

class Asset;
using AssetID = uint64_t;
#define NoneAssetID 0

// Asset과 생성 컨텍스트가 공유하는 메모리 로드 상태이며, 실제 상태의 보관과 갱신은 Asset이 담당한다.
// 디스크 저장 여부나 dirty 상태를 나타내지 않는다.
enum class EAssetLoadState
{
    Unloaded,
    Loading,
    Loaded, // 실제 에셋 데이터가 메모리에 준비된 상태다.
    Failed
};

enum class EAssetType : uint32_t
{
    eUnknown = 0,
    eTexture = 0x1 << 1,
    eStaticMesh = 0x1 << 2,
    eMaterial = 0x1 << 3,
    eSkinningMesh = 0x1 << 4,
    eShader = 0x1 << 5,
    eAnimation = 0x1 << 6,
    eSound = 0x1 << 7,
    ePrefab = 0x1 << 8,
    eCount = 0x1 << 9,
    eMap = 0x1 << 10,
    eFont = 0x1 << 11,
    eCXX = 0x1 << 12,
    eSkeleton = 0x1 << 13,
    eAnimationTransitionSet = 0x1 << 14
};

// 이설정들보고 렌더시스템이 샘플러를 동적으로 만들어서 바인딩하거나,기존의 샘플러를 바인딩할것이다.
enum class EWrapMode
{
    eClamp,
    eRepeat,
    eMirror
}; // 머터리얼속성
enum class EFilterMode
{
    ePoint,
    eLinear,
    eTrilinear
}; // 머터리얼속성

struct CreationTextureAssetInfo
{
    GRM::TextureDesc mTextureDesc;
    std::string mName;
};

struct CORE_ASSET_API TextureProperties
{

    GRM::TextureDesc mMetaData;
    bool mIsSRGB; // 감마보정을해야하는지 +(일반 컬러텍스처인지, 데이터텍스처인지도 나타낸다(대략적으로));

    void SetRawData(uint8_t *pMemory, size_t size);
};

CORE_ASSET_API Arch &operator<<(Arch &, TextureProperties &textureProperty);

using MeshIndexType = uint32_t;

struct LineVertex
{
    CoreMath::Vector4 mPos;
    CoreMath::Vector4 mColor;
};

struct StaticVertex
{
    CoreMath::Vector3 mPos;
    CoreMath::Vector2 mTex;
    CoreMath::Vector3 mNormal;
    CoreMath::Vector4 mTangent;
};

/// 스키닝 가능한 mesh 정점의 영구 CPU 에셋 형식이다.
/// joint index는 SkinBinding의 palette index이며 Skeleton joint index나 GPU handle을 직접 저장하지 않는다.
struct SkinningVertex
{
    CoreMath::Vector3 mPos;
    CoreMath::Vector2 mTex;
    CoreMath::Vector3 mNormal;
    CoreMath::Vector4 mTangent;
    std::array<uint32_t, 8> mJointIndices{};
    std::array<float, 8> mJointWeights{};
};

struct SubMesh
{

    // 머터리얼 id
    // vertex offset,
    // index offset, index num
    std::string mName;
    AssetID mMaterialID;
    uint64_t mVertexOffset;
    uint64_t mIndexOffset;
    uint64_t mIndexNum;
};

struct MeshPart
{
    uint32_t mStartSubMeshIndex = 0;
    uint32_t mSubMeshCount = 0;
};

struct MeshPartInstance
{
    uint32_t mMeshPartIndex = 0;
    CoreMath::Matrix4X4 mLocalTransform;
    std::vector<CoreAsset::AssetID> mSubMeshMaterialIDList;
};

CORE_ASSET_API Arch &operator<<(Arch &arch, SubMesh &subMesh);

struct GlyphPlaneBounds
{
    float mLeft;
    float mBottom;
    float mRight;
    float mTop;
};
using AtlasBounds = GlyphPlaneBounds;

struct FontGlyph
{
    uint32_t mUnicode;
    float mAdvance;
    GlyphPlaneBounds mPlaneBounds;
    AtlasBounds mAltasBounds;
};

struct FontMatrix
{
    float mLineHeight;
    float mAscender;
    float mDescender;
    float mUnderlineY;
};

struct FontAltas
{
    int mDistanceRange;
    float mSize; // 한문자 사이즈
    uint32_t mWidth;
    uint32_t mHeight;
    float mPxRange; // == (mDistanceRange/mSize);
};

enum class EAssetLoadResultFlag
{
    eSuccess = 0, // 성공
    eAlready,     // 이미 에셋이 존재
    eFail,        // 실패
    eIgnore       // 옵션으로 인해 무시됨
};

struct AssetLoadExecutionContext
{
    // 해당 타입에 대응되는에셋이면 로드를 하지않는닫.
    EAssetType mIgnoredAssetType = EAssetType::eUnknown;
};

struct AssetLoadResult
{
    Asset *pAsset = nullptr;
    EAssetType mAssetType = EAssetType::eUnknown;
    EAssetLoadResultFlag mResultFlag;

    std::string mFailedReason;
};

enum class EShadingModel : uint8_t
{
    eNone = 0,
    eUnlit,
};

} // namespace CoreAsset

template <> struct Blitable_Trait<CoreAsset::StaticVertex>
{
    static constexpr bool value = true;
};
template <> struct Blitable_Trait<CoreAsset::SkinningVertex>
{
    static constexpr bool value = true;
};

template <> struct Blitable_Trait<CoreMath::Vector2>
{
    static constexpr bool value = true;
};

template <> struct Blitable_Trait<CoreMath::Vector3>
{
    static constexpr bool value = true;
};

template <> struct Blitable_Trait<CoreMath::Vector4>
{
    static constexpr bool value = true;
};

template <> struct Blitable_Trait<CoreMath::Quaternion>
{
    static constexpr bool value = true;
};

template <> struct Blitable_Trait<CoreMath::Matrix4X4>
{
    static constexpr bool value = true;
};

namespace std
{

template <> struct hash<CoreAsset::EAssetType>
{
    size_t operator()(const CoreAsset::EAssetType &assetType) const
    {
        return static_cast<const uint32_t>(assetType);
    }
};

} // namespace std
