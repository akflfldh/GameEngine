#include "GameWindow.h"
#include"GameWindowController.h"

namespace Quad
{
    GameWindow::GameWindow(HINSTANCE hInstance)
        :BaseWindow(hInstance, Quad::EWindowType::eGamePlayWindow)
    {



    }

    GameWindow::~GameWindow()
    {
    }

    void GameWindow::Initialize(Quad::BaseWindowController* windowController)
    {
        Quad::BaseWindow::Initialize(windowController);
        mGameWindowController = static_cast<GameWindowController*>(windowController);

    }

    void GameWindow::Update(float deltaTime)
    {
        mGameWindowController->Update(deltaTime);

    }

    void GameWindow::UploadObjectToRenderSystem()
    {
      

        mGameWindowController->UploadObjectToRenderSystem();

    }

    void GameWindow::EndUpdate(float deltaTime)
    {
       /// BaseWindowController *  GetWindowController();
        mGameWindowController->EndUpdate(deltaTime);

    }

    void GameWindow::Draw()
    {
        mGameWindowController->Draw();


    }

    void GameWindow::CreateWindowClass()
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
        wc.lpszClassName = L"GameWindow";


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

        HWND hwnd = CreateWindowEx(WS_EX_LEFT/*WS_EX_TOOLWINDOW*/, L"GameWindow", L"Game",
            WS_BORDER | WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
            CW_USEDEFAULT, CW_USEDEFAULT, windowClientRect.right, windowClientRect.bottom, 0, 0, GetHInstance(), nullptr);

        if (hwnd == NULL)
        {
            DWORD error = GetLastError();
            std::wstring errorMessage = L"CreateWindow Failed , error code :" + std::to_wstring(error);
            MessageBox(0, errorMessage.c_str(), 0, 0);
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

    LRESULT GameWindow::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        auto instance = GetInstance();
        return    instance->InnerWndProc(hwnd, msg, wParam, lParam);

    }

    LRESULT GameWindow::InnerWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        BaseWindowController* controller = GetWindowController();
        if(controller!=nullptr)
            return controller->HandleMsg(hwnd, msg, wParam, lParam);


        //controller 있기 전 최소한의 메세지 처리
        switch (msg)
        {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        }

        return DefWindowProc(hwnd, msg, wParam, lParam);

    }


}