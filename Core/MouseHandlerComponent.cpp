#include "MouseHandlerComponent.h"
#include"MouseEvent.h"
#include"BaseWindowController.h"
#include"BaseWindow.h"
#include"Mouse.h"
#include"MouseInOutEvent.h"
Quad::MouseHandlerComponent::MouseHandlerComponent(BaseWindowController* controller)
	:mController(controller)
{

}

LRESULT Quad::MouseHandlerComponent::HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, bool& ret)
{

    int clientMousePosX = mController->GetClientMousePosX();
    int clientMousePosY = mController->GetClientMousePosY();
    HWND windowHandle = mController->GetWindow()->GetWindowHandle();

    ret = true;
	switch (msg)
	{
    case WM_MOUSELEAVE:
    {
        ret = true;
        //기본적으로 창 전체가 클라이언트영역인데
        //  ,제목표지줄을 눌르면 비클라이언트영역으로 인식되니
         //이메세지가 전달된다. 실제로 창을 나간것은 아니나. 지금은 구별할필요가없을거같다.
        //제목표시줄을 클릭해서 움직이는게 다른 엔티티들에게는 창밖으로 나간것과 동일하다고 인식해도 문제없다.
        mController->SetMousePositionState(false);
        MouseInOutEvent* mouseInOutEvent = new MouseInOutEvent(EMouseInOut::eOut);
        mController->AddEvent(mouseInOutEvent);


        //창밖으로 나갔을떄의 메세지 전송 

    }      return 0;

    case WM_LBUTTONDOWN:
    {

        MouseEvent* event = new MouseEvent("LButtonDown");


        POINT preClientPosition = { clientMousePosX,clientMousePosY };
        event->SetPreClientPosition(preClientPosition.x, preClientPosition.y);
        ClientToScreen(windowHandle, &preClientPosition);
        event->SetPreScreenPosition(preClientPosition.x, preClientPosition.y);

        POINT currClientPosition = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        event->SetClientPosition(currClientPosition.x, currClientPosition.y);
        mController->SetClientMousePosX(currClientPosition.x);
        mController->SetClientMousePosY(currClientPosition.y);

        ClientToScreen(windowHandle, &currClientPosition);
        event->SetScreenPosition(currClientPosition.x, currClientPosition.y);


        mController->AddEvent(event);

        Mouse::SetLButtonState(true);
    }
    return 0;
    case WM_RBUTTONDOWN:
        // MouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
    {
        //HandleDefaultRButtonDown(wParam, lParam);
        MouseEvent* event = new MouseEvent("RButtonDown");
        /*      event->SetPreClientPosition(mClientMousePosX, mClientMousePosY);
              event->SetClientPosition(LOWORD(lParam), HIWORD(lParam));
              mClientMousePosX = LOWORD(lParam);
              mClientMousePosY = HIWORD(lParam);*/

        POINT preClientPosition = { clientMousePosX,clientMousePosY };
        event->SetPreClientPosition(preClientPosition.x, preClientPosition.y);
        ClientToScreen(windowHandle, &preClientPosition);
        event->SetPreScreenPosition(preClientPosition.x, preClientPosition.y);

        POINT currClientPosition = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        event->SetClientPosition(currClientPosition.x, currClientPosition.y);
        mController->SetClientMousePosX(currClientPosition.x);
        mController->SetClientMousePosY(currClientPosition.y);

        ClientToScreen(windowHandle, &currClientPosition);
        event->SetScreenPosition(currClientPosition.x, currClientPosition.y);
        mController->AddEvent(event);

        Mouse::SetRButtonState(true);
       // OutputDebugString(L"RButtonDownOtherWindow\n");
    }
    return 0;
    case WM_LBUTTONUP:
    {
        //HandleDefaultLButtonUp(wParam, lParam);
        MouseEvent* event = new MouseEvent("LButtonUp");
        /*  event->SetPreClientPosition(mClientMousePosX, mClientMousePosY);
          event->SetClientPosition(LOWORD(lParam), HIWORD(lParam));
          mClientMousePosX = LOWORD(lParam);
          mClientMousePosY = HIWORD(lParam);*/
        POINT preClientPosition = { clientMousePosX,clientMousePosY };
        event->SetPreClientPosition(preClientPosition.x, preClientPosition.y);
        ClientToScreen(windowHandle, &preClientPosition);
        event->SetPreScreenPosition(preClientPosition.x, preClientPosition.y);

        POINT currClientPosition = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        event->SetClientPosition(currClientPosition.x, currClientPosition.y);
        mController->SetClientMousePosX(currClientPosition.x);
        mController->SetClientMousePosY(currClientPosition.y);

        ClientToScreen(windowHandle, &currClientPosition);
        event->SetScreenPosition(currClientPosition.x, currClientPosition.y);

        mController->AddEvent(event);

        Mouse::SetLButtonState(false);
    }
    return 0;
    case WM_RBUTTONUP:
    {
        //HandleDefaultRButtonUp(wParam, lParam);
       

        MouseEvent* event = new MouseEvent("RButtonUp");
        /*  event->SetPreClientPosition(mClientMousePosX, mClientMousePosY);
          event->SetClientPosition(LOWORD(lParam), HIWORD(lParam));
          mClientMousePosX = LOWORD(lParam);
          mClientMousePosY = HIWORD(lParam);*/
        POINT preClientPosition = { clientMousePosX,clientMousePosY };
        event->SetPreClientPosition(preClientPosition.x, preClientPosition.y);
        ClientToScreen(windowHandle, &preClientPosition);
        event->SetPreScreenPosition(preClientPosition.x, preClientPosition.y);

        POINT currClientPosition = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        event->SetClientPosition(currClientPosition.x, currClientPosition.y);
        mController->SetClientMousePosX(currClientPosition.x);
        mController->SetClientMousePosY(currClientPosition.y);

        ClientToScreen(windowHandle, &currClientPosition);
        event->SetScreenPosition(currClientPosition.x, currClientPosition.y);

        mController->AddEvent(event);
        Mouse::SetRButtonState(false);
    }

    return 0;
    case WM_LBUTTONDBLCLK:
    {
        MouseEvent* event = new MouseEvent("LButtonDownDouble");


        POINT preClientPosition = { clientMousePosX,clientMousePosY };
        event->SetPreClientPosition(preClientPosition.x, preClientPosition.y);
        ClientToScreen(windowHandle, &preClientPosition);
        event->SetPreScreenPosition(preClientPosition.x, preClientPosition.y);

        POINT currClientPosition = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        event->SetClientPosition(currClientPosition.x, currClientPosition.y);
        mController->SetClientMousePosX(currClientPosition.x);
        mController->SetClientMousePosY(currClientPosition.y);

        ClientToScreen(windowHandle, &currClientPosition);
        event->SetScreenPosition(currClientPosition.x, currClientPosition.y);

        mController->AddEvent(event);
    }
    return 0;
    case WM_NCMOUSEMOVE:

    {








    }
    return 0;
    case WM_MOUSEMOVE:
    {
       


            if (mController->GetMousePositionState() == false)
            {//나가있었는데 창안으로 들어왔다면
                //다시 나가는것을 추적시작
               // mController->StartTrackMouseWindowOut();

                TRACKMOUSEEVENT trackMouseEvent;
                trackMouseEvent.cbSize = sizeof(trackMouseEvent);
                trackMouseEvent.dwFlags = TME_LEAVE; //결국 클라이언트영역을 나갔느냐 안나갔느냐 
                trackMouseEvent.hwndTrack = windowHandle;
                trackMouseEvent.dwHoverTime = INFINITE;

                TrackMouseEvent(&trackMouseEvent);

                mController->SetMousePositionState(true);
            }


            //클릭해서 누를때만 제목표시줄이되는거고 
            //다 클라이언트영역
            //클락이언트영역 넘어가는것만 인식하면된다.
            //아마 눌르때 클라이언트영역에서 넘어가겠지 

         /*   static int mm = 0;
            OutputDebugString(L"MouseMove\n");
            OutputDebugString(std::to_wstring(mm).c_str());
            OutputDebugString(L"\n");
            mm++;*/






            MouseEvent* event = new MouseEvent("MouseMove");
            if (event->GetSystemID() == ESystemID::eFrameDockingSystem)
            {
                int a = 2;
            }

            /*              Object* capturedObject = Mouse::GetCapturedObject();
                          if (capturedObject != nullptr)
                          {
                              event->SetSystemID()

                          }*/

            POINT preClientPosition = { clientMousePosX,clientMousePosY };
            event->SetPreClientPosition(preClientPosition.x, preClientPosition.y);
            ClientToScreen(windowHandle, &preClientPosition);
            event->SetPreScreenPosition(preClientPosition.x, preClientPosition.y);

            POINT currClientPosition = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            event->SetClientPosition(currClientPosition.x, currClientPosition.y);
            mController->SetClientMousePosX(currClientPosition.x);
            mController->SetClientMousePosY(currClientPosition.y);

            ClientToScreen(windowHandle, &currClientPosition);
            event->SetScreenPosition(currClientPosition.x, currClientPosition.y);
            
            
            mController->AddEvent(event);

            Mouse::SetCurrScreenPos(currClientPosition.x, currClientPosition.y);

        
    }
    return 0;
    case WM_MOUSEWHEEL:

    {
        int wheelShift = GET_WHEEL_DELTA_WPARAM(wParam);
        MouseEvent* event = new MouseEvent("Wheel");
        event->SetWheelShift(wheelShift);
        mController->AddEvent(event);

    }
    return 0;
	}


    ret = false;
    return 0;







}
