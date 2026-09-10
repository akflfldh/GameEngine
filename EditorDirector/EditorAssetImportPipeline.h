#pragma once

#include <CoreAsset/AssetType.h>
#include <filesystem>
#include <string>

struct AssetImportStoreRequest
{
    std::filesystem::path mSourcePath;

    CoreAsset::AssetID mRequestedAssetID = NoneAssetID;
    std::string mRequestedAssetName;
    std::string mTargetRegistryPath;

    std::filesystem::path mSerializedOutputPath;
    std::filesystem::path mRawOutputPath;

    bool mEngineAsset = false;
};

struct AssetImportStoreResult
{
    bool mSuccess = false;
    // PakWriteAssetRecord mPakRecord;
};

class EditorAssetImportPipeline
{
  public:
    static EditorAssetImportPipeline *GetInstance(); // asset system들이 초기화된이후 사용

    AssetImportStoreResult ImportAndStore(const AssetImportStoreRequest &request);
};
