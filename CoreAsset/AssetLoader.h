#pragma once

#include "CoreAsset/CoreAssetDLLMacro.h"
#include "CoreAsset/SerializedAssetType.h"
#include <memory>
namespace QuadRW
{
class BinaryReader;
}

namespace CoreAsset
{
class TextureLoader;
class MaterialLoader;
class AssetMetaData;
class CORE_ASSET_API AssetLoader
{
  public:
    static AssetLoader *GetInstance();
    ~AssetLoader();

    std::unique_ptr<SerializedAsset> LoadAsset(const std::string &filePath, AssetMetaData &oAssetMetaData);
    std::unique_ptr<SerializedAssetRawData> LoadAssetRawData(const std::string &filePath, EAssetType assetType);

  private:
    AssetLoader();

  private:
    // loader
    TextureLoader *mTextureLoader;
    MaterialLoader *mMaterialLoader;
};
} // namespace CoreAsset
