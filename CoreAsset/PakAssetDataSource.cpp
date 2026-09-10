#include "PakAssetDataSource.h"

CoreAsset::PakAssetDataSource::PakAssetDataSource() {}

bool CoreAsset::PakAssetDataSource::Initialize(const std::filesystem::path &pakPath)
{

    return mPakReader.LoadPakFile(pakPath);
}

void CoreAsset::PakAssetDataSource::GetAssetRecordList(std::vector<AssetDataRecord> &oRecords) const
{

    oRecords.clear();
    std::vector<CoreAsset::AssetID> assetIDList;
    mPakReader.GetSerializedAssetIDList(assetIDList);

    for (size_t i = 0; i < assetIDList.size(); ++i)
    {
        AssetDataRecord record;

        record.mAssetID = assetIDList[i];
        record.mRegistryPath = mPakReader.GetAssetRegistryPath(assetIDList[i]);

        oRecords.push_back(record);
    }
}

bool CoreAsset::PakAssetDataSource::ReadSerializedAssetData(const AssetSerializedDataRequest &request,
                                                            std::vector<uint8_t> &oBuffer) const
{

    return mPakReader.ReadSerializedAssetData(request.mAssetID, oBuffer);
}

bool CoreAsset::PakAssetDataSource::ReadRawData(const AssetRawDataRequest &request, std::vector<uint8_t> &oBuffer) const
{

    return mPakReader.ReadRawData(request.mAssetID, oBuffer);
}
