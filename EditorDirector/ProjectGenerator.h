#pragma once

#include <CoreBase/PrograssContext.h>
#include <filesystem>
#include <string>
// 멀티스레드

class ProjectGenerator
{
  public:
    static ProjectGenerator *GetInstance();
    ProjectGenerator();
    ~ProjectGenerator();

    static bool GenerateUserProject(const std::string &projectName, const std::string &targetPath,
                                    const std::string &enginePath);

    static bool GenerateObjectCXXFile(const std::string &parentClassName, const std::string &className,
                                      const std::filesystem::path &targetPath, bool isCoreClass = false);

    static bool ReBuildCMake(const std::filesystem::path &targetPath, const std::string &projectName);

  private:
    static bool GenerateCmakeLists(const std::string &projectName, const std::string &targetPath,
                                   const std::string &enginePath);
    static bool GenerateMainCppFile(const std::string &projectName, const std::string &targetPath,
                                    const std::string &enginePath);

    static bool GenerateDirectories(const std::string &projectName, const std::string &targetPath,
                                    const std::string &enginePath);
};
