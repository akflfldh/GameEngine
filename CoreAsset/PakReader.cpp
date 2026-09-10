#include "PakReader.h"
#include <BinaryReaderWriter/BinaryReader.h>
#include <vector>

// PakReader *PakReader::GetInstance()
//{
//
//     static PakReader instance;
//     return &instance;
// }
PakReader::PakReader() {}

PakReader::~PakReader() {}

bool PakReader::LoadPakFile(const std::filesystem::path &path)
{

    if (!std::filesystem::is_regular_file(path))
        return false;

    QuadRW::BinaryReader reader;
    bool bRead = reader.StartRead(path);
    if (!bRead)
    {
        return false;
    }

    PakHeader pakHeader;
    for (int i = 0; i < 4; ++i)
    {
        bRead = reader.Read<char>(pakHeader.mMagic[i]);
        if (!bRead)
        {
            return false;
        }
    }

    // Check mMagic
    char magic[4] = {'Q', 'P', 'A', 'K'};
    for (int i = 0; i < 4; ++i)
    {
        if (pakHeader.mMagic[i] != magic[i])
            return false;
    }

    bRead = reader.Read<uint32_t>(pakHeader.mVersion);
    if (!bRead)
    {
        return false;
    }

    reader.Read<uint32_t>(pakHeader.mEntryCount);
    reader.Read<uint64_t>(pakHeader.mTableOffset);
    reader.Read<uint64_t>(pakHeader.mRegistryPathStringTableOffset);
    reader.Read<uint64_t>(pakHeader.mDataOffset);

    reader.SetReadPointer(pakHeader.mTableOffset);

    PakEntryTable entrySerializedAssetTable;
    PakEntryTable entryRawTable;

    // table
    for (uint32_t i = 0; i < pakHeader.mEntryCount; ++i)
    {
        PakEntry entry;

        //    // 순서중요(고정)
        // CoreAsset::AssetID mAssetID;
        // uint32_t mAssetType;
        // EPakPayloadType mPayloadType;
        // uint32_t mFlags;
        // uint64_t mRegistryPathOffset;
        // uint32_t mRegistryPathLength;
        // uint64_t mDataOffset;
        // uint64_t mStoredSize;
        // uint64_t mOriginalSize;

        reader.Read<CoreAsset::AssetID>(entry.mAssetID);
        reader.Read<uint32_t>(entry.mAssetType);

        uint8_t payloadType;
        reader.Read<uint8_t>(payloadType);
        entry.mPayloadType = static_cast<EPakPayloadType>(payloadType);

        reader.Read<uint32_t>(entry.mFlags);
        reader.Read<uint64_t>(entry.mRegistryPathOffset);
        // reader.Read<uint32_t>(entry.mRegistryPathLength);
        reader.Read<uint64_t>(entry.mDataOffset);
        reader.Read<uint64_t>(entry.mStoredSize);
        reader.Read<uint64_t>(entry.mOriginalSize);

        if (entry.mPayloadType == EPakPayloadType::eSerializedAsset)
        {
            entrySerializedAssetTable.insert({entry.mAssetID, entry});
        }
        else
        {
            entryRawTable.insert({entry.mAssetID, entry});
        }
    }

    // string table 구축
    PakEntryRegistryPathTable entryRegistryPathTable;
    //    reader.SetReadPointer(pakHeader.mRegistryPathStringTableOffset);

    for (const auto &entry : entrySerializedAssetTable)
    {
        uint64_t offset = entry.second.mRegistryPathOffset;
        // uint32_t length = entry.second.mRegistryPathLength;

        reader.SetReadPointer(offset);
        std::string registryPath;
        reader.Read(registryPath);

        entryRegistryPathTable.insert({entry.first, registryPath});
    }

    mFilePath = path;
    mEntrySerializedAssetDataTable = std::move(entrySerializedAssetTable);
    mEntryRawDataTable = std::move(entryRawTable);
    mEntryRegistryPathTable = std::move(entryRegistryPathTable);

    return true;
}

bool PakReader::ReadRawData(CoreAsset::AssetID assetID, std::vector<uint8_t> &oBuffer) const
{
    // 테이블 확인

    auto it = mEntryRawDataTable.find(assetID);
    if (it == mEntryRawDataTable.end())
        return false;

    uint64_t offset = it->second.mDataOffset;
    uint64_t size = it->second.mStoredSize;

    QuadRW::BinaryReader reader;
    bool bRead = reader.StartRead(mFilePath, offset, size);
    if (!bRead)
    {
        return false;
    }

    oBuffer.resize(size);
    return reader.ReadRaw(oBuffer.data(), size);
}

bool PakReader::ReadSerializedAssetData(CoreAsset::AssetID assetID, std::vector<uint8_t> &oBuffer) const
{
    // 테이블 확인

    auto it = mEntrySerializedAssetDataTable.find(assetID);
    if (it == mEntrySerializedAssetDataTable.end())
        return false;

    uint64_t offset = it->second.mDataOffset;
    uint64_t size = it->second.mStoredSize;

    QuadRW::BinaryReader reader;
    bool bRead = reader.StartRead(mFilePath, offset, size);
    if (!bRead)
    {
        return false;
    }

    oBuffer.resize(size);
    return reader.ReadRaw(oBuffer.data(), size);
}

bool PakReader::HasRawData(CoreAsset::AssetID assetID) const
{

    return mEntryRawDataTable.find(assetID) == mEntryRawDataTable.end() ? false : true;
}

std::string PakReader::GetAssetRegistryPath(CoreAsset::AssetID assetID) const
{
    auto it = mEntryRegistryPathTable.find(assetID);
    return it != mEntryRegistryPathTable.end() ? it->second : "";
}

void PakReader::GetSerializedAssetIDList(std::vector<CoreAsset::AssetID> &oAssetIDList) const
{

    for (auto it = mEntrySerializedAssetDataTable.begin(); it != mEntrySerializedAssetDataTable.end(); ++it)
    {
        oAssetIDList.push_back(it->first);
    }
}
