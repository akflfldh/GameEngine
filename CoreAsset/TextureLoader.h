#pragma once

#include "CoreAsset/CoreAssetDLLMacro.h"
#include <CoreAsset/AssetLoader.h>
#include <memory>
#include <string>
namespace CoreAsset
{

class AssetMetaData;
class Asset;

class CORE_ASSET_API TextureLoader : public AssetLoader
{
  public:
    static TextureLoader *GetInstance();
    ~TextureLoader();
    virtual bool LoadAssetFile(Arch &arch, AssetFactoryManager *assetFactoryManager, Asset *&oAsset,
                               std::unique_ptr<AssetMetaData> &oAssetMetaDataPtr) override;
    virtual bool LoadAssetRawFile(Arch &arch, Asset *asse) override;

    /*virtual std::unique_ptr<SerializedAsset> LoadAssetFile(const std::string &filePath,
                                                           CoreAsset::AssetMetaData &oMetaData) override;
    virtual std::unique_ptr<AssetMetaData> LoadAssetMetaDataFile(QuadRW::BinaryReader &binaryReader) override;

    virtual std::unique_ptr<SerializedAssetRawData> LoadRawFile(const std::string &filePath) override;*/

  protected:
  private:
    TextureLoader();
};

} // namespace CoreAsset
