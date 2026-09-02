#pragma once

#pragma once

#include "CoreAsset/AssetType.h"
#include <CommonHeader/GpuTypes.h>
#include <CoreAsset/CoreAssetDLLMacro.h>
#include <CoreBase/FString.h>
#include <RenderSystem/MaterialType.h>
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

    Render::MaterialID mGpuMaterialID;
    std::vector<std::pair<std::string, AssetID>> mTexResourceList;
    std::vector<uint32_t> mSamplerResourceList;

    CoreMath::Vector3 mDiffuseColor;
    float mDiffuseFactor;
    CoreMath::Vector3 mSpecular;
    float mSpecularFactor;
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
    std::unique_ptr<IntermediateAsset> mIntermediateAsset;
    bool mValid = true;
};

enum class EImportDependencyType
{
    eSubMeshDefaultMaterial = 0, // subMesh별 DefaultMaterial
    eMeshPartInstanceMaterial,   // MeshPartInstance 가 Material에 의존
    eMaterialTexture,            // Material Texture에 의존
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
    // OwnerAsset이 Dependency Asset에 의존한다.
    ImportAssetKey mOwnerAssetKey;
    ImportAssetKey mDependencyAssetKey;

    int mSlotIndex; // MeshPartInnstance-subMeshMaterial : meshPartInstance index  ,    materialTexture : texture slot
                    // index
};

struct ImportRequestTextureContext
{
    std::string mFilePath;
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