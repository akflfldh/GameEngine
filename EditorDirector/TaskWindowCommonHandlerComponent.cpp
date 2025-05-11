#include "TaskWindowCommonHandlerComponent.h"

#include"DockingWindowController.h"
#include"MouseInOutEvent.h"

Quad::TaskWindowCommonHandlerComponent::TaskWindowCommonHandlerComponent(DockingWindowController* controller)
	:mController(controller)
{
}

LRESULT Quad::TaskWindowCommonHandlerComponent::HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, bool& ret)
{

    static bool move = false;
    
    TaskWindow* window = mController->GetWindow();

    switch (msg)
    {
    case WM_MOVE:
    {

        ret = true;

        int preWindowPosX = window->GetWindowPositionX();
        int preWindowPosY = window->GetWindowPositionY();
        window->SetWindowPositionX(GET_X_LPARAM(lParam));
        window->SetWindowPositionY(GET_Y_LPARAM(lParam));


        //// GameRenderWindow* testw = dynamic_cast<GameRenderWindow*>(this);
        //FileUiWindow* testw = dynamic_cast<FileUiWindow*>(this);
        //if (testw != nullptr)
        //{

        //    OutputDebugString(L"FileUiWindowMove\n");
        //    OutputDebugString(std::to_wstring(GET_X_LPARAM(lParam)).c_str());
        //    OutputDebugString(L"\n");
        //    OutputDebugString(std::to_wstring(GET_Y_LPARAM(lParam)).c_str());
        //    OutputDebugString(L"\n");

        //}



      //  if (mWindowController != nullptr)
            mController->WindowMove(preWindowPosX, preWindowPosY, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

        /*    if (testw != nullptr)
            {
                OutputDebugString(L"한번호출됨");

            }*/

            //drag를 해서 움직일떄 같이움직일것이다.
            //controller가 적절한처리를해야한다.
            //controller->WindowMove()
            //프레임윈도우라면 도킹된윈도우들도 움직이게끔할것이다.






        move = true;
    }
    return 0;
   /* case WM_SETFOCUS:
    {

        ret = true;
    }
    break;*/
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

    }
    return 0;
    case WM_MOUSEHOVER:
    {
        ret = true;
        int a = 2;

    }
    return 0;
    case WM_SYSCOMMAND:
    {

        ret = true;
        switch (wParam)
        {
        case SC_RESTORE:

            //최대화상태에서 원래상태로 돌아가는 RESTORE발생안함
            mController->HandleWindowReStore();
            break;
        case SC_TASKLIST:
        {
            int a = 2;
        }
        break;
        case SC_MINIMIZE:
        {
            //작업표시줄 아이콘 클릭은 최소화되었다.
            mController->WindowMinimize();
        }
        break;
        case SC_NEXTWINDOW:
        {
            int a = 2;

        }
        break;

        }

        /*if ((wParam & 0xFFF0) == SC_MOVE)
        {
            static int a2 = 0;
            OutputDebugString(L"SC_MOVE\n");
            OutputDebugString(std::to_wstring(a2).c_str());
            OutputDebugString(L"\n");
            a2++;
                int a = 2;
        }*/



    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
    case WM_SHOWWINDOW:
    {
        ret = true;
        int a = 2;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
    case WM_NCMOUSELEAVE:

    {
        ret = true;
        mController->SetEdgeDragDirection(0);

    }
    return 0;
    case WM_NCHITTEST:
    {
        ret = true;
        int a = 2;

        

        if (!window->GetEdgeDragResizeFlag())
        {

            /*int cursorState = Mouse::GetArrowCursorState();

            if (cursorState == 1)
            {
                return HTBOTTOM;
            }
            else if (cursorState == 2)
            {
                return HTLEFT;*/

                //}
            return DefWindowProc(hwnd, msg, wParam, lParam);
            //break;
        }

        static int horizontalMargin = 5;
        static int verticalMargin = 5;
        int mouseScreenPosY = GET_Y_LPARAM(lParam);
        int mouseScreenPosX = GET_X_LPARAM(lParam);

        RECT windowScreenPos;
        GetWindowRect(hwnd, &windowScreenPos);

        if (mouseScreenPosY > windowScreenPos.top && mouseScreenPosY <= windowScreenPos.bottom)
        {


            if ((windowScreenPos.right - horizontalMargin) <= mouseScreenPosX)
            {

                if (window->GetEdgeDragResizableDirectionState(EWindowEdgeDragResizableDirection::eRight))
                {
                    mController->SetEdgeDragDirection(2);
                    return HTRIGHT;
                }
            }
            else if ((windowScreenPos.left + horizontalMargin) >= mouseScreenPosX)
            {

                if (window->GetEdgeDragResizableDirectionState(EWindowEdgeDragResizableDirection::eLeft))
                {
                    mController->SetEdgeDragDirection(1);
                    return HTLEFT;

                }
            }
            else if ((windowScreenPos.top + verticalMargin) >= mouseScreenPosY)
            {
                if (window->GetEdgeDragResizableDirectionState(EWindowEdgeDragResizableDirection::eTop))
                {
                    mController->SetEdgeDragDirection(3);
                    return HTTOP;
                }
            }
            else if ((windowScreenPos.bottom - verticalMargin) <= mouseScreenPosY)
            {
                if (window->GetEdgeDragResizableDirectionState(EWindowEdgeDragResizableDirection::eBottom))
                {
                    mController->SetEdgeDragDirection(4);
                    return HTBOTTOM;
                }
            }
            else
            {
                mController->SetEdgeDragDirection(0);
            }

        }
        else
        {
            int a = 2;
            mController->SetEdgeDragDirection(0);
        }
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
    case WM_SIZE:



        ret = true;

        //이 메세지를 처리할떄 이전윈도우의크기,그리고 새로운 윈도우크기정보를같이 시스템,엔티티에 전달하자.

        //mRenderWindowWidth = LOWORD(lParam);
       // mRenderWindowHeight = HIWORD(lParam);

    //마우스 드래그로 사이즈를 조절했다면
    //메세지가전달되는데 여기서 처리를해야한다 근데 잘못하면 여러 창들이 재귀에빠질수있으니
    //drag로만 사이즈가조절됬을떄만 frame controller에게 event를 보내야한다.
    //event처리로인해 사이즈가조절되는거는 event를 보내서는안된다.


    //프레임윈도우 드래그 로 사이즈조절
    //wm_size이벤트발생
    //frame window controller에게 단순히 event를 전송하면안됨. (바로처리가안되기에)
    //(resize메서드(frame window)를 호출 이어서 frame window controller resize호출됨
    //frame window controller는 resize에서 자기창에대한 처리뿐만아니라, 자식창들의 크기또한 조정 
    //그러면 자식창들의 wm_resize이벤트발생 -> 그 각각의 창들의 resize메서드 호출 

    //여기서 프레임윈도우가아닌  각각창들을  resize할때도 알려야하는데 바로 frame window가 처리할수있는가?
    //그냥 event로는 하지못하는데. 아니면 controller포인터를가져와서 OnEvent로 처리? 이것도 좋은방법은아니고 
    //근데 최종사이즈조절은 프레임윈도우  컨트롤러에서 하니. 전달해야하는데
    //바로바로처리해야 사이즈를 계속조절할떄도 실시간으로 내부가 변하는게보인다.

    //아니면 프레임윈도우가 어떻게보면 부모 윈도우라고할수있으니, 프레임윈도우의 컨트롤러에 접근할수있게만드는것도 
    //괜찮은구조일수있다.


        if (wParam == SIZE_MINIMIZED) {
            // 최소화 처리
            int a = 2;

        }
        else if (wParam == SIZE_RESTORED) {
            // 복원 처리
            int a = 2;

        }



        if (wParam == SIZE_MINIMIZED)
        {
            int a = 2;
        }





        //최대 최소
        //사이즈가변화고있을땐 onresize호출하지않는다.
        //if (mDevice)
       // {
        //if (wParam == SIZE_MAXIMIZED)
        //{
        //    // mIsMinimized = false;
        //    // mIsMaximized = true;
        //    OnResize();
        //}
        //else if (wParam == SIZE_MINIMIZED)
        //{
        //    // mIsMinimized = true;
        //     // mIsMaximized = false;
        //      //OnResize();

        //}
        //else if (wParam == SIZE_RESTORED)
        //{

        //    //if (mIsMinimized)
        //    {
        //        //   mIsMinimized = false;
        //        OnResize();
        //    }
        //    // else if (mIsMaximized)
        //    {
        //        //    mIsMaximized = false;
        //           // OnResize();
        //    }
        //    // else if (mIsResizing)
        //    {




        //    }
        //    // else// API call such as SetWindowPos or mSwapChain->SetFullscreenState.
        //    {
        //        //  OnResize();
        //    }
        //}
        ////}
        {





            //if (mWindowController)
              //  OnResize(LOWORD(lParam), HIWORD(lParam), mController->GetEdgeDragDirection());
            
                int width = LOWORD(lParam);
                int height = HIWORD(lParam);

                if (width != 0 && height != 0)
                    mController->OnResize(width, height, mController->GetEdgeDragDirection());
            
        }
        //소수점유지
        return 0;
    case WM_ENTERSIZEMOVE://창의크기조절테투리를 클릭할때 전달되는 메시지
        //  mGameTimer.Stop();
        //  mIspaused = true;
         // mIsResizing = true;{
    {
        //OnResize(mEdgeDragDirection);

        ret = true;
    }
    return 0;
    case WM_EXITSIZEMOVE://창의크기조절테두리에서 마우스를 땟을때 
        //mGameTimer.Start();
       // mIspaused = false;
       // mIsResizing = false;
    {
        ret = true;
        HWND windowHandle = mController->GetWindow()->GetWindowHandle();
        int clientMousePosX = mController->GetClientMousePosX();
        int clientMousePosY = mController->GetClientMousePosY();
        if (move)
        {
            SendMessage(windowHandle, WM_SYSCOMMAND, HTCAPTION, 0);
            SendMessage(windowHandle, WM_LBUTTONUP, clientMousePosX, clientMousePosY);
            move = false;

            // OnResize(GetClientWidth(), GetClientHeight(), mWindowController->GetEdgeDragDirection());
            mController->SetEdgeDragDirection(0);
        }

    }
        return 0;

    case WM_GETMINMAXINFO:

    {

        ret = true;


        ((MINMAXINFO*)lParam)->ptMinTrackSize.x = window->GetMinClientWidth();
        ((MINMAXINFO*)lParam)->ptMinTrackSize.y = window->GetMinClientHeight();
        ((MINMAXINFO*)lParam)->ptMaxTrackSize.y = window->GetMaxClientWidth();
        ((MINMAXINFO*)lParam)->ptMaxTrackSize.y = window->GetMaxClientHeight();


    }
        return 0;
    case WM_DESTROY://창이 파괴될때 전달되는 메시지
        ret = 0;
        PostQuitMessage(0);
        return 0;
    }






}
