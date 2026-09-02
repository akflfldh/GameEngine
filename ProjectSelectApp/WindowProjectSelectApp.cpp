
#ifdef _WINDOWS

#include "WindowProjectSelectApp.h"
#include <JsonParser.h>
#include <Utility.h>
#include <assert.h>

#define mProjectExecuteButtonID 100
#define mProjectCreationButtonID 101

QuadPSA::WindowProjectSelectApp *QuadPSA::WindowProjectSelectApp::mInstance = nullptr;

QuadPSA::WindowProjectSelectApp *QuadPSA::WindowProjectSelectApp::GetInstance()
{
    if (mInstance == nullptr)
    {
        assert(0);
    }

    return mInstance;
}

QuadPSA::WindowProjectSelectApp::WindowProjectSelectApp(HINSTANCE hInstance)
    : mWindow(hInstance), mSelectedProjectIndex(-1)
{
    if (mInstance != nullptr)
    {
        assert(0);
    }
    mInstance = this;

    mWindow.Initialize(WindowProjectSelectApp::WinProc);

    mWindow.CreateWindowClass(L"ProjectSelectApp", L"ProjectSelectProgram",
                              WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX);
}

QuadPSA::WindowProjectSelectApp::~WindowProjectSelectApp() {}

void QuadPSA::WindowProjectSelectApp::Initialize()
{

    // 실행파일에 동일한 위치에 projectConfig 파일이있다.
    // 가볍게텍스트나 json
    mJsonParser.ReadFile("./ProjectConfigLists.json");

    JsonParser::JsonPath projectNumPath;
    projectNumPath.push_back("ProjectNum");

    int projectNum = 0;
    bool ret = mJsonParser.GetValue<int>(projectNumPath, projectNum);
    if (ret == false)
    {
        // log
        return;
    }

    mProjectConfigLists.resize(projectNum);

    JsonParser::JsonPath projectListPath;
    projectListPath.push_back("ProjectList");
    projectListPath.push_back(0);
    for (int i = 0; i < projectNum; ++i)
    {
        projectListPath.back() = i;
        projectListPath.push_back("Path");

        std::string path, version, name;
        mJsonParser.GetValue<std::string>(projectListPath, path);

        projectListPath.back() = "Version";
        mJsonParser.GetValue<std::string>(projectListPath, version);

        projectListPath.back() = "Name";
        mJsonParser.GetValue<std::string>(projectListPath, name);

        mProjectConfigLists[i].Initialize(path, version, name);
        projectListPath.pop_back();
    }

    if (mProjectListHandle == NULL)
    {
        DWORD error = GetLastError();
        // log
        return;
    }

    for (int i = 0; i < mProjectConfigLists.size(); i++)
    {
        // const std::string& name = mProjectConfigLists[i].GetName().c_str();
        const std::wstring &name = CoreUtility::Utility::ConvertToWString(mProjectConfigLists[i].GetName(), true);

        SendMessage(mProjectListHandle, LB_ADDSTRING, 0, (LPARAM)(name.c_str()));
    }
}

void QuadPSA::WindowProjectSelectApp::CreateWindowController(HWND hwnd)
{

    mProjectExecuteButtonHandle =
        CreateWindowW(L"button", L"프로젝트 실행", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 20, 600, 120, 25, hwnd,
                      (HMENU)mProjectExecuteButtonID, mWindow.GetHInstance(), NULL);

    mProjectListHandle = CreateWindowW(L"listbox", nullptr, WS_CHILD | WS_VISIBLE | LBS_NOTIFY | WS_VSCROLL | WS_BORDER,
                                       10, 10, 500, 500, hwnd, NULL, mWindow.GetHInstance(), NULL);

    if (mProjectExecuteButtonHandle == NULL || mProjectListHandle == NULL)
    {
        MessageBox(nullptr, L"APP초기화 실패", L"에러", 0);

        // RETURN FALSE;
    }
}

void QuadPSA::WindowProjectSelectApp::Run()
{

    MSG msg;
    int ret;
    mRunFlag = true;
    while (mRunFlag)
    {

        while (PeekMessage(&msg, mWindow.GetWindowHandle(), 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
}

void QuadPSA::WindowProjectSelectApp::EndApp()
{
    mRunFlag = false;
}

LRESULT QuadPSA::WindowProjectSelectApp::WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    WindowProjectSelectApp *instance = GetInstance();
    return instance->InnerWinProc(hwnd, msg, wParam, lParam);
}

LRESULT QuadPSA::WindowProjectSelectApp::InnerWinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CREATE:

        CreateWindowController(hwnd);
        // 윈도우가 생성될 때 한 번 호출됩니다.
        // 여기에서 UI 컨트롤(버튼, 리스트박스 등)을 생성하고 배치하는 코드를 작성할 수 있습니다.
        // 예를 들어:
        // HWND hButton = CreateWindowEx(0, L"BUTTON", L"프로젝트 열기",
        //     WS_CHILD | WS_VISIBLE, 10, 10, 100, 30, hwnd, (HMENU)1001, GetModuleHandle(NULL), NULL);
        // HWND hListBox = CreateWindowEx(0, L"LISTBOX", NULL,
        //     WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_BORDER, 10, 50, 300, 200, hwnd, (HMENU)1002,
        //     GetModuleHandle(NULL), NULL);
        return 0; // 메시지를 처리했음을 나타냅니다.

    case WM_COMMAND:
        // 컨트롤(버튼, 메뉴 등)에서 발생하는 알림 메시지를 처리합니다.
        // wParam의 하위 워드(LOWORD)는 컨트롤의 ID 또는 메뉴 ID입니다.
        // wParam의 상위 워드(HIWORD)는 알림 코드입니다 (버튼 클릭 시 BN_CLICKED 등).
        // lParam은 컨트롤의 HWND입니다.
        {
            {
                int controlId = LOWORD(wParam);
                int notificationCode = HIWORD(wParam);

                switch (notificationCode)
                {

                case LBN_SELCHANGE:

                {
                    // project 선택
                    int projectIndex = SendMessage(mProjectListHandle, LB_GETCURSEL, 0, 0);
                    if (projectIndex != LB_ERR)
                    {
                        mSelectedProjectIndex = projectIndex;
                    }
                    else
                        mSelectedProjectIndex = -1;
                }
                break;

                case BN_CLICKED:
                {
                    switch (controlId)
                    {
                    case mProjectExecuteButtonID:
                    {

                        EexcuteProject();
                    }
                    break;
                    }
                }
                }
            }
        }
        return 0;

    case WM_SIZE:
        // 윈도우 크기가 변경될 때 호출됩니다.
        // UI 컨트롤들의 위치나 크기를 윈도우 크기에 맞게 재조정하는 로직을 여기에 작성할 수 있습니다.
        // LOWORD(lParam)은 새로운 클라이언트 영역 너비, HIWORD(lParam)는 새로운 클라이언트 영역 높이입니다.
        InvalidateRect(hwnd, nullptr, TRUE);
        return 0;

    case WM_PAINT:
        // 윈도우의 클라이언트 영역이 다시 그려져야 할 때 호출됩니다.
        // GDI/DirectX/OpenGL 등을 사용하여 윈도우에 내용을 그리는 로직을 여기에 작성합니다.
        // 여기서는 기본 DefWindowProc으로 전달하여 시스템이 기본 처리를 하도록 할 수도 있습니다.
        // PAINTSTRUCT ps;
        // HDC hdc = BeginPaint(hwnd, &ps);
        // // 여기에 그리기 코드 작성...
        // EndPaint(hwnd, &ps);

        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            EndPaint(hwnd, &ps);
        }
        return 0;

    case WM_CLOSE:
        // 사용자가 닫기 버튼을 누르거나 시스템 메뉴에서 닫기를 선택했을 때 호출됩니다.
        // DestroyWindow 함수를 호출하여 윈도우를 파괴합니다.
        DestroyWindow(hwnd);
        return 0;

    case WM_DESTROY:
        // 윈도우가 파괴될 때 호출됩니다 (DestroyWindow 호출 후).
        // 애플리케이션의 메시지 루프를 종료하기 위해 WM_QUIT 메시지를 게시합니다.
        EndApp();
        PostQuitMessage(0);
        return 0;

        // 그 외의 모든 메시지는 기본 윈도우 프로시저에 의해 처리되도록 합니다.
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}

void QuadPSA::WindowProjectSelectApp::EexcuteProject()
{

    if (mSelectedProjectIndex == -1)
    {
        return;
    }

    // 에디터 프로그램을 실행한다.
    // 같은곳에 존재한다고 본다.

    std::wstring commandLine;
    commandLine += L"\"EditorDirector.exe\"";
    commandLine +=
        L" " + CoreUtility::Utility::ConvertToWString(mProjectConfigLists[mSelectedProjectIndex].GetPath(), true);
    STARTUPINFOW startupInfo;
    memset(&startupInfo, 0, sizeof(STARTUPINFO));
    startupInfo.cb = sizeof(startupInfo);

    wchar_t currentDirectoryPath[512];
    GetCurrentDirectory(512, currentDirectoryPath);

    _PROCESS_INFORMATION processInfo;
    CreateProcess(L"../x64/Debug/EditorDirector.exe", commandLine.data(), nullptr, nullptr, false,
                  CREATE_UNICODE_ENVIRONMENT, NULL, NULL, &startupInfo, &processInfo);
    CloseHandle(processInfo.hProcess);
    CloseHandle(processInfo.hThread);
    // QuitMessage()

    SendMessage(mWindow.GetWindowHandle(), WM_DESTROY, 0, 0);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{

    QuadPSA::WindowProjectSelectApp app(hInstance);
    app.Initialize();
    app.Run();

    return 0;
}

#endif
