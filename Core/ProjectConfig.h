#pragma once

#include "Core/CoreDllExport.h"
#include <CoreAsset/AssetType.h>
#include <filesystem>
#include <string>

class Arch;

namespace Quad
{
class CORE_API_LIB ProjectConfig
{
  public:
    static ProjectConfig *GetInstance();
    ~ProjectConfig();

    void SetProjectPath(const std::filesystem::path &path);
    void SetProjectName(const std::string &name);
    const std::string &GetProjectName() const;
    const std::filesystem::path &GetProjectPath() const;
    std::filesystem::path GetProjectRawAssetPath() const;

    void Load();
    void Save();
    void Serialize(Arch &arch);

    void SetStartMapID(CoreAsset::AssetID id);
    CoreAsset::AssetID GetStartMapID() const;

  private:
    std::filesystem::path mProjectPath;
    std::filesystem::path mProjectConfigFilePath;
    std::string mProjectName;
    CoreAsset::AssetID mStartMapID = NoneAssetID;

  private:
    ProjectConfig();
};

} // namespace Quad
