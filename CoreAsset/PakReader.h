#pragma once

#include <CoreAsset/PakType.h>
#include <filesystem>
#include <vector>

class PakReader
{
  public:
    // static PakReader *GetInstance();

    PakReader();
    ~PakReader();

    /*
    성공하면 EntryTable이 구축된다.
    */
    bool LoadPakFile(const std::filesystem::path &path);

    // asset raw data
    bool ReadRawData(CoreAsset::AssetID assetID, std::vector<uint8_t> &oBuffer) const;

    // asset meta data
    bool ReadSerializedAssetData(CoreAsset::AssetID assetID, std::vector<uint8_t> &oBuffer) const;

    std::string GetAssetRegistryPath(CoreAsset::AssetID assetID) const;
    void GetSerializedAssetIDList(std::vector<CoreAsset::AssetID> &oAssetIDList) const;

    bool HasRawData(CoreAsset::AssetID assetID) const;

  private:
    PakEntryTable mEntrySerializedAssetDataTable;
    PakEntryTable mEntryRawDataTable;
    PakEntryRegistryPathTable mEntryRegistryPathTable;

    std::filesystem::path mFilePath;
};
