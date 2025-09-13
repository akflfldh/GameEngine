#pragma once

#include "Core/CoreDllExport.h"
#include <string>
namespace Quad
{
class CORE_API_LIB ProjectConfig
{
  public:
    static ProjectConfig *GetInstance();
    ~ProjectConfig();

    void SetProjectPath(const std::string &path);
    const std::string &GetProjectPath() const;
    std::string GetProjectRawAssetPath() const;

    void Load();
    void Save();

  private:
    std::string mProjectPath;
    std::string mProjectConfigFile;

  private:
    ProjectConfig();
};

} // namespace Quad
