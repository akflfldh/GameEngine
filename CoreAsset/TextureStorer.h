#pragma once

#include "CoreAsset/AssetStorer.h"
#include "CoreAsset/CoreAssetDLLMacro.h"
#include <string>
namespace CoreAsset
{
class Texture;
class TextureMetaData;

class CORE_ASSET_API TextureStorer : public AssetStorer
{
  public:
    static TextureStorer *GetInstance();
    ~TextureStorer();

    virtual void StoreAssetFile(Arch &arch, Asset *asset, AssetMetaData *assetMetaData) override;
    virtual bool Store(CoreAsset::Asset *asset, CoreAsset::AssetMetaData *metaData,
                       const std::string &filePath) override;
    virtual bool StoreAssetRawDataFile(Arch &arch, CoreAsset::Asset *asset,
                                       CoreAsset::AssetMetaData *metaData) override;
    //  bool Store(CoreAsset::Asset *asset, CoreAsset::AssetMetaData *metaData, const std::string &filePath) override;

    // 텍스처자체데이터 저장
    //	C:/File/File2/asset.texture.myTest.raw가 같은이름을 넘길것이다( 이름은 논리적주소에서 구분자를 . 으로 변환해서
    // 만든것)
    //  bool StoreTextureRaw(const std::string &texRawPath, const GRM::TextureDesc &textureDesc);

  private:
  private:
    TextureStorer();
};

} // namespace CoreAsset
