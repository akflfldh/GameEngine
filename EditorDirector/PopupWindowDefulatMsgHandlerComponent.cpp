#include "PopupWindowDefulatMsgHandlerComponent.h"
#include"PopupWindowController.h"
#include"CustomWindowMessage.h"
Quad::PopupWindowDefulatMsgHandlerComponent::PopupWindowDefulatMsgHandlerComponent(PopupWindowController* controller)
	:mController(controller)
{
}

LRESULT Quad::PopupWindowDefulatMsgHandlerComponent::HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, bool& ret)
{
    //마우스가 창밖으로 나갔는지를 추적하는메서드를 호출해야하는지를 나타내는플래그
    static bool mouseLeaveTrackFuncFlag = true;

    ret = true;

    switch (msg)
    {
    case WM_CREATE:

        return 0;
    case WM_ACTIVATE:
        switch (wParam)
        {
        case WA_INACTIVE:
        {
            mController->HandleInActiveMsg();
        }
        break;
        }


        return 0;
    case WM_POPUP_HIDE:
        mController->SetVisiblity(false);
        return 0;
    case WM_SIZE:

    {
        //if(wParam == SIZE_MINIMIZED) 발생하지않아.
      
        mController->OnResize(LOWORD(lParam), HIWORD(lParam));


    }
    return 0;
   /* case WM_MOUSEMOVE:
        HandleDefaultMouseMove(wParam, lParam);

        if (mouseLeaveTrackFuncFlag)
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
    return 0;*/

    //default:
    //    return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    ret = false;
    return 0;

}
