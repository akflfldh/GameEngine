#include "ProjectGenerator.h"
#include <ProjectConfig.h>
#include <filesystem>
#include <fstream>
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

    // create ConfigFile

    Quad::ProjectConfig *projectConfig = Quad::ProjectConfig::GetInstance();
    projectConfig->SetProjectPath(safeTargetPath);
    projectConfig->Save();

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

    std::string buildCmd = "cd /d \"" + targetPath + "\" && cmake -S . -B build -G \"Visual Studio 17 2022\" -A x64";

    // std::cout << "[ProjectGenerator] MSVC 솔루션 생성 중..." << std::endl;
    int result = std::system(buildCmd.c_str());

    if (result == 0)
    {
        // 생성된 빌드파일 빌드수행

        buildCmd = "cd /d \"" + targetPath + "\"  && cmake --build build --config Debug";
        result = std::system(buildCmd.c_str());

        result = std::filesystem::exists(targetPath + "/build/Debug/" + projectName + ".dll");
        return result;
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
