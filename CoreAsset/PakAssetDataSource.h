#pragma once

#include <CoreAsset/IAssetDataSource.h>
#include <CoreAsset/PakReader.h>
#include <filesystem>
namespace CoreAsset
{

struct AssetDataRecord
{
    CoreAsset::AssetID mAssetID;
    std::string mRegistryPath;
};

class CORE_ASSET_API PakAssetDataSource : public IAssetDataSource
{
  public:
    PakAssetDataSource();
    virtual ~PakAssetDataSource() = default;

    bool Initialize(const std::filesystem::path &pakPath);

    void GetAssetRecordList(std::vector<AssetDataRecord> &oRecords) const;

    virtual bool ReadSerializedAssetData(const AssetSerializedDataRequest &request,
                                         std::vector<uint8_t> &oBuffer) const override;
    virtual bool ReadRawData(const AssetRawDataRequest &request, std::vector<uint8_t> &oBuffer) const override;

  private:
    PakReader mPakReader;
};

} // namespace CoreAsset