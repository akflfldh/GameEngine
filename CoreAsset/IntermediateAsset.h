#pragma once

#pragma once

#include "CoreAsset/AssetType.h"
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
    friend class IntermediateAssetFactory;

    //  AssetID mAssetID = NoneAssetID;
    EAssetType mAssetType = EAssetType::eUnknown;
    FString mAssetName = "";

    //  virtual void Serialize(QuadRW::BinaryWriter &writer);
    //   virtual void DeSerialize(QuadRW::BinaryReader &reader);

  protected:
    IntermediateAsset();
};

// void SerializeTextureProperties(const TextureProperties &textureProperties, QuadRW::BinaryWriter &writer);
// void DeSerializeTextureProperties(TextureProperties &oTextureProperties, QuadRW::BinaryReader &reader);

struct CORE_ASSET_API IntermediateTexture : public IntermediateAsset
{
    friend class IntermediateAssetFactory;
    //  TextureProperties mTextureProperties;
    // GRM::ETextureUsage mTextureUsage;
    GRM::TextureDesc mTextureRawData;
    //  virtual void Serialize(QuadRW::BinaryWriter &writer) override;
    //  virtual void DeSerialize(QuadRW::BinaryReader &reader) override;

  protected:
    IntermediateTexture();
};

struct CORE_ASSET_API IntermediateMaterial : public IntermediateAsset
{
    friend class IntermediateAssetFactory;
    Render::MaterialID mGpuMaterialID;
    std::vector<std::pair<std::string, AssetID>> mTexResourceList;
    std::vector<uint32_t> mSamplerResourceList;

  protected:
    IntermediateMaterial();
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

    void ReleaseIntermediateAsset(IntermediateTexture *texture);
    void ReleaseIntermediateAsset(IntermediateMaterial *material);

    void ReleaseIntermediateAsset(CoreAsset::EAssetType type, IntermediateAsset *asset);

  private:
};

} // namespace CoreAsset