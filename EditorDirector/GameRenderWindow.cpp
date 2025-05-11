#include "GameRenderWindow.h"
#include"resource.h"
#include"KeyEvent.h"
#include"KeyBoard.h"
#include"Mouse.h"
#include"DockingWindowController.h"
#include"WindowResizeEvent.h"

namespace Quad
{

    GameRenderWindow::GameRenderWindow(HINSTANCE hInstance)
        :TaskWindow(hInstance, EWindowType::eGamePlayWindow)
    {
        SetClientWidth(400);
        SetClientHeight(1000);
        CreateWindowClass();
    }


    void GameRenderWindow::Initialize(BaseWindowController* windowController)
    {
        TaskWindow::Initialize(windowController);

        //option 설정

        //사이즈조절이 가능하다 불가능하다.
        //물론 테투리가 없을때 유효한설정이겠지.
        //
        SetEdgeDragResizeFlag(true);

        static_cast<DockingWindowController*>(windowController)->SetBackgroundColor(0.0f, 1.0f, 1.0f, 1.0f);





    }

    void GameRenderWindow::CreateWindowClass()
    {
        WNDCLASS wc = { 0, };
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = WndProc;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = GetHInstance();
        wc.hIcon = LoadIcon(0, IDI_APPLICATION);
        wc.hCursor = LoadCursor(0, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
        wc.lpszMenuName = nullptr;// MAKEINTRESOURCE(IDR_MENU1);
        wc.lpszClassName = L"GameRenderWindow";


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

        HWND hwnd = CreateWindowEx(WS_EX_ACCEPTFILES| WS_EX_LAYERED /*WS_EX_TOOLWINDOW*/, L"GameRenderWindow", L"GameRenderWindow",
            WS_POPUP| WS_MINIMIZEBOX,
            CW_USEDEFAULT, CW_USEDEFAULT, windowClientRect.right, windowClientRect.bottom, 0, 0, GetHInstance(), nullptr);
        
       
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


    LRESULT GameRenderWindow::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        GameRenderWindow* windowInstance = GetInstance();
        return windowInstance->InnerWndProc(hwnd, msg, wParam, lParam);
    }


}