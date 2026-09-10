#pragma once

#include <Core/CoreDllExport.h>
#include <CoreAsset/AssetType.h>
#include <filesystem>
#include <stdint.h>
#include <string>

namespace Core
{

class CORE_API_LIB GameBuildManifest
{
  public:
    uint32_t mVersion = 1;
    std::string mGameName;
    std::string mGameModuleRelativePath; // dll path
    CoreAsset::AssetID mStartupMapAssetID = NoneAssetID;
};

CORE_API_LIB Arch &operator<<(Arch &arch, GameBuildManifest &manifest);

CORE_API_LIB bool ReadGameBuildManifest(const std::filesystem::path &filePath, GameBuildManifest &outManifest);

CORE_API_LIB bool WriteGameBuildManifest(const std::filesystem::path &filePath, const GameBuildManifest &manifest);

} // namespace Core