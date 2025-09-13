#pragma once

#include <CoreAsset/CoreAssetDLLMacro.h>
#include <CoreBase/FString.h>
#include <CoreBase/FVector.h>
#include <unordered_map>

namespace CoreAsset
{

class IAssetImporter;
struct IntermediateAsset;

class CORE_ASSET_API AssetImporterManager
{
  public:
    static AssetImporterManager *GetInstance();
    AssetImporterManager();
    ~AssetImporterManager();

    // register AssetImporter  ()  . key는 ? 확장자, 여러개의 key에대해서 동작한다면 여러번등록
    void RegisterAssetImporter(const FString &extension, IAssetImporter *importer);
    void ReleaseAssetImporter(const FString &extension);

    FVector<IntermediateAsset *> Import(const char *filePath);

  private:
    std::unordered_map<FString, IAssetImporter *> mAssetImporterTable;
};
} // namespace CoreAsset
