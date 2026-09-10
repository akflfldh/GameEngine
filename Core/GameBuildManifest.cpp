#include "GameBuildManifest.h"
#include <CoreBase/BinaryArch.h>
Arch &Core::operator<<(Arch &arch, GameBuildManifest &manifest)
{

    arch << manifest.mVersion;
    arch << manifest.mGameName;
    arch << manifest.mGameModuleRelativePath;
    arch << manifest.mStartupMapAssetID;
    return arch;

    // TODO: 여기에 return 문을 삽입합니다.
}

bool Core::ReadGameBuildManifest(const std::filesystem::path &filePath, GameBuildManifest &outManifest)
{
    BinaryArch arch(true);

    arch.SetFile(filePath);
    arch.Start();

    arch << outManifest;

    arch.End();

    // 개선점 -  에러발생 여부

    return true;
}

bool Core::WriteGameBuildManifest(const std::filesystem::path &filePath, const GameBuildManifest &manifest)
{
    BinaryArch arch(false);

    arch.SetFile(filePath);
    arch.Start();

    GameBuildManifest tempManifest = manifest;

    arch << tempManifest;

    arch.End();

    // 개선점 -  에러발생 여부

    return true;
}
