#include "EditorDirector/FileUiWindow.h"
namespace Quad
{
    FileUiWindow::FileUiWindow(HINSTANCE hInstance) 
        :TaskWindow(hInstance,EWindowType::eFileUiWindow)
    {
        SetClientWidth(600);
        SetClientHeight(800);
        SetClientWidthFloat(600);
        SetClientHeightFloat(800);
        CreateWindowClass();
    }

    void FileUiWindow::Initialize(BaseWindowController* windowController)
    {
        TaskWindow::Initialize(windowController);
        SetEdgeDragResizeFlag(true);
    }

    void FileUiWindow::CreateWindowClass()
    {
        WNDCLASS wc;
        wc.hInstance = GetHInstance();
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
        wc.hCursor = LoadCursor(0, IDC_ARROW);
        wc.hIcon = LoadIcon(0, IDI_APPLICATION);
        wc.lpszMenuName = NULL;
        wc.lpszClassName = L"FileUiWindow";
        wc.lpfnWndProc = FileUiWindow::WndProc;
        wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;

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

        HWND hwnd = CreateWindowEx(WS_EX_ACCEPTFILES | WS_EX_LAYERED  /*WS_EX_TOOLWINDOW*/, L"FileUiWindow", L"FileUiWindow",WS_POPUP| WS_MINIMIZEBOX,
            0, 0, windowClientRect.right, windowClientRect.bottom, 0, 0, GetHInstance(), nullptr);

        SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);


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


    LRESULT FileUiWindow::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        return GetInstance()->InnerWndProc(hwnd, msg, wParam, lParam);
    }

}