#include "CoreAsset/AssetCommon.h"

#include "CoreAsset/Asset.h"
#include <Utility.h>
#include <vector>

const char *CoreAsset::GetAssetFileExtension()
{
    return "asset";
}

const char *CoreAsset::GetAssetRawFileExtension()
{
    return "raw";
}

std::string CoreAsset::GetAssetFileName(CoreAsset::Asset *asset)
{
    if (asset == nullptr)
        return "";

    std::string fileName = asset->GetName() + "." + GetAssetFileExtension();
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
