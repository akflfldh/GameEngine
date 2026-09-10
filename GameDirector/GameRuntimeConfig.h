#pragma once
#include <filesystem>
namespace Quad
{

class GameRuntimeConfig
{
  public:
    explicit GameRuntimeConfig(const std::filesystem::path &executableDirectory);
    ~GameRuntimeConfig();

    const std::filesystem::path &GetExecutableDirectory() const;
    const std::filesystem::path &GetGameRootDirectory() const;

    std::filesystem::path GetManifestPath() const;
    std::filesystem::path GetAssetDirectory() const;
    std::filesystem::path GetRawAssetDirectory() const;
    std::filesystem::path GetEngineAssetDirectory() const;

  private:
    std::filesystem::path mExecutableDirectory;
    std::filesystem::path mGameRootDirectory;
};

} // namespace Quad