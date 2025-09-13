#pragma once

#include <CoreAsset/AssetMetaDataType.h>
#include <CoreAsset/AssetPtr.h>
#include <CoreAsset/AssetType.h>
#include <CoreAsset/CoreAssetDLLMacro.h>
#include <memory>
#include <unordered_map>
namespace CoreAsset
{
class AssetLoader;
class AssetStorer;
class Asset;
class AssetFactoryManager;

class CORE_ASSET_API AssetIOManager
{
  public:
    static AssetIOManager *GetInstance();

    AssetIOManager();
    ~AssetIOManager();

    void RegisterAssetLoader(EAssetType assetType, AssetLoader *assetLoader);
    void RegisterAssetStorer(EAssetType assetType, AssetStorer *assetStorer);
    // load
    // std::unique_ptr<SerializedAsset> LoadAsset(const std::string &filePath, AssetMetaData &oAssetMetaData);

    // 빈 Asset을 생성한다.
    bool LoadAssetFromMetaData(const std::string &filePath, AssetFactoryManager *assetFactoryManager, Asset *&oAsset,
                               std::unique_ptr<AssetMetaData> &oAssetMetaDataPtr);

    // 실제 빈asset의 내부데이터를 로드하고,직렬화하여 채운다.
    bool LoadAssetRawData(Asset *asset, const std::string &path);

    // std::unique_ptr<SerializedAssetRawData> LoadAssetRawData(const std::string &filePath, EAssetType assetType);
    // store
    bool StoreAsset(CoreAsset::Asset *asset, const std::string &filePath, AssetMetaData *assetMetaData);
    bool StoreAssetRawData(CoreAsset::Asset *asset, const std::string &filePath, AssetMetaData *assetMetaData);

  private:
    // asset loader table
    std::unordered_map<EAssetType, AssetLoader *> mAssetLoaderTable;

    // asset storer table
    std::unordered_map<EAssetType, AssetStorer *> mAssetStorerTable;
};

} // namespace CoreAsset