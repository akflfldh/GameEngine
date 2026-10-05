#include "ProjectGenerator.h"
#include <Core/CollisionChannelSystem.h>
#include <CoreBase/BinaryArch.h>
#include <EditorDirector/CMakeProjectBuilder.h>
#include <ProjectConfig.h>
#include <filesystem>
#include <fstream>
#include <system_error>

ProjectGenerator *ProjectGenerator::GetInstance()
{
    static ProjectGenerator instance;

    return &instance;
}

ProjectGenerator::ProjectGenerator() {}

ProjectGenerator::~ProjectGenerator() {}

bool ProjectGenerator::GenerateUserProject(const std::string &projectName, const std::string &targetPath,
                                           const std::string &enginePath)
{
    std::string safeEnginePath = enginePath;
    std::replace(safeEnginePath.begin(), safeEnginePath.end(), '\\', '/');

    std::string safeTargetPath = targetPath;
    std::replace(safeTargetPath.begin(), safeTargetPath.end(), '\\', '/');

    bool ret = GenerateDirectories(projectName, targetPath, enginePath);
    if (!ret)
    {
        return false;
    }

    // Create main.cpp
    ret = GenerateMainCppFile(projectName, safeTargetPath, safeEnginePath);
    if (!ret)
    {
        return false;
    }

    // 최초 CMake 구성에서 사용자 소스와 리플렉션 대상이 함께 수집되도록 먼저 생성한다.
    ret = GenerateUserGameInstanceCXXFile(std::filesystem::path(safeTargetPath) / "Source");
    if (!ret)
    {
        return false;
    }

    // create ConfigFile
    Quad::ProjectConfig *projectConfig = Quad::ProjectConfig::GetInstance();
    projectConfig->SetProjectPath(safeTargetPath);
    projectConfig->Save();

    // Create ProjectCollisionCfg
    BinaryArch arch(false);
    arch.SetFile(std::filesystem::path(safeTargetPath) / "ProjectCollision.cfg");
    arch.Start();

    auto collisionChannelSystem = Core::CollisionChannelSystem::GetInstance();
    collisionChannelSystem->InitDefault();
    collisionChannelSystem->Serialize(arch);

    arch.End();

    // CreateCmakeList
    ret = GenerateCmakeLists(projectName, safeTargetPath, safeEnginePath);
    if (!ret)
    {

        return false;
    }

    return true;
}

bool ProjectGenerator::GenerateObjectCXXFile(const std::string &parentClassName, const std::string &className,
                                             const std::filesystem::path &targetPath, bool isCoreClass)
{
    // prograssContext.Report("클래스 파일생성 중");

    std::string header = R"(
#include"@Path@@parentClassName@.h"

#include"@className@.generated.h"


class REFLECT_CLASS() @className@ : public @parentClassName@
{
    GENERATED_BODY(@className@)

    public:
        @className@();
        
        
    protected:
        virtual void OnBegin() override;    
        virtual void Tick(float deltaTime) override;
    private:



};

)";

    std::string cpp = R"(
#include"@className@.h"


@className@::@className@()
{

};

void @className@::OnBegin()
{
    @parentClassName@::OnBegin();
   


}


void @className@::Tick(float deltaTime)
{
    @parentClassName@::Tick(deltaTime);

}
)";

    size_t pos = 0;
    while ((pos = header.find("@parentClassName@", pos)) != std::string::npos)
    {
        header.replace(pos, 17, parentClassName);
    }

    pos = 0;
    while ((pos = header.find("@className@", pos)) != std::string::npos)
    {
        header.replace(pos, 11, className);
    }

    std::string path = "";
    if (isCoreClass)
    {
        path = "Core/";
    }

    pos = 0;
    while ((pos = header.find("@Path@", pos)) != std::string::npos)
    {
        header.replace(pos, 6, path);
    }

    pos = 0;
    while ((pos = cpp.find("@className@", pos)) != std::string::npos)
    {
        cpp.replace(pos, 11, className);
    }

    pos = 0;
    while ((pos = cpp.find("@parentClassName@", pos)) != std::string::npos)
    {
        cpp.replace(pos, 17, parentClassName);
    }

    std::filesystem::path headerPath = targetPath / (className + ".h");
    std::ofstream fout;
    fout.open(headerPath);
    if (fout.is_open() == false)
    {
        return false;
    }

    fout << header;

    fout.close();

    std::filesystem::path cppPath = targetPath / (className + ".cpp");
    fout.open(cppPath);

    if (fout.is_open() == false)
    {
        return false;
    }

    fout << cpp;
    fout.close();

    return true;
}

bool ProjectGenerator::GenerateUserGameInstanceCXXFile(const std::filesystem::path &targetPath)
{
    const std::filesystem::path headerPath = targetPath / "UserGameInstance.h";
    const std::filesystem::path cppPath = targetPath / "UserGameInstance.cpp";

    // 프로젝트 생성 재시도 시 사용자가 작성한 코드는 덮어쓰지 않는다.
    // 한 파일만 남아 있으면 새 템플릿과 기존 구현이 섞일 수 있으므로 호출자에게 실패를 전달한다.
    std::error_code error;
    const bool headerExists = std::filesystem::exists(headerPath, error);
    if (error)
        return false;
    const bool cppExists = std::filesystem::exists(cppPath, error);
    if (error)
        return false;
    if (headerExists || cppExists)
    {
        if (!headerExists || !cppExists)
            return false;
        if (!std::filesystem::is_regular_file(headerPath, error) || error)
            return false;
        return std::filesystem::is_regular_file(cppPath, error) && !error;
    }

    const std::string header = R"(#pragma once

#include <Core/GameInstance.h>
#include <ReflectSystem/ReflectionMacro.h>

#include "UserGameInstance.generated.h"

// 사용자 게임의 플레이 세션 로직을 작성하는 진입점이다. 맵 전환과 무관한 게임 상태와 공통 UI를 준비한다.
// 실행 모드가 인스턴스의 수명을 관리하며, 월드·맵·공유 Canvas의 소유권은 갖지 않는다.
class REFLECT_CLASS() UserGameInstance : public GameInstance
{
    GENERATED_BODY(UserGameInstance)
  public:
    UserGameInstance();
    virtual ~UserGameInstance() override;

    virtual void Initialize() override;
    virtual void Start() override;
    virtual void Update(float deltaTime) override;
    virtual void EndUpdate() override;
    virtual void Shutdown() override;
};
)";

    const std::string cpp = R"(#include "UserGameInstance.h"

UserGameInstance::UserGameInstance() {}

UserGameInstance::~UserGameInstance() {}

void UserGameInstance::Initialize()
{
    GameInstance::Initialize();
}

void UserGameInstance::Start()
{
    GameInstance::Start();

    // 맵별 초기화가 아니라 플레이 세션 동안 유지할 UI와 상태의 준비 코드를 작성한다.
}

void UserGameInstance::Update(float deltaTime)
{
    GameInstance::Update(deltaTime);
}

void UserGameInstance::EndUpdate()
{
    GameInstance::EndUpdate();
}

void UserGameInstance::Shutdown()
{
    // 사용자 UI의 제거 요청과 콜백 해제는 이곳에서 수행한다. 기반 종료보다 먼저 정리하여
    // GameInstance가 Canvas 참조를 비우기 전에 세션에서 사용한 자원과 연결을 해제한다.

    GameInstance::Shutdown();
}
)";

    std::ofstream headerFile(headerPath);
    if (!headerFile.is_open())
        return false;
    // 사용자 프로젝트의 컴파일러 기본 코드 페이지와 무관하게 한국어 주석을 읽도록 UTF-8 BOM을 붙인다.
    headerFile << "\xEF\xBB\xBF" << header;
    headerFile.close();
    if (headerFile.fail())
        return false;

    std::ofstream cppFile(cppPath);
    if (!cppFile.is_open())
        return false;
    cppFile << "\xEF\xBB\xBF" << cpp;
    cppFile.close();
    return !cppFile.fail();
}

bool ProjectGenerator::ReBuildCMake(const std::filesystem::path &targetPath, const std::string &projectName)
{

    // prograssContext.Report("Cmake빌드 진행중");

    // std::string targetPath;
    std::string buildCmd =
        "cd /d \"" + targetPath.string() + "\" && cmake -S . -B build -G \"Visual Studio 17 2022\" -A x64";
    int result = std::system(buildCmd.c_str());

    if (result == 0)
    {
        // 생성된 빌드파일 빌드수행

        // buildCmd = "cd /d \"" + targetPath + "\"  && cmake --build build --config Debug";
        // result = std::system(buildCmd.c_str());

        // result = std::filesystem::exists(targetPath + "/build/Debug/" + projectName + ".dll");
        // return result;
    }

    return true;
}

bool ProjectGenerator::GenerateCmakeLists(const std::string &projectName, const std::string &targetPath,
                                          const std::string &enginePath)
{
    std::ifstream fin(enginePath + "/Template/CmakeTemplate.txt");

    fin.seekg(0, std::ios_base::end);
    size_t size = fin.tellg();
    fin.seekg(0);
    std::string str;
    str.resize(size, ' ');

    fin.read(str.data(), str.size());
    fin.close();

    size_t pos;
    while ((pos = str.find("@USER_PROJECT_NAME@")) != std::string::npos)
    {
        str.replace(pos, 19, projectName);
    }

    pos = 0;
    while ((pos = str.find("@ENGINE_PATH@")) != std::string::npos)
    {
        str.replace(pos, 13, enginePath);
    }

    std::ofstream fout(targetPath + "/CmakeLists.txt");
    fout << str;
    fout.close();

    Quad::CMakeConfigureDesc cmakeConfigDesc;
    cmakeConfigDesc.mTargetPath = targetPath;
    cmakeConfigDesc.mSubDirectory = "build";
    int result = Quad::CMakeProjectBuilder::Configure(cmakeConfigDesc);

    // std::string buildCmd = "cd /d \"" + targetPath + "\" && cmake -S . -B build -G \"Visual Studio 17 2022\" -A x64";

    //// std::cout << "[ProjectGenerator] MSVC 솔루션 생성 중..." << std::endl;
    // int result = std::system(buildCmd.c_str());

    if (result)
    {
        Quad::CMakeBuildDesc cmakeBuildDesc;
        cmakeBuildDesc.mTargetPath = targetPath;
        cmakeBuildDesc.mSubDirectory = "build";
        cmakeBuildDesc.mConfig = Quad::EBuildConfig::eDebug;

        result = Quad::CMakeProjectBuilder::Build(cmakeBuildDesc);

        return result;
        //// 생성된 빌드파일 빌드수행

        // buildCmd = "cd /d \"" + targetPath + "\"  && cmake --build build --config Debug";
        // result = std::system(buildCmd.c_str());

        // result = std::filesystem::exists(targetPath + "/build/Debug/" + projectName + ".dll");
        // return result;
    }

    return false;
}

bool ProjectGenerator::GenerateMainCppFile(const std::string &projectName, const std::string &targetPath,
                                           const std::string &enginePath)
{

    std::string cppTemplate = R"(#include "Core/EngineModuleMacro.h"

// Quad 엔진 게임 모듈 진입점
IMPLEMENT_GAME_MODULE(@USER_PROJECT_NAME@);
)";

    size_t pos = 0;
    while ((pos = cppTemplate.find("@USER_PROJECT_NAME@")) != std::string::npos)
    {
        cppTemplate.replace(pos, 19, projectName);
    }

    std::string sourcePath = targetPath + "/Source";

    std::ofstream fout(sourcePath + "/Main.cpp");
    if (fout.is_open() == false)
    {
        return false;
    }

    fout << cppTemplate;

    return true;
}

bool ProjectGenerator::GenerateDirectories(const std::string &projectName, const std::string &targetPath,
                                           const std::string &enginePath)
{
    namespace fs = std::filesystem;
    bool ret = fs::create_directories(targetPath + "/Source");

    ret = fs::create_directories(targetPath + "/Asset");

    ret = fs::create_directories(targetPath + "/RawAsset");

    std::ofstream fout(targetPath + "/LogFile.txt");
    fout.close();

    return ret;
}
