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

    // targetPath는 Source 디렉터리다. 기존 사용자 파일 쌍은 보존하며, 한 파일만 있으면 생성하지 않고 실패한다.
    static bool GenerateUserGameInstanceCXXFile(const std::filesystem::path &targetPath);

    static bool ReBuildCMake(const std::filesystem::path &targetPath, const std::string &projectName);

  private:
    static bool GenerateCmakeLists(const std::string &projectName, const std::string &targetPath,
                                   const std::string &enginePath);
    static bool GenerateMainCppFile(const std::string &projectName, const std::string &targetPath,
                                    const std::string &enginePath);

    static bool GenerateDirectories(const std::string &projectName, const std::string &targetPath,
                                    const std::string &enginePath);
};
