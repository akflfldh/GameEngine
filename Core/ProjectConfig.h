#pragma once

#include "Core/CoreDllExport.h"
#include <filesystem>
#include <string>
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

  private:
    std::filesystem::path mProjectPath;
    std::filesystem::path mProjectConfigFilePath;
    std::string mProjectName;

  private:
    ProjectConfig();
};

} // namespace Quad
