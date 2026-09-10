#include <Core/Application.h>
#include <GameDirector/GameDirector.h>

#ifdef WIN32

#include <Windows.h>

std::filesystem::path GetExecutableDirectory()
{

    std::wstring buffer(32768, L'\0');

#ifdef _WIN32

    DWORD length = GetModuleFileNameW(nullptr, buffer.data(), buffer.size());

    buffer.resize(length);
#endif

    return std::filesystem::path(buffer).parent_path();
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR pCmdLine, int nCmdShow)
{
    SetProcessDPIAware();

    Quad::Application *app = Quad::Application::GetInstance();
    Quad::GameDirector *gameDirector = Quad::GameDirector::GetInstance();

    auto gameRootDirectory = GetExecutableDirectory();

    // 현재 디버깅모드일떄만 에디터에서 올바르게 실행가능.
    // 혹은 별도의 빌드된 실행파일로 실행할 경우만
    if (IsDebuggerPresent())
    {
        gameRootDirectory = GAME_DEBUG_PROJECT_PATH;
    }

    gameDirector->InitRuntimeConfig(gameRootDirectory);

    Quad::AppInitData appInitData;
    appInitData.hInstance = hInstance;
    appInitData.nShowCmd = nCmdShow;
    appInitData.programDirector = gameDirector;

    if (!app->Initialize(appInitData))
        return 0;

    return app->Run();
}

#endif