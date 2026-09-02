#include "CoreAsset/AssetCommon.h"

#include "CoreAsset/Asset.h"
#include <Utility.h>
#include <vector>

const char *CoreAsset::GetAssetFileExtension()
{
    return "asset";
}

const char *CoreAsset::GetAssetDotFileExtension()
{
    return ".asset";
}

const char *CoreAsset::GetAssetFileExtension(EAssetType assetType)
{

    switch (assetType)
    {
    case EAssetType::eStaticMesh:
    case EAssetType::eSkinningMesh:
    case EAssetType::eMaterial:
    case EAssetType::eTexture:
    case EAssetType::eAnimation:
    case EAssetType::eSound:
        return GetAssetFileExtension();
    case EAssetType::eMap:
        return GetMapFileExtension();
    }

    return GetAssetFileExtension();
}

const char *CoreAsset::GetMapFileExtension()
{
    return "map";
}

const char *CoreAsset::GetAssetRawFileExtension()
{
    return "raw";
}

std::string CoreAsset::GetAssetFileName(CoreAsset::Asset *asset)
{
    if (asset == nullptr)
        return "";
    std::string assetName = asset->GetName().c_str();
    std::string fileName = assetName + "." + GetAssetFileExtension();
    return fileName;
}

std::string CoreAsset::GetAssetRawFileName(const std::string &logicalPath)
{

    std::vector<std::string> logicalPathTokenVector = CoreUtility::Utility::Split(logicalPath, '/');

    std::string rawFileName;
    for (const auto &folderToken : logicalPathTokenVector)
    {
        rawFileName += folderToken + ".";
    }

    rawFileName += GetAssetRawFileExtension();

    return rawFileName;
}
