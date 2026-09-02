#pragma once

#include <CoreAsset/CoreAssetDLLMacro.h>
#include <CoreAsset/IntermediateAsset.h>
#include <CoreBase/FString.h>
#include <filesystem>
#include <memory>
#include <unordered_map>
#include <vector>

namespace CoreAsset
{

class IAssetImporter;
struct IntermediateAsset;
struct ImportExecutionContext;

class CORE_ASSET_API AssetImporterManager
{
  public:
    static AssetImporterManager *GetInstance();
    AssetImporterManager();
    ~AssetImporterManager();

    // register AssetImporter  ()  . key는 ? 확장자, 여러개의 key에대해서 동작한다면 여러번등록
    void RegisterAssetImporter(const FString &extension, IAssetImporter *importer);
    void ReleaseAssetImporter(const FString &extension);

    ImportPackage Import(const std::filesystem::path &filePath, const ImportExecutionContext &executionContext);

  private:
    std::unordered_map<FString, IAssetImporter *> mAssetImporterTable;
};
} // namespace CoreAsset
