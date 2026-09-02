#include "EditorBuildConfig.h"
#include <Core/Application.h>
#include <Core/ProjectConfig.h>
#include <EditorDirector/EditorConfig.h>
#include <EditorDirector/EditorDirector.h>
#include <Logger/Logger.h>
#include <Utility/Utility.h>

#if __has_include("EditorDirector/Dev/LocalDevSetup.h")
#include "EditorDirector/Dev/LocalDevSetup.h"
#define LOCAL_DEV_SETUP
#endif

namespace
{

std::filesystem::path GetExecutableDirectory()
{

    std::wstring buffer(32768, L'\0');

#ifdef _WIN32

    DWORD length = GetModuleFileNameW(nullptr, buffer.data(), buffer.size());

    buffer.resize(length);
#endif

    return std::filesystem::path(buffer).parent_path();
}

std::filesystem::path ResolveEditorRoot()
{
#ifdef GAMEENGINE_DEVELOPMENT_BUILD

    return std::filesystem::path(GAMEENGINE_DEVELOPMENT_ROOT);

#else
    return GetExecutableDirectory();
#endif
}

void SetEditorConfig(LPWSTR *cmdLists, int cmdNum)
{
    Quad::EditorConfig *editorConfig = Quad::EditorConfig::GetInstance();

    editorConfig->SetEditorRootPath(ResolveEditorRoot());
    editorConfig->SetEditorExecutableDirectory(GetExecutableDirectory());

    if (wcscmp(cmdLists[0], L"1") == 0)
    {

        editorConfig->SetEditorMode(EEditorMode::eEditProject);
        Quad::ProjectConfig *projectConfig = Quad::ProjectConfig::GetInstance();
        std::string projectName = CoreUtility::Utility::ConvertToString(cmdLists[1], true);
        projectConfig->SetProjectName(projectName);

        std::filesystem::path projectFolderPath = CoreUtility::Utility::ConvertToString(cmdLists[2], true);
        projectConfig->SetProjectPath(projectFolderPath);
        QuadLog::Logger *logger = QuadLog::Logger::GetInstance();

        logger->SetLoggerFile((projectConfig->GetProjectPath() / "LogFile.txt").c_str());
    }
    else
    {
        editorConfig->SetEditorMode(EEditorMode::eProjectBrowser);
    }

    LocalFree(cmdLists);
}

} // namespace

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR pCmdLine, int nCmdShow)
{
    SetProcessDPIAware();

    // TODO 지금은 cmd를 코드로 넣어주자

    int cmdNum;

    LPWSTR *cmdLists = nullptr;

#ifdef LOCAL_DEV_SETUP
    cmdLists = LocalDevSetup(cmdNum);
#else
    cmdLists = CommandLineToArgvW(pCmdLine, &cmdNum);
#endif

    SetEditorConfig(cmdLists, cmdNum);

    // #ifdef _DEBUG
    //     // bool ret = SetDllDirectory(L"C:\\Users\\dongd\\gitproject\\GameEngine\\Dll\\x64\\Debug\\");
    //
    //     // if (ret != true)
    //     //   assert(1);
    //     // 메모리 누수 체크 활성화
    //     //  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    // #endif

    Quad::Application *app = Quad::Application::GetInstance();
    Quad::EditorDirector *editorDirector = Quad::EditorDirector::GetInstance();

    Quad::AppInitData appInitData;
    appInitData.hInstance = hInstance;
    appInitData.nShowCmd = nCmdShow;
    appInitData.programDirector = editorDirector;

    if (!app->Initialize(appInitData))
        return 0;

    return app->Run();
}
