#include "CMakeProjectBuilder.h"
#include <cstdlib>
bool Quad::CMakeProjectBuilder::Configure(const CMakeConfigureDesc &desc)
{

    // std::string buildCmd =
    //   "cd /d \"" + desc.mTargetPath.string() + "\" && cmake -S . -B build -G \"Visual Studio 17 2022\" -A x64";

    std::filesystem::path buildPath = desc.mTargetPath / desc.mSubDirectory;
    //    "game-build";

    std::string buildCmd = " cmake -S \"" + desc.mTargetPath.string() + "\" -B \"" + buildPath.string() +
                           "\" -G \"Visual Studio 17 2022\" -A x64";

    // std::cout << "[ProjectGenerator] MSVC 솔루션 생성 중..." << std::endl;
    int result = std::system(buildCmd.c_str());

    return !result;
}

bool Quad::CMakeProjectBuilder::Build(const CMakeBuildDesc &desc)
{

    // 생성된 빌드파일 빌드수행
    std::filesystem::path buildPath = desc.mTargetPath / desc.mSubDirectory;

    std::string buildConfig;
    if (desc.mConfig == EBuildConfig::eDebug)
    {
        buildConfig = "Debug";
    }
    else
    {
        buildConfig = "Release";
    }

    //"game-build";

    std::string buildCmd = "cmake --build \"" + buildPath.string() + "\" --config " + buildConfig;
    int result = std::system(buildCmd.c_str());

    // result = std::filesystem::exists(buildPath / "Debug" / (projectName + ".dll"));

    return !result;
}
