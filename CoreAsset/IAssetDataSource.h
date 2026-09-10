#pragma once

#include <CoreAsset/AssetMetaDataType.h>
#include <CoreAsset/AssetType.h>
#include <CoreAsset/CoreAssetDLLMacro.h>
#include <filesystem>
#include <stdint.h>
#include <vector>

namespace CoreAsset
{

struct AssetRawDataRequest
{
    CoreAsset::AssetID mAssetID = NoneAssetID; // pak버전
    EAssetDomain mDomain = EAssetDomain::eProject;
    std::filesystem::path mRawDataRelativePath; // file path버전
};

struct AssetSerializedDataRequest
{
    CoreAsset::AssetID mAssetID = NoneAssetID; // pak버전
    std::filesystem::path mPath;               // file path버전
};

class CORE_ASSET_API IAssetDataSource
{
  public:
    IAssetDataSource() = default;
    virtual ~IAssetDataSource() = default;

    virtual bool ReadRawData(const AssetRawDataRequest &request, std::vector<uint8_t> &oBuffer) const = 0;
    virtual bool ReadSerializedAssetData(const AssetSerializedDataRequest &request,
                                         std::vector<uint8_t> &oBuffer) const = 0;

  private:
};
} // namespace CoreAsset