#pragma once

#include <CoreAsset/IAssetDataSource.h>

namespace CoreAsset
{

class CORE_ASSET_API LooseFileAssetRawDataSource : public IAssetDataSource
{
  public:
    explicit LooseFileAssetRawDataSource(const std::filesystem::path &projectRawDatafileRootPath,
                                         const std::filesystem::path &engineRawDataFileRootPath
                                         /*   const std::filesystem::path &serializedAssetFileRootPath*/);

    virtual ~LooseFileAssetRawDataSource() override;

    virtual bool ReadSerializedAssetData(const AssetSerializedDataRequest &request,
                                         std::vector<uint8_t> &oBuffer) const override;
    bool ReadRawData(const AssetRawDataRequest &request, std::vector<uint8_t> &oBuffer) const override;

  private:
    std::filesystem::path mProjectRawDataRootPath;
    std::filesystem::path mEngineRawDataRootPath;
    // std::filesystem::path mSerializedDataRootPath;
};

} // namespace CoreAsset