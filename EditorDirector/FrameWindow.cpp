#include "EditorDirector/FrameWindow.h"
#include"EditorDirector/DockingWindowController.h"
#include"EditorDirector/FrameWindowController.h"

Quad::FrameWindow::FrameWindow(HINSTANCE hInstance)
	:TaskWindow(hInstance , EWindowType::eFrameWindow), mProjectSelectSceneFlag(true)
{

    SetClientWidth(1200);
    SetClientHeight(1000);
    CreateWindowClass();

    //이것도 window클래스 생성자 에서 너비,높이 받고 ,createwindowclass함수 호출해줘야겠다.
}

void Quad::FrameWindow::CreateWindowClass()
{
    WNDCLASS wc;
    wc.hInstance = GetHInstance();
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.hCursor = LoadCursor(0, IDC_ARROW);
    wc.hIcon = LoadIcon(0, IDI_APPLICATION);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = L"FrameWindow";
    wc.lpfnWndProc = FrameWindow::WndProc;
    wc.style = CS_HREDRAW | CS_VREDRAW| CS_DBLCLKS;

    if (!RegisterClass(&wc))
    {
        MessageBox(0, L"RegisterClass Failed", 0, 0);
        //return false;
    }



    RECT windowClientRect{ 0,0,(LONG)GetClientWidth(),(LONG)GetClientHeight() };
    int windowWidth = 800;
    int windowHeight = 600;
    if (AdjustWindowRect(&windowClientRect, WS_POPUP, false))
    {
        windowWidth = windowClientRect.right - windowClientRect.left;
        windowHeight = windowClientRect.bottom - windowClientRect.top;
    }

    HWND hwnd = CreateWindowEx( WS_EX_ACCEPTFILES| WS_EX_APPWINDOW , L"FrameWindow", L"FrameWindow", WS_POPUP|WS_MINIMIZEBOX,
        0, 0, windowClientRect.right, windowClientRect.bottom, 0, 0, GetHInstance(), nullptr);


    if (hwnd == NULL)
    {
        MessageBox(0, L"CreateWindow Failed", 0, 0);
        return;
        //return false;
    }
    SetWindowHandle(hwnd);
    GetClientRect(hwnd, &windowClientRect);
    SetClientWidth(windowWidth);
    SetClientHeight(windowHeight);

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    return;



}

void Quad::FrameWindow::Initialize(BaseWindowController* windowController)
{
    
    FrameWindowController* frameWindowController = (FrameWindowController*)windowController;
    TaskWindow::Initialize(frameWindowController);

    SetEdgeDragResizeFlag(true);
    frameWindowController->SetBackgroundColor(1.0f, 1.0f, 1.0f, 1.0f);


    //title bar 가 있다 .

    //윈도우의 크기는 몇이다 .
    //title bar의 사이즈는 몇이다. 그래서 킅라이언트영역은 윈도우사이즈에서 타이틀바사이즈를 뺸것이될것이다.
    //그럼 이logical정보로 controller에서 가지는게 좋을거같은데

    frameWindowController->SetTitleBarSize(true, 3000, 60);



}

LRESULT Quad::FrameWindow::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    return GetInstance()->InnerWndProc(hwnd, msg, wParam, lParam);
}

bool Quad::FrameWindow::GetProjectSelectSceneFlag () const
{

    return mProjectSelectSceneFlag;
}

void Quad::FrameWindow::SetProjectSelectSceneFlag(bool flag)
{
    mProjectSelectSceneFlag = flag;
}
