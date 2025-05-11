#include "EditorDirector/DragAndDropWindow.h"
#include"EditorDirector/DragAndDropWindowController.h"

//
//void Quad::DragAndDropWindow::SetWindowShowState(bool state)
//{
//    DragAndDropWindow* instance = GetInstance();
//    HWND hwnd = instance->GetWindowHandle();
//    if (state)
//    {
//        ShowWindow(hwnd, SW_SHOW);
//    }
//    else
//    {
//        ShowWindow(hwnd, SW_HIDE);
//    }
//
//    UpdateWindow(hwnd);
//    instance->mWindowShowState = state;
//   
//}

//bool Quad::DragAndDropWindow::GetWindowShowState()
//{
//    DragAndDropWindow* instance = GetInstance();
//    return instance->mWindowShowState;
//}

Quad::DragAndDropWindow::DragAndDropWindow(HINSTANCE hInstance)
	:BaseWindow(hInstance,EWindowType::ePopUpWindow)
{
}

void Quad::DragAndDropWindow::Initialize(DragAndDropWindowController* controller,
    unsigned short width, unsigned short height)
{
    mWindowController = controller;

    SetMaxClientWidth(10000);
    SetMaxClientHeight(10000);
    SetMinClientWidth(1);
    SetMinClientHeight(1);

    SetClientWidth(width);
    SetClientHeight(height);
    CreateWindowClass();
}

void Quad::DragAndDropWindow::Update(float deltaTime)
{
    if(DragAndDropWindowController
        ::GetWindowShowState())
        mWindowController->Update(deltaTime);
}


void Quad::DragAndDropWindow::UploadObjectToRenderSystem()
{

    mWindowController->UploadObjectToRenderSystem();

}


void Quad::DragAndDropWindow::EndUpdate(float deltaTime)
{
    mWindowController->EndUpdate(deltaTime);
}

void Quad::DragAndDropWindow::Draw()
{
    if (DragAndDropWindowController::
        GetWindowShowState())
       mWindowController->Draw();
}

void Quad::DragAndDropWindow::CreateWindowClass()
{
    WNDCLASS wc;
        wc.hInstance = GetHInstance();
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
        wc.hCursor = LoadCursor(0, IDC_ARROW);
        wc.hIcon = LoadIcon(0, IDI_APPLICATION);
        wc.lpszMenuName = NULL;
        wc.lpszClassName = L"DragAndDropWindow";
        wc.lpfnWndProc = DragAndDropWindow::WndProc;
        wc.style = CS_HREDRAW | CS_VREDRAW;
    
        if (!RegisterClass(&wc))
        {
            MessageBox(0, L"RegisterClass Failed", 0, 0);
            //return false;
        }
    
    
    
        RECT windowClientRect{ 0,0,(LONG)GetClientWidth(),(LONG)GetClientHeight() };
        int windowWidth = 0;
        int windowHeight = 0;
        if (AdjustWindowRect(&windowClientRect, WS_POPUP, false))
        {
            windowWidth = windowClientRect.right - windowClientRect.left;
            windowHeight = windowClientRect.bottom - windowClientRect.top;
        }
    
        HWND hwnd = CreateWindowEx( WS_EX_LAYERED  /*|WS_EX_TOOLWINDOW*/, L"DragAndDropWindow", L"DragAndDropWindow", WS_POPUP ,
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
        //ShowWindow(hwnd, SW_HIDE);
        UpdateWindow(hwnd);
        SetWindowHandle(hwnd);
        return;
}

LRESULT Quad::DragAndDropWindow::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	DragAndDropWindow* instance = GetInstance();
	return instance->InnerWndProc(hwnd, msg, wParam, lParam);
}



LRESULT Quad::DragAndDropWindow::InnerWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	
	switch (msg)
	{
    case WM_DESTROY://창이 파괴될때 전달되는 메시지
        PostQuitMessage(0);
        return 0;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}
