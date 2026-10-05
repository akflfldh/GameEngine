#pragma once

#pragma once

#include "CoreAsset/AssetType.h"
#include <CommonHeader/GpuTypes.h>
#include <CoreAsset/AnimationTypes.h>
#include <CoreAsset/CoreAssetDLLMacro.h>
#include <CoreBase/FString.h>
#include <string>
#include <vector>

namespace QuadRW
{
class BinaryWriter;
class BinaryReader;
} // namespace QuadRW

// intermeidate data라고 해야겠다 -> 새롭게

namespace CoreAsset
{
class IntermediateAssetFactory;

struct CORE_ASSET_API IntermediateAsset
{

    //  AssetID mAssetID = NoneAssetID;
    EAssetType mAssetType = EAssetType::eUnknown;
    FString mAssetName = "";

    //  virtual void Serialize(QuadRW::BinaryWriter &writer);
    //   virtual void DeSerialize(QuadRW::BinaryReader &reader);

    IntermediateAsset(EAssetType assetType = EAssetType::eUnknown);
    virtual ~IntermediateAsset() = default;
};

// void SerializeTextureProperties(const TextureProperties &textureProperties, QuadRW::BinaryWriter &writer);
// void DeSerializeTextureProperties(TextureProperties &oTextureProperties, QuadRW::BinaryReader &reader);

struct CORE_ASSET_API IntermediateTexture : public IntermediateAsset
{

    //  TextureProperties mTextureProperties;
    // GRM::ETextureUsage mTextureUsage;
    GRM::TextureDesc mTextureRawData;
    //  virtual void Serialize(QuadRW::BinaryWriter &writer) override;
    //  virtual void DeSerialize(QuadRW::BinaryReader &reader) override;

    IntermediateTexture();
};

struct CORE_ASSET_API IntermediateMaterial : public IntermediateAsset
{

    std::vector<std::pair<std::string, AssetID>> mTexResourceList;
    std::vector<uint32_t> mSamplerResourceList;

    CoreMath::Vector3 mDiffuseColor = {1.0f, 1.0f, 1.0f};
    float mDiffuseFactor = 1.0f;
    CoreMath::Vector3 mSpecular = {1.0f, 1.0f, 1.0f};
    float mSpecularFactor = 1.0f;
    float mShininess = 1.0f;
    float mMetalic = 0.4f;
    EShadingModel mShadingModel = EShadingModel::eNone;

    IntermediateMaterial();
};

struct CORE_ASSET_API IntermediateMesh : public IntermediateAsset
{
    std::vector<MeshIndexType> mIndexVector;
    std::vector<SubMesh> mSubMeshVector;

    std::vector<MeshPart> mMeshPartVector;
    std::vector<MeshPartInstance> mMeshPartInstanceVector;
};

struct CORE_ASSET_API IntermediateStaticMesh : public IntermediateMesh
{
    std::vector<StaticVertex> mVertexVector;
    bool bCaculateAABB = false;

    IntermediateStaticMesh();
};

struct CORE_ASSET_API IntermediateSkinningMesh : public IntermediateMesh
{

    std::vector<SkinningVertex> mVertexVector;
    bool bCaculateAABB = false;

    IntermediateSkinningMesh()
    {
        mAssetType = EAssetType::eSkinningMesh;
    }

    // skin binding

    // offset matrix  list
    std::vector<CoreMath::Matrix4X4> mInverseBindMatrices;

    // palette to joint  list
    std::vector<uint32_t> mPaletteToSkeletonJoint;

    CoreMath::Matrix4X4 mMeshToSkeleton = CoreMath::Matrix4X4::Identity;
};

struct CORE_ASSET_API IntermediateSkeleton : public IntermediateAsset
{
    // joints hierarchy
    //
    std::vector<SkeletonJoint> mSkeletonJoints;

    IntermediateSkeleton()
    {
        mAssetType = EAssetType::eSkeleton;
    }
};

struct CORE_ASSET_API IntermediateAnimationClip : public IntermediateAsset
{

    // (joint key-  key frame list ) list
    //  sample  rate 등등

    float mDurationSeconds = 0.0f;
    uint32_t mSampleRateNumerator = 1;        ///< uniform sample rate의 유리수 분자다.
    uint32_t mSampleRateDenominator = 1;      ///< uniform sample rate의 유리수 분모다.
    uint32_t mSampleCount = 1;                ///< clip 시간축의 sample 수이며 non-constant channel 길이의 기준이다.
    std::vector<AnimationJointTrack> mTracks; ///< stable joint key별 baked local channel 소유 데이터다.

    IntermediateAnimationClip()
    {
        mAssetType = EAssetType::eAnimation;
    }
};

struct CORE_ASSET_API IntermediateFont : public IntermediateAsset
{
    IntermediateFont();
    std::vector<FontGlyph> mGlyphVector;
    FontMatrix mFontMatrix;
    FontAltas mFontAltas;
    AssetID mGlyphAltasID;
};

using ImportAssetKey = std::string;

struct CORE_ASSET_API ImportedIntermediateAsset
{
    ImportAssetKey mKey;
    // 표시 이름과 분리된, 한 ImportPackage 안에서 의존성을 연결하는 고유 식별자다.
    ImportAssetKey mImportKey;
    std::unique_ptr<IntermediateAsset> mIntermediateAsset;
    bool mValid = true;
};

struct AssetCreationContext
{
    AssetID mRequestedAssetID = NoneAssetID;
    std::string mRequestedAssetName = "";
    EAssetLoadState mInitLoadState = EAssetLoadState::Unloaded;
};

struct AssetImportContext
{
    std::string mRegistryPrefix;
    bool mEngineAsset = false;

    std::unordered_map<ImportAssetKey, AssetCreationContext> mCreationContextTable;
};

enum class EImportDependencyType
{
    eSubMeshDefaultMaterial = 0, // subMesh별 DefaultMaterial
    eMeshPartInstanceMaterial,   // MeshPartInstance 가 Material에 의존
    eMaterialTexture,            // Material Texture에 의존
    eSkinningMesh_Skeleton,      // SkinningMesh가 Skeleton에 의존
    eAnimClip_Skeleton           // AnimClip이 Skeleton에 의존
};

enum class EImportDependencySubInfo
{
    eNone = 0,
    eUseDefaultMaterial,
    eDiffuseMap, // material이 의존하는 이 텍스처가 DiffuseMap
    eNormalMap   // maetrial이 의존하는 이 텍스처가 NormalMap
};

struct ImportDependencyContext
{

    EImportDependencyType mDependencyType;
    EImportDependencySubInfo mSubInfo;
    // OwnerAsset이 Dependency Asset에 의존한다. 두 값은 표시 이름이 아닌 mImportKey를 참조한다.
    ImportAssetKey mOwnerAssetKey;
    ImportAssetKey mDependencyAssetKey;

    int mSlotIndex = -1; // MeshPartInnstance-subMeshMaterial : meshPartInstance index  ,    materialTexture : texture
                         // slot index
};

struct ImportRequestTextureContext
{
    std::string mFilePath;
    // 텍스처 임포터의 로컬 키 대신 부모 패키지에서 사용할 import key다.
    ImportAssetKey mKey;
};

struct ImportPackageOption
{
    bool mNeedToCalculateNormals = false;
    bool mNeedToCalculateTangents = false;
};

struct ImportPackage
{
    std::vector<ImportedIntermediateAsset> mInteremdiateAssets;
    std::vector<ImportDependencyContext> mDependencyContexts;
    std::vector<ImportRequestTextureContext> mImportRequestTextureContexts;
    std::string mFailReason;

    ImportPackageOption mOption;

    bool mFailed = false;
};

// SerializeAsset을 생성할 factory가 있어야한다. 그래야 엔진에서 메모리 관리가 가능하다 ( 그렇지않으면 crt 문제가
// 발생한다 (임포터는 유저가 직접등록할수있기에 ))

class CORE_ASSET_API IntermediateAssetFactory
{

  public:
    static IntermediateAssetFactory *GetInstance();
    IntermediateAssetFactory();
    ~IntermediateAssetFactory();

    IntermediateTexture *CreateIntermediateTexture();
    IntermediateMaterial *CreateIntermediateMaterial();
    IntermediateStaticMesh *CreateIntermediateStaticMesh();

    void ReleaseIntermediateAsset(IntermediateTexture *texture);
    void ReleaseIntermediateAsset(IntermediateMaterial *material);
    void ReleaseIntermediateAsset(IntermediateStaticMesh *staticMesh);

    void ReleaseIntermediateAsset(CoreAsset::EAssetType type, IntermediateAsset *asset);

  private:
};

} // namespace CoreAsset
