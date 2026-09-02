#include "EditorProjectBrowserManager.h"
#include "TaskUIController.h"
#include <CoreBase/TextArch.h>
#include <EditorDirector/EditorConfig.h>
#include <EditorDirector/EditorDirector.h>
#include <EditorDirector/ProjectGenerator.h>
#include <EditorDirector/UIProjectBrowser.h>
#include <UiSystem/UICanvas.h>
#include <Utility/Utility.h>

EditorProjectBrowserManager *EditorProjectBrowserManager::GetInstance()
{
    static EditorProjectBrowserManager instance;
    return &instance;
}

EditorProjectBrowserManager::EditorProjectBrowserManager() : mUIProjectBrowser(nullptr) {}

EditorProjectBrowserManager::~EditorProjectBrowserManager() {}

void EditorProjectBrowserManager::Initialize(UI::UICanvas *uiCanvas)
{
    mUIProjectBrowser = uiCanvas->CreateUIElement<UIProjectBrowser>("UIProjectBrowser");
    mUIProjectBrowser->SetSize(1000, 1000);

    InitProjectList();
}

void EditorProjectBrowserManager::InitProjectList()
{
    std::filesystem::path editorPath = Quad::EditorConfig::GetInstance()->GetEditorRootPath();
    std::filesystem::path projectListFilePath = editorPath / "ProjectList.txt";
    std::string projectListFilePathStr = projectListFilePath.string();

    size_t projectNum = 0;
    if (!std::filesystem::exists(projectListFilePath))
    {
        TextArch writer(false);

        writer.SetFile(projectListFilePathStr.data());
        writer.Start();

        if (!writer.IsFail())
        {
            size_t projectCount = 0;
            writer << projectCount;
        }
        writer.End();
    }

    TextArch arch;

    arch.SetFile(projectListFilePathStr.data());
    arch.Start();

    std::vector<std::string> projectNameList;
    if (!arch.IsFail())
    {
        arch << projectNum;

        for (size_t i = 0; i < projectNum; ++i)
        {
            std::string projectName;
            std::string projectPath;

            arch << projectName << projectPath;

            mProjectPathTable.try_emplace(projectName, projectPath);
            projectNameList.push_back(projectName);
        }
    }
    arch.End();

    mUIProjectBrowser->SetProjectList(projectNameList);

    mUIProjectBrowser->mOnProjectGeneartionButtonDownCallbackSystem.Register(
        [this](const std::string &projectPath, const std::string &projectName)
        { CreateProject(projectPath, projectName); });

    mUIProjectBrowser->mOnSelectedProjectItemCallbackSystem.Register([this](const std::string &projectName)
                                                                     { OnSelectedProject(projectName); });
}

bool EditorProjectBrowserManager::VerifyProjectPath(const std::string &projectPath)
{
    return true;
}

void EditorProjectBrowserManager::SaveProjectListFile()
{
    const std::filesystem::path &editorPath = Quad::EditorConfig::GetInstance()->GetEditorRootPath();
    std::filesystem::path projectListFilePath = editorPath / "ProjectList.txt";

    TextArch arch(false);
    std::string projectListFilePathStr = projectListFilePath.string();
    arch.SetFile(projectListFilePathStr.c_str());

    arch.Start();

    size_t projectNum = mProjectPathTable.size();
    arch << projectNum;

    for (auto &projectElement : mProjectPathTable)
    {
        std::string projectName = projectElement.first;
        std::string projectPath = projectElement.second;

        arch << projectName << projectPath;
    }

    arch.End();
}

void EditorProjectBrowserManager::OnSelectedProject(const std::string &projectName)
{

    auto it = mProjectPathTable.find(projectName);
    if (it == mProjectPathTable.end())
        return;

    Quad::EditorDirector::GetInstance()->ShutDownWindow();

    LaunchNewEditor(it->first, it->second);
}

bool EditorProjectBrowserManager::LaunchNewEditor(const std::string &projectName, const std::string &projectPath)
{
    //  const std::filesystem::path &editorPath = Quad::EditorConfig::GetInstance()->GetEditorRootPath();
    const std::filesystem::path &executablePath = Quad::EditorConfig::GetInstance()->GetExecutableDirectory();

    //  editorPath = "/build/debug-ninja/bin/Debug";
    std::filesystem::path editorEXEPath = executablePath / "EditorDirector.exe";
    std::string cmdArguments = "1 " + projectName + " " + projectPath;
    // 새 에디터 실행
    // 생성된 프로젝트
    return CoreUtility::Utility::CreateNewProcess(editorEXEPath.string(), cmdArguments, "");
}

void EditorProjectBrowserManager::CreateProject(const std::string &projectPath, const std::string &projectName)
{

    const std::filesystem::path &enginePath = Quad::EditorConfig::GetInstance()->GetEditorRootPath();

    bool ret = true;

    if (projectName.empty() || (CoreUtility::Utility::IsAllAlpha(projectName) == false))
    {
        ret = false;
        mUIProjectBrowser->ShowMessageBox("프로젝트 생성실패\n프로젝트의 이름은 영문자만 가능합니다.");
        return;
    }

    ret = ProjectGenerator::GenerateUserProject(projectName, projectPath, enginePath.string());

    if (ret)
    {
        mProjectPathTable.try_emplace(projectName, projectPath);
        SaveProjectListFile();

        // 현재 에디터종료
        Quad::EditorDirector::GetInstance()->ShutDownWindow();

        ret = LaunchNewEditor(projectName, projectPath);
        return;
    }
    else
    {
        // 실패
        // 메세지박스 띄우기
        mUIProjectBrowser->ShowMessageBox("프로젝트 생성실패, 프로젝트 경로가 올바르지못합니다.");
    }

    /* using dllProc = void (*)();

     std::string projectPath = ProjectConfig::GetInstance()->GetProjectPath();
     std::string libPath = projectPath + "/build/Debug/TestProject.dll";
     HMODULE hModule = LoadLibraryA(libPath.c_str());
     if (hModule != nullptr)
     {
         dllProc fun;
         fun = (dllProc)GetProcAddress(hModule, "InitializeUserModule");
         fun();
     }*/
}
