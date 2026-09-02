#pragma once

#include "CoreAsset/CoreAssetDLLMacro.h"
#include <CoreAsset/AssetType.h>
#include <memory>
#include <unordered_map>
namespace QuadRW
{
class BinaryReader;
}

class Arch;
namespace CoreAsset
{
class TextureLoader;
class MaterialLoader;
struct AssetMetaData;
class AssetFactoryManager;
class Asset;
class CORE_ASSET_API AssetLoader
{
  public:
    AssetLoader();
    virtual ~AssetLoader();

    virtual bool LoadAssetFile(EAssetType assetType, Arch &arch, AssetFactoryManager *assetFactoryManage,
                               Asset *&oAsset, std::unique_ptr<AssetMetaData> &oAssetMetaDataPtr) = 0;

    virtual bool LoadAssetRawFile(Arch &arch, Asset *asset) = 0;

    // virtual std::unique_ptr<SerializedAsset> LoadAssetFile(const std::string &filePath,
    //   CoreAsset::AssetMetaData &oMetaData) = 0;

    // virtual std::unique_ptr<AssetMetaData> LoadAssetMetaDataFile(QuadRW::BinaryReader &binaryReader) = 0;

    // virtual std::unique_ptr<SerializedAssetRawData> LoadRawFile(const std::string &filePath) = 0;

  protected:
    AssetFactoryManager *mAssetFactoryManager;

  private:
};
} // namespace CoreAsset
