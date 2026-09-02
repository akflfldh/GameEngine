#include "AssetImporterManager.h"
#include <CoreAsset/IAssetImporter.h>
#include <Utility/Utility.h>
#include <string >
CoreAsset::AssetImporterManager *CoreAsset::AssetImporterManager::GetInstance()
{

    static AssetImporterManager instance;
    return &instance;
}

CoreAsset::AssetImporterManager::AssetImporterManager() {}

CoreAsset::AssetImporterManager::~AssetImporterManager() {}

void CoreAsset::AssetImporterManager::RegisterAssetImporter(const FString &extension, IAssetImporter *importer)
{

    mAssetImporterTable[extension] = importer;
}

void CoreAsset::AssetImporterManager::ReleaseAssetImporter(const FString &extension)
{

    mAssetImporterTable.erase(extension);
}

CoreAsset::ImportPackage CoreAsset::AssetImporterManager::Import(const std::filesystem::path &filePath,
                                                                 const ImportExecutionContext &executionContext)
{
    // 리스트를반환하기위해서 FVector 를 만들자

    if (filePath == "")
        return {};

    std::string extension = filePath.extension().string();

    //    CoreUtility::Utility::GetExtensionFromPath(filePath);

    std::unordered_map<FString, IAssetImporter *>::const_iterator it = mAssetImporterTable.find(extension.c_str());
    if (it == mAssetImporterTable.cend())
    {
        return {};
    }

    IAssetImporter *importer = it->second;
    return importer->Import(filePath, this, executionContext);
}
