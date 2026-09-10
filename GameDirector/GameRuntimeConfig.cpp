#include "GameRuntimeConfig.h"

Quad::GameRuntimeConfig::GameRuntimeConfig(const std::filesystem::path &executableDirectory)
{

    mExecutableDirectory = executableDirectory;
    mGameRootDirectory = executableDirectory;
}

Quad::GameRuntimeConfig::~GameRuntimeConfig() {}

const std::filesystem::path &Quad::GameRuntimeConfig::GetExecutableDirectory() const
{
    return mExecutableDirectory;
}

const std::filesystem::path &Quad::GameRuntimeConfig::GetGameRootDirectory() const
{
    return mGameRootDirectory;
    // TODO: 여기에 return 문을 삽입합니다.
}

std::filesystem::path Quad::GameRuntimeConfig::GetManifestPath() const
{
    return mGameRootDirectory / "GameBuildManifest.bin";
}

std::filesystem::path Quad::GameRuntimeConfig::GetAssetDirectory() const
{
    return mGameRootDirectory / "Asset";
}

std::filesystem::path Quad::GameRuntimeConfig::GetRawAssetDirectory() const
{
    return mGameRootDirectory / "RawAsset";
}

std::filesystem::path Quad::GameRuntimeConfig::GetEngineAssetDirectory() const
{
    return mGameRootDirectory / "Engine";
}
