#include "PopupWindow.h"

#include"CustomWindowMessage.h"
#include"PopupWindowController.h"

#include"WindowResizeEvent.h"
#include"MouseInOutEvent.h"

Quad::PopupWindow::PopupWindow(HINSTANCE hInstance,int clientWidth,int clientHeight, bool visibleFlag)
	:BaseWindow(hInstance,EWindowType::ePopUpWindow), mVisibleState(visibleFlag)
{
    SetClientWidth(clientWidth);
    SetClientHeight(clientHeight);
    CreateWindowClass();

    mMonitorHandle = MonitorFromWindow(GetWindowHandle(), MONITOR_DEFAULTTOPRIMARY);


}

void Quad::PopupWindow::Initialize(BaseWindowController* controller)
{
    BaseWindow::Initialize(controller);
    mPopupController = static_cast<PopupWindowController*>(controller);
    //mController = controller;
   
}

void Quad::PopupWindow::Update(float deltaTime )
{
    mPopupController->Update(deltaTime);
}

void Quad::PopupWindow::UploadObjectToRenderSystem()
{
    mPopupController->UploadObjectToRenderSystem();
}

void Quad::PopupWindow::EndUpdate(float deltaTime)
{
    mPopupController->EndUpdate(deltaTime);

}

void Quad::PopupWindow::Draw()
{

    mPopupController->Draw();

}

void Quad::PopupWindow::CreateWindowClass()
{

    WNDCLASS wc;
    wc.hInstance = GetHInstance();
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.hCursor = LoadCursor(0, IDC_ARROW);
    wc.hIcon = LoadIcon(0, IDI_APPLICATION);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = L"PopupWindow";
    wc.lpfnWndProc = PopupWindow::WndProc;
    wc.style = CS_HREDRAW | CS_VREDRAW;

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

    HWND hwnd = CreateWindowEx(WS_EX_ACCEPTFILES /*| WS_EX_LAYERED*/  /*WS_EX_TOOLWINDOW*/,L"PopupWindow", L"PopupWindow", WS_POPUP /*| WS_MINIMIZEBOX*/,
        0, 0, windowClientRect.right, windowClientRect.bottom, 0, 0, GetHInstance(), nullptr);

  //  SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);


    if (hwnd == NULL)
    {
        MessageBox(0, L"CreateWindow Failed", 0, 0);
        return;
        //return false;
    }

    SetWindowHandle(hwnd);
    GetClientRect(hwnd, &windowClientRect);


    if (mVisibleState)
    {
        ShowWindow(hwnd, SW_SHOW);
        UpdateWindow(hwnd);
    }

    return;
}

LRESULT Quad::PopupWindow::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	auto instance = GetInstance();
	return	instance->InnerWndProc(hwnd, msg, wParam, lParam);
}

void Quad::PopupWindow::SetVisibilityState(bool flag)
{
    auto instance = GetInstance();
    HWND hwnd = instance->GetWindowHandle();
    instance->mVisibleState = flag;
    if (flag)
    {
        ShowWindow(hwnd, SW_SHOW);
    }
    else
    {
        ShowWindow(hwnd,SW_HIDE);
    }

    UpdateWindow(hwnd);
}

//void Quad::PopupWindow::ActiveWindow(int screenPosX, int screenPosY, bool visibleFlag)
//{
//    auto instance = GetInstance();
//    HWND hwnd = instance->GetWindowHandle();
//    SetVisibilityState(visibleFlag);
//}

bool Quad::PopupWindow::GetVisibilityState() const
{
    return mVisibleState;
}




LRESULT Quad::PopupWindow::InnerWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    
    //마우스가 창밖으로 나갔는지를 추적하는메서드를 호출해야하는지를 나타내는플래그


    if (mPopupController)
        return mPopupController->HandleMsg(hwnd, msg, wParam, lParam);



    static bool mouseLeaveTrackFuncFlag = true;

	switch (msg)
	{
	case WM_CREATE:

        return 0;
    case WM_ACTIVATE:
        switch (wParam)
        {
        case WA_ACTIVE:
        case WA_CLICKACTIVE:
        {

        }
            break;

        case WA_INACTIVE:
        {
            mPopupController->HandleInActiveMsg();
        }
        break;
        }


		return 0;
    case WM_POPUP_HIDE:
        mPopupController->SetVisiblity(false);
        return 0;
    case WM_SIZE :

    {   
        //if(wParam == SIZE_MINIMIZED) 발생하지않아.
        if(mPopupController !=nullptr)
            mPopupController->OnResize(LOWORD(lParam), HIWORD(lParam));
      

    }
    return 0;
    case WM_LBUTTONDOWN:

        HandleDefaultLButtonDown(wParam, lParam);

    return 0;
    case WM_RBUTTONDOWN:

        HandleDefaultRButtonDown(wParam, lParam);

        return 0;
    case WM_LBUTTONUP:

        HandleDefaultLButtonUp(wParam, lParam);

        return 0;
    case WM_RBUTTONUP:

        HandleDefaultRButtonUp(wParam, lParam);

        return 0;

    case WM_MOUSEMOVE:
        HandleDefaultMouseMove(wParam, lParam);

        if(mouseLeaveTrackFuncFlag)
        {
            TRACKMOUSEEVENT trackMouseEvent;
            trackMouseEvent.cbSize = sizeof(TRACKMOUSEEVENT);
            trackMouseEvent.dwFlags = TME_LEAVE;
            trackMouseEvent.hwndTrack = GetWindowHandle();
            trackMouseEvent.dwHoverTime = HOVER_DEFAULT;
            TrackMouseEvent(&trackMouseEvent);
        }
        
        return 0;
    case WM_MOUSELEAVE:
    {
      
        MouseInOutEvent* mouseInOutEvent = new MouseInOutEvent(EMouseInOut::eOut);
        mPopupController->AddEvent(mouseInOutEvent);
        OutputDebugStringW(L"나감\n");
        mouseLeaveTrackFuncFlag = true;
    }
        return 0;
  
    
    default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

}
