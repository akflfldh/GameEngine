#pragma once

#include <CoreAsset/AssetType.h>
#include <filesystem>
#include <stdint.h>
#include <string>
#include <unordered_map>

struct PakHeader
{
    // 순서 중요 (고정)
    char mMagic[4]; // "QPAK"
    uint32_t mVersion;
    uint32_t mEntryCount;
    uint64_t mTableOffset;
    uint64_t mRegistryPathStringTableOffset;
    uint64_t mDataOffset;
};

enum class EPakPayloadType : uint8_t
{
    eSerializedAsset,
    eRawData
};

struct PakEntry
{
    // 순서중요(고정)
    CoreAsset::AssetID mAssetID;
    uint32_t mAssetType;
    EPakPayloadType mPayloadType;
    uint32_t mFlags;
    uint64_t mRegistryPathOffset;
    // uint32_t mRegistryPathLength;
    uint64_t mDataOffset;
    uint64_t mStoredSize;
    uint64_t mOriginalSize;
};

using PakEntryTable = std::unordered_map<CoreAsset::AssetID, PakEntry>;
using PakEntryRegistryPathTable = std::unordered_map<CoreAsset::AssetID, std::string>;

struct PakWriteAssetRecord
{
    CoreAsset::AssetID mAssetID = NoneAssetID;
    CoreAsset::EAssetType mAssetType = CoreAsset::EAssetType::eUnknown;

    // 런타임 Registry에 등록할 전체 논리 경로
    std::string mRegistryPath;

    // .asset 또는 .map 파일의 물리적 경로
    std::filesystem::path mSerializedAssetPath;

    // 별도 Raw Data가 있는 경우의 물리적 경로
    std::filesystem::path mRawDataPath;

    uint32_t mFlags = 0;
};