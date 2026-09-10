#pragma once

#include <CoreAsset/AssetMetaDataType.h>
#include <CoreAsset/AssetPtr.h>
#include <CoreAsset/AssetType.h>
#include <CoreAsset/CoreAssetDLLMacro.h>
#include <filesystem>
#include <memory>
#include <stdint.h>
#include <unordered_map>

class Arch;

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
    AssetLoadResult LoadAssetFromMetaData(const std::filesystem::path &filePath,
                                          AssetFactoryManager *assetFactoryManager, Asset *&oAsset,
                                          std::unique_ptr<AssetMetaData> &oAssetMetaDataPtr,
                                          const AssetLoadExecutionContext &executionContext);

    AssetLoadResult LoadAssetFromMetaDataFromBuffer(const uint8_t *data, size_t size,
                                                    AssetFactoryManager *assetFactoryManager, Asset *&oAsset,
                                                    std::unique_ptr<AssetMetaData> &oAssetMetaDataPtr,
                                                    const AssetLoadExecutionContext &executionContext);

    // 실제 빈asset의 내부데이터를 로드하고,직렬화하여 채운다.
    bool LoadAssetRawData(Asset *asset, const std::filesystem::path &path);
    bool LoadAssetRawDataFromBuffer(Asset *asset, const uint8_t *data, size_t size);

    // std::unique_ptr<SerializedAssetRawData> LoadAssetRawData(const std::string &filePath, EAssetType assetType);
    // store
    bool StoreAsset(CoreAsset::Asset *asset, const std::filesystem::path &filePath, AssetMetaData *assetMetaData);
    bool StoreAssetRawData(CoreAsset::Asset *asset, const std::filesystem::path &filePath,
                           AssetMetaData *assetMetaData);

    AssetStorer *GetStorer(EAssetType type) const;
    AssetLoader *GetLoader(EAssetType type) const;

  private:
    AssetLoadResult LoadAssetFromMetaDataCommon(Arch &arch, AssetFactoryManager *assetFactoryManager, Asset *&oAsset,
                                                std::unique_ptr<AssetMetaData> &oAssetMetaDataPtr,
                                                const AssetLoadExecutionContext &executionContext);

    bool LoadAssetRawDataCommon(Asset *asset, Arch &binaryArch);

  private:
    // asset loader table
    std::unordered_map<EAssetType, AssetLoader *> mAssetLoaderTable;

    // asset storer table
    std::unordered_map<EAssetType, AssetStorer *> mAssetStorerTable;

    static uint32_t mCurrentEngineVerison;
};

} // namespace CoreAsset