#pragma once

#include <CoreAsset/AssetMetaDataType.h>
#include <CoreAsset/SerializedAssetType.h>
#include <memory>
#include <unordered_map>

namespace Quad
{

struct SerializedAssetContainerContext
{

    std::unique_ptr<CoreAsset::SerializedAsset> mSerializedAsset;
    std::unique_ptr<CoreAsset::SerializedAssetRawData> mSerializedRawAssetData;
    CoreAsset::AssetMetaData assetMetaData;
    std::string mParentFolderPath;
};

class SerializedAssetTContainer
{
  public:
    static SerializedAssetTContainer *GetInstance();
    ~SerializedAssetTContainer();

    void Register(std::unique_ptr<CoreAsset::SerializedAsset> serializedAsset,
                  std::unique_ptr<CoreAsset::SerializedAssetRawData> serializedRawAssetData,
                  const CoreAsset::AssetMetaData &assetMetaDataconst, std::string &parentFolderPath);

    void Register(std::unique_ptr<CoreAsset::SerializedAsset> serializedAsset,
                  std::unique_ptr<CoreAsset::SerializedAssetRawData> serializedRawAssetData,
                  const CoreAsset::AssetMetaData &assetMetaDataconst, std::string &&parentFolderPath);

    const std::vector<SerializedAssetContainerContext> &GetSerializedAssetContextVector(
        CoreAsset::EAssetType assetType);

    // 더이상 필요없다면 내부테이블을 비워서 메모리를절약하자
    void Clear();

  private:
    SerializedAssetTContainer();
    std::unordered_map<CoreAsset::EAssetType, std::vector<SerializedAssetContainerContext>>
        mSerializedAssetContextTable;
};

} // namespace Quad
