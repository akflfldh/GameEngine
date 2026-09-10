#pragma once

#include <filesystem>

namespace Quad
{

struct CMakeConfigureDesc
{
    std::filesystem::path mTargetPath; // 최상위 CMakeLists 위치
    std::string mSubDirectory;         // configure 하위 폴더
};

enum class EBuildConfig
{
    eDebug = 0,
    eRelease
};

struct CMakeBuildDesc
{
    std::filesystem::path mTargetPath;           // 최상위 CMakeLists 위치
    std::string mSubDirectory;                   // build 하위 폴더
    EBuildConfig mConfig = EBuildConfig::eDebug; // config옵션
    //최종 빌드출력물위치는 mTargetPath/mSubDirectory/Debug or Release
};

class CMakeProjectBuilder
{
  public:
    static bool Configure(const CMakeConfigureDesc &desc);
    static bool Build(const CMakeBuildDesc &desc);

  private:
};

} // namespace Quad