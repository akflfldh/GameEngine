#include "EditorDirector/Window.h"



#include"Core/KeyEvent.h"
#include"Core/KeyBoard.h"
#include"Core/Controller.h"
#include"EditorDirector/DockingWindowController.h"


#include"Core/resource.h"
#include"Core/KeyEvent.h"
#include"Core/KeyBoard.h"
#include"Core/Mouse.h"
#include"Core/MouseEvent.h"
#include"Core/WindowResizeEvent.h"
#include"Core/MouseInOutEvent.h"
#include"EditorDirector/GameRenderWindow.h"
#include"EditorDirector/AttributeWindow.h"
#include"EditorDirector/FileUiWindow.h"
#include"EditorDirector/FrameWindow.h"

#include"Core/WindowActiveEvent.h"


namespace Quad
{

    //float TaskWindow::mMaxWidth = GetSystemMetrics(SM_CXMAXTRACK); // 최대 너비
    //float TaskWindow::mMaxHeight = GetSystemMetrics(SM_CYMAXTRACK); // 최대 높이


	TaskWindow::TaskWindow(HINSTANCE hInstance, EWindowType windowID)
        :BaseWindow(hInstance,windowID), mWindowMoveFlag(false)
	{

	}

  
    void TaskWindow::Update(float deltaTime)
    {
        mWindowController->Update(deltaTime);




    }

    void TaskWindow::UploadObjectToRenderSystem()
    {
        mWindowController->UploadObjectToRenderSystem();


    }

    void TaskWindow::EndUpdate(float deltaTime)
    {


        mWindowController->EndUpdate(deltaTime);






    }

    void TaskWindow::Draw()
    {
        mWindowController->Draw();
    }

    void TaskWindow::OnResize(int width, int height,int dragDirection)
    {
        if (mWindowController)
        {
            if(width != 0 && height !=0 )
            mWindowController->OnResize(width, height, dragDirection);
        }

    }

    void TaskWindow::Initialize(BaseWindowController* windowController)
    {
        BaseWindow::Initialize(windowController);
        
        mWindowController = static_cast<DockingWindowController*>(windowController);
        mKeyBoard = KeyBoard::GetInstance();


        SetEdgeDragResizableDirection(EWindowEdgeDragResizableDirection::eLeft, true);
        SetEdgeDragResizableDirection(EWindowEdgeDragResizableDirection::eRight, true);
        SetEdgeDragResizableDirection(EWindowEdgeDragResizableDirection::eTop, true);
        SetEdgeDragResizableDirection(EWindowEdgeDragResizableDirection::eBottom, true);


        SetNotDockingMaxClientWidth(GetMaxClientWidth());
        SetNotDockingMaxClientHeight(GetMaxClientHeight());
        SetNotDockingMinClientWidth(GetMinClientWidth());
        SetNotDockingMinClientHeight(GetMinClientHeight());

    }

  

	void TaskWindow::SetClientWidthFloat(float width)
	{
        if (GetMaxClientWidth() < width)
            width = GetMaxClientWidth();
        else if (GetMinClientWidth() > width)
            width = GetMinClientHeight();

		mClientWidthFloat = width;
	}

	void TaskWindow::SetClientHeightFloat(float height)
	{
        if (GetMaxClientHeight() < height)
            height = GetMaxClientHeight();
        else if (GetMinClientHeight() > height)
            height = GetMinClientHeight();
		mClientHeightFloat = height;
	}


	float TaskWindow::GetClientWidthFloat() const
	{
		return mClientWidthFloat;
	}

	float TaskWindow::GetClientHeightFloat() const
	{
		return mClientHeightFloat;
	}




	void TaskWindow::SetWindowMoveFlag(bool flag)
	{
		mWindowMoveFlag = flag;
	}

	bool TaskWindow::GetWindowMoveFlag() const
	{
		return mWindowMoveFlag;
	}

 

    LRESULT TaskWindow::InnerWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
       // static int test = 0;
       static bool move = false;


        if(mWindowController)
           return  mWindowController->HandleMsg(hwnd, msg, wParam, lParam);



        switch (msg)
        {


        case WM_CREATE:
        
            
            break;

        case WM_DROPFILES:
        {
            
            HDROP hdrop = (HDROP)wParam;
            int fileNum = DragQueryFileA(hdrop, 0xFFFFFFFF, nullptr, 0);
            char fileNameBuffer[300];
            std::vector<std::string> dropFileVector;
            for (int i = 0; i < fileNum; ++i)
            {
                int fileNameLength  =  DragQueryFileA(hdrop, i, NULL, 0);
                DragQueryFileA(hdrop, i, fileNameBuffer, fileNameLength+1);
                dropFileVector.push_back(fileNameBuffer);
           }
            
            mWindowController->DragAndDropFile(dropFileVector);


        }
        return 0;
        case WM_MOVE:
        {

            int preWindowPosX = GetWindowPositionX();
            int preWindowPosY = GetWindowPositionY();
            SetWindowPositionX(GET_X_LPARAM(lParam));
            SetWindowPositionY(GET_Y_LPARAM(lParam));


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



            if(mWindowController!=nullptr)
            mWindowController->WindowMove(preWindowPosX, preWindowPosY, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

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
       
        case WM_ACTIVATE:
        {

            WindowActiveEvent* activeEvent = new WindowActiveEvent;

            switch (wParam)
            {
            case WA_CLICKACTIVE:
            case WA_ACTIVE:
                KeyBoard::ActiveSystemHook();
                activeEvent->SetWindowActiveState(1);
                break;
            case WA_INACTIVE:
            {
                KeyBoard::InActiveSystemHook();
                activeEvent->SetWindowActiveState(0);



            }
                break;
            }
  

            if (!KeyBoard::GetTextBoxActiveFlag())
            {

                HIMC hIMC = ImmGetContext(hwnd);
                DWORD currentCMODE, sentence;

                ImmGetConversionStatus(hIMC, &currentCMODE, &sentence);
                ImmSetConversionStatus(hIMC, IME_CMODE_CHARCODE, sentence);
                ImmNotifyIME(hIMC, NI_COMPOSITIONSTR, CPS_CANCEL, 0); // 조합 중인 상태를 취소
                ImmSetOpenStatus(hIMC, FALSE);


              // ImmReleaseContext(hwnd, hIMC);
            }
            














            //컨트롤러 함수를 제공하자.
            //전경에없어도 계속 로직은 돌아가야하는거니깐 뭐 키고 끄고할게없고
            //프레임윈도우가 나머지 자식윈도우들을 앞으로 보이게하는정도
            if (mWindowController != nullptr)
            {
                mWindowController->AddEvent(activeEvent);
               // mWindowController->WindowActivate(wParam);
            }
            return 0;
        }
        break;
        case WM_SETFOCUS:
        {

        }
        break;
        case WM_MOUSELEAVE:
        {
            //기본적으로 창 전체가 클라이언트영역인데
            //  ,제목표지줄을 눌르면 비클라이언트영역으로 인식되니
             //이메세지가 전달된다. 실제로 창을 나간것은 아니나. 지금은 구별할필요가없을거같다.
            //제목표시줄을 클릭해서 움직이는게 다른 엔티티들에게는 창밖으로 나간것과 동일하다고 인식해도 문제없다.
            mWindowController->SetMousePositionState(false);
            MouseInOutEvent *mouseInOutEvent =new MouseInOutEvent(EMouseInOut::eOut);
            mWindowController->AddEvent(mouseInOutEvent);


            //창밖으로 나갔을떄의 메세지 전송 

        }
        return 0;
        case WM_MOUSEHOVER:
        {
            int a = 2;

        }
        return 0;
        case WM_SYSCOMMAND:
        {
           
            switch(wParam)
            { 
            case SC_RESTORE:

                //최대화상태에서 원래상태로 돌아가는 RESTORE발생안함
                mWindowController->HandleWindowReStore();  
                break;
            case SC_TASKLIST:
            {
                int a = 2;
            }
            break;
            case SC_MINIMIZE:
            {
                //작업표시줄 아이콘 클릭은 최소화되었다.
                mWindowController->WindowMinimize();
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
        break;
        case WM_SHOWWINDOW:
        {
            int a = 2;
        }
        break;
        case WM_NCMOUSELEAVE:

        {
           
            if (mWindowController)
            mWindowController->SetEdgeDragDirection(0);
           
        }
        return 0;
        case WM_NCHITTEST:
        {
            int a = 2;
            if (!GetEdgeDragResizeFlag())
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
                break;
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
                   
                    if (GetEdgeDragResizableDirectionState(EWindowEdgeDragResizableDirection::eRight))
                    {
                        mWindowController->SetEdgeDragDirection(2);
                        return HTRIGHT;
                    }
                }
                else if ((windowScreenPos.left + horizontalMargin) >= mouseScreenPosX)
                {

                    if (GetEdgeDragResizableDirectionState(EWindowEdgeDragResizableDirection::eLeft))
                    {
                        mWindowController->SetEdgeDragDirection(1);
                        return HTLEFT;

                    }
                }else if ((windowScreenPos.top + verticalMargin) >= mouseScreenPosY)
                {
                    if (GetEdgeDragResizableDirectionState(EWindowEdgeDragResizableDirection::eTop))
                    {
                        mWindowController->SetEdgeDragDirection(3);
                        return HTTOP;
                    }
                }
                else if ((windowScreenPos.bottom - verticalMargin) <= mouseScreenPosY)
                {
                    if (GetEdgeDragResizableDirectionState(EWindowEdgeDragResizableDirection::eBottom))
                    {
                        mWindowController->SetEdgeDragDirection(4);
                        return HTBOTTOM;
                    }
                }
                else
                {
                    mWindowController->SetEdgeDragDirection(0);
                }

            }
            else
            {
                int a = 2;
                mWindowController->SetEdgeDragDirection(0);
            }
        }
        break;
        case WM_SIZE:




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
            
  


            
            if (mWindowController)
            OnResize(LOWORD(lParam), HIWORD(lParam), mWindowController->GetEdgeDragDirection());


            FileUiWindow* testw = dynamic_cast<FileUiWindow*>(this);
            if (testw != nullptr)
            {
                OutputDebugString(L"파일유아이윈도우리사이즈한번수행\n");

            }


        }
        //소수점유지
        return 0;
        case WM_ENTERSIZEMOVE://창의크기조절테투리를 클릭할때 전달되는 메시지
          //  mGameTimer.Stop();
          //  mIspaused = true;
           // mIsResizing = true;{
        {
            //OnResize(mEdgeDragDirection);

        }
        return 0;
        case WM_EXITSIZEMOVE://창의크기조절테두리에서 마우스를 땟을때 
            //mGameTimer.Start();
           // mIspaused = false;
           // mIsResizing = false;
            if (move)
            {
                SendMessage(GetWindowHandle(), WM_SYSCOMMAND, HTCAPTION, 0);
                SendMessage(GetWindowHandle(), WM_LBUTTONUP, mClientMousePosX, mClientMousePosY);
                move = false;
                
               // OnResize(GetClientWidth(), GetClientHeight(), mWindowController->GetEdgeDragDirection());
                mWindowController->SetEdgeDragDirection(0);
            }


            return 0;

        case WM_GETMINMAXINFO:
            ((MINMAXINFO*)lParam)->ptMinTrackSize.x = GetMinClientWidth();
            ((MINMAXINFO*)lParam)->ptMinTrackSize.y = GetMinClientHeight();
            ((MINMAXINFO*)lParam)->ptMaxTrackSize.y = GetMaxClientWidth();
            ((MINMAXINFO*)lParam)->ptMaxTrackSize.y = GetMaxClientHeight();
            return 0;
          
        case WM_LBUTTONDOWN:
        {  
          //  HandleDefaultLButtonDown(wParam, lParam);


                MouseEvent* event = new MouseEvent("LButtonDown");
        //event->SetPreClientPosition(mClientMousePosX, mClientMousePosY);
        //event->SetClientPosition(LOWORD(lParam), HIWORD(lParam));
        //mClientMousePosX = LOWORD(lParam);
        //mClientMousePosY = HIWORD(lParam);

        

        POINT preClientPosition = { mClientMousePosX,mClientMousePosY };
        event->SetPreClientPosition(preClientPosition.x, preClientPosition.y);
        ClientToScreen(GetWindowHandle(), &preClientPosition);
        event->SetPreScreenPosition(preClientPosition.x, preClientPosition.y);

        POINT currClientPosition = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        event->SetClientPosition(currClientPosition.x, currClientPosition.y);
        mClientMousePosX = currClientPosition.x;
        mClientMousePosY = currClientPosition.y;

        ClientToScreen(GetWindowHandle(), &currClientPosition);
        event->SetScreenPosition(currClientPosition.x, currClientPosition.y);


        mWindowController->AddEvent(event);

        Mouse::SetLButtonState(true);
        }
        return 0;
        case WM_RBUTTONDOWN:
            // MouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        {   
            HandleDefaultRButtonDown(wParam, lParam);

        //    MouseEvent* event = new MouseEvent("RButtonDown");
        ///*      event->SetPreClientPosition(mClientMousePosX, mClientMousePosY);
        //      event->SetClientPosition(LOWORD(lParam), HIWORD(lParam));
        //      mClientMousePosX = LOWORD(lParam);
        //      mClientMousePosY = HIWORD(lParam);*/

        //POINT preClientPosition = { mClientMousePosX,mClientMousePosY };
        //event->SetPreClientPosition(preClientPosition.x, preClientPosition.y);
        //ClientToScreen(GetWindowHandle(), &preClientPosition);
        //event->SetPreScreenPosition(preClientPosition.x, preClientPosition.y);

        //POINT currClientPosition = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        //event->SetClientPosition(currClientPosition.x, currClientPosition.y);
        //mClientMousePosX = currClientPosition.x;
        //mClientMousePosY = currClientPosition.y;

        //ClientToScreen(GetWindowHandle(), &currClientPosition);
        //event->SetScreenPosition(currClientPosition.x, currClientPosition.y);
        //mWindowController->AddEvent(event);

        //Mouse::SetRButtonState(true);
        //OutputDebugString(L"RButtonDownOtherWindow\n");
        
        }
        return 0;
        case WM_LBUTTONUP:
        {
            HandleDefaultLButtonUp(wParam, lParam);
            // MouseEvent* event = new MouseEvent("LButtonUp");
            ///*  event->SetPreClientPosition(mClientMousePosX, mClientMousePosY);
            //  event->SetClientPosition(LOWORD(lParam), HIWORD(lParam));
            //  mClientMousePosX = LOWORD(lParam);
            //  mClientMousePosY = HIWORD(lParam);*/
            //POINT preClientPosition = { mClientMousePosX,mClientMousePosY };
            //event->SetPreClientPosition(preClientPosition.x, preClientPosition.y);
            //ClientToScreen(GetWindowHandle(), &preClientPosition);
            //event->SetPreScreenPosition(preClientPosition.x, preClientPosition.y);

            //POINT currClientPosition = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            //event->SetClientPosition(currClientPosition.x, currClientPosition.y);
            //mClientMousePosX = currClientPosition.x;
            //mClientMousePosY = currClientPosition.y;

            //ClientToScreen(GetWindowHandle(), &currClientPosition);
            //event->SetScreenPosition(currClientPosition.x, currClientPosition.y);

            //mWindowController->AddEvent(event);

            //Mouse::SetLButtonState(false);
        }
        return 0;
        case WM_RBUTTONUP:
        { 
            HandleDefaultRButtonUp(wParam, lParam);
            // MouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            //Mouse::SetRButtonState(false);

            //MouseEvent* event = new MouseEvent("RButtonUp");
            ///*  event->SetPreClientPosition(mClientMousePosX, mClientMousePosY);
            //  event->SetClientPosition(LOWORD(lParam), HIWORD(lParam));
            //  mClientMousePosX = LOWORD(lParam);
            //  mClientMousePosY = HIWORD(lParam);*/
            //POINT preClientPosition = { mClientMousePosX,mClientMousePosY };
            //event->SetPreClientPosition(preClientPosition.x, preClientPosition.y);
            //ClientToScreen(GetWindowHandle(), &preClientPosition);
            //event->SetPreScreenPosition(preClientPosition.x, preClientPosition.y);

            //POINT currClientPosition = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            //event->SetClientPosition(currClientPosition.x, currClientPosition.y);
            //mClientMousePosX = currClientPosition.x;
            //mClientMousePosY = currClientPosition.y;

            //ClientToScreen(GetWindowHandle(), &currClientPosition);
            //event->SetScreenPosition(currClientPosition.x, currClientPosition.y);

            //mWindowController->AddEvent(event);

        }

            return 0;
        case WM_LBUTTONDBLCLK:
        {
            MouseEvent* event = new MouseEvent("LButtonDownDouble");


            POINT preClientPosition = { mClientMousePosX,mClientMousePosY };
            event->SetPreClientPosition(preClientPosition.x, preClientPosition.y);
            ClientToScreen(GetWindowHandle(), &preClientPosition);
            event->SetPreScreenPosition(preClientPosition.x, preClientPosition.y);

            POINT currClientPosition = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            event->SetClientPosition(currClientPosition.x, currClientPosition.y);
            mClientMousePosX = currClientPosition.x;
            mClientMousePosY = currClientPosition.y;

            ClientToScreen(GetWindowHandle(), &currClientPosition);
            event->SetScreenPosition(currClientPosition.x, currClientPosition.y);

            mWindowController->AddEvent(event);

         //   Mouse::SetLButtonState(true);






        }
        return 0;
        case WM_NCMOUSEMOVE:

        {


          





        }
        return 0;
        case WM_MOUSEMOVE:
        {
            if (mWindowController != nullptr)
            {


                if (mWindowController->GetMousePositionState()==false)
                {//나가있었는데 창안으로 들어왔다면
                    //다시 나가는것을 추적시작
                    mWindowController->StartTrackMouseWindowOut();
                    mWindowController->SetMousePositionState(true);
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
                if(event->GetSystemID()==ESystemID::eFrameDockingSystem)
                {
                    int a = 2;
                }
                
  /*              Object* capturedObject = Mouse::GetCapturedObject();
                if (capturedObject != nullptr)
                {
                    event->SetSystemID()

                }*/

                POINT preClientPosition = { mClientMousePosX,mClientMousePosY };
                event->SetPreClientPosition(preClientPosition.x, preClientPosition.y);
                ClientToScreen(GetWindowHandle(), &preClientPosition);
                event->SetPreScreenPosition(preClientPosition.x, preClientPosition.y);

                POINT currClientPosition = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
                event->SetClientPosition(currClientPosition.x, currClientPosition.y);
                mClientMousePosX = currClientPosition.x;
                mClientMousePosY = currClientPosition.y;

                ClientToScreen(GetWindowHandle(), &currClientPosition);
                event->SetScreenPosition(currClientPosition.x, currClientPosition.y);

            
                    mWindowController->AddEvent(event);
                
            }
        }
        return 0;
        case WM_MOUSEWHEEL:

        {
            int wheelShift = GET_WHEEL_DELTA_WPARAM(wParam);
            MouseEvent* event = new MouseEvent("Wheel");
            event->SetWheelShift(wheelShift);
            mWindowController->AddEvent(event);

        }
        return 0;

        case WM_KEYDOWN:
            return CommonWM_KEYDOWN(hwnd, msg, wParam, lParam, mWindowController);

        case WM_KEYUP:
            return CommonWM_KEYUP(hwnd, msg, wParam, lParam, mWindowController);
        case WM_CHAR:

        {
            if (wParam == VK_HANGUL || wParam == VK_KANA || wParam == VK_MENU)
            {
                // 한/영 키 입력을 가로채어 IME 상태 변경을 방지
                return 0; // 메시지를 처리하고 추가로 전달하지 않음
            }

            return CommonWM_CHAR(hwnd, msg, wParam, lParam, mWindowController);


        }
        //case WM_IME_CHAR:

        //{
        //    wchar_t te = wParam;
        //    int a = 2;
        //    //이거는 wm_ime_composition에서 보내는데 우리는 composition에서 처리를하고 default보내고
        //    //이메세지는 처리하면안되는데 문제는 char로 wm_char로 보낸다는거야 여기서 또 default로보내면
        //    //그러니 여기서는 보내지않도록해야한다.
        //}
        //return 0;       //그래서 그냥 return 0이다 또생각해보면 compositon에서도 그냥 return 0을해도될거같고
           //return DefWindowProc(hwnd, msg, wParam, lParam);
        case WM_IME_COMPOSITION:


            if (KeyBoard::GetTextBoxActiveFlag())
            {
                return  CommonWM_IME_COMPOSITION(hwnd, msg, wParam, lParam, mWindowController);
            }
            else
            {
           
                int a = 2;
            }
            break;

        case WM_SYSKEYDOWN:
        {
            if (wParam == VK_HANGUL || wParam == VK_KANA || wParam == VK_RMENU || wParam == VK_MENU )
            {
                // 한/영 키 입력을 가로채어 IME 상태 변경을 방지
                return 0; // 메시지를 처리하고 추가로 전달하지 않음
            }
            // 다른 키 입력 처리
            break;
        }
        case WM_IME_KEYDOWN:
        {

             if (wParam == VK_HANGUL || wParam == VK_KANA || wParam == VK_RMENU || wParam == VK_MENU)
            {
                // 한/영 키 입력을 가로채어 IME 상태 변경을 방지
                return 0; // 메시지를 처리하고 추가로 전달하지 않음
            }
            // 다른 키 입력 처리
            break;

        }
        case WM_SYSKEYUP:
        {
            if (wParam == VK_HANGUL || wParam == VK_KANA || wParam==VK_RMENU || wParam == VK_MENU)
            {
                // 한/영 키 릴리스도 가로채기
                return 0;
            }
            break;
        }
        case WM_DESTROY://창이 파괴될때 전달되는 메시지
            PostQuitMessage(0);
            return 0;
        }

        // return DefWindowProc(hwnd, msg, wParam, lParam);
        return DefWindowProc(hwnd, msg, wParam, lParam);
		
	}

    void TaskWindow::SetEdgeDragResizeFlag(bool flag)
    {
        mEdgeDragResizeFlag = flag;
    }

    bool TaskWindow::GetEdgeDragResizeFlag() const
    {
        return mEdgeDragResizeFlag;
    }

    void TaskWindow::SetEdgeDragResizableDirection(EWindowEdgeDragResizableDirection eDirection, bool state)
    {
        switch (eDirection)
        {
        case EWindowEdgeDragResizableDirection::eLeft:
            mEdgeDragResizableDirection[0] = state;
            break;

        case EWindowEdgeDragResizableDirection::eRight:
            mEdgeDragResizableDirection[1] = state;

            break;
        case EWindowEdgeDragResizableDirection::eTop:
            mEdgeDragResizableDirection[2] = state;

            break;
        case EWindowEdgeDragResizableDirection::eBottom:
            mEdgeDragResizableDirection[3] = state;

            break;
        }



    }

    bool TaskWindow::GetEdgeDragResizableDirectionState(EWindowEdgeDragResizableDirection eDirection) const
    {

        switch (eDirection)
        {
        case EWindowEdgeDragResizableDirection::eLeft:
            return mEdgeDragResizableDirection[0];
            

        case EWindowEdgeDragResizableDirection::eRight:
            return mEdgeDragResizableDirection[1];

        case EWindowEdgeDragResizableDirection::eTop:
            return mEdgeDragResizableDirection[2];

        case EWindowEdgeDragResizableDirection::eBottom:
            return mEdgeDragResizableDirection[3];
        }
    }


    void TaskWindow::SetDockingState(bool state)
    {
        mDockingState = state;
    }

    bool TaskWindow::GetDockingState() const
    {
        return mDockingState;
    }


    float TaskWindow::GetWindowPositionX() const
    {
        return mWindowPositionX;
    }

    float TaskWindow::GetWindowPositionY() const
    {
        return mWindowPositionY;
    }

    void TaskWindow::SetWindowPositionX(float x)
    {
        mWindowPositionX = x;
    }

    void TaskWindow::SetWindowPositionY(float y)
    {
        mWindowPositionY = y;
    }

    void TaskWindow::SetMaxMinClientSizeFlag(bool flag)
    {
        mMaxMinClientSizeFlag = flag;
    }

    bool TaskWindow::GetMaxMinClientSizeFlag() const
    {
        return mMaxMinClientSizeFlag;
    }

    void TaskWindow::SetNotDockingMaxClientWidth(unsigned short width)
    {
        mNotDockingMaxClientWidth = width;
    }

    void TaskWindow::SetNotDockingMaxClientHeight(unsigned short height)
    {
        mNotDockingMaxClientHeight = height;
    }

    void TaskWindow::SetNotDockingMinClientWidth(unsigned short width)
    {
        mNotDockingMinClientWidth = width;
    }

    void TaskWindow::SetNotDockingMinClientHeight(unsigned short height)
    {
        mNotDockingMinClientHeight = height;
    }

    unsigned short TaskWindow::GetNotDockingMaxClientWidth() const
    {
        return mNotDockingMaxClientWidth;
    }

    unsigned short TaskWindow::GetNotDockingMaxClientHeight() const
    {
        return mNotDockingMaxClientHeight;
    }

    unsigned short TaskWindow::GetNotDockingMinClientWidth() const
    {
        return mNotDockingMinClientWidth;
    }

    unsigned short TaskWindow::GetNotDockingMinClientHeight() const
    {
        return mNotDockingMinClientHeight;
    }

    void TaskWindow::SetPlayMode(bool playMode)
    {
        mWindowController->SetPlayMode(playMode);

    }

    bool TaskWindow::GetPlayMode() const
    {
        return mWindowController->GetPlayMode();
    }



	LRESULT TaskWindow::CommonWM_KEYDOWN(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, Controller* controller)
	{
		VK_KEY key = wParam;

        if (wParam == VK_HANGUL || wParam == VK_KANA || wParam == VK_RMENU || wParam == VK_MENU)
        {
            // 한/영 키 입력을 가로채어 IME 상태 변경을 방지
            return 0; // 메시지를 처리하고 추가로 전달하지 않음
        }
		if (KeyBoard::GetTextBoxActiveFlag())
		{
            

			//텍스박스가 받는 유일한 가상키는 화살표방향키
			if (key == VK_LEFT || key == VK_RIGHT || key == VK_UP || key == VK_DOWN || key == VK_BACK)
			{

             
				KeyEvent* event = new KeyEvent("KeyDown");
				KeyBoard::KeyDown(key);
				event->SetKey(key);     //숫자는 가상키
				event->SetVKFlag(true);
				controller->AddEvent(event);
			}
		}
		else
		{

            //HIMC hIMC = ImmGetContext(hwnd);
            //DWORD currentCMODE, sentence;

            //ImmGetConversionStatus(hIMC, &currentCMODE, &sentence);
            //ImmSetConversionStatus(hIMC, IME_CMODE_CHARCODE, sentence);
            //ImmNotifyIME(hIMC, NI_COMPOSITIONSTR, CPS_CANCEL, 0); // 조합 중인 상태를 취소
            //ImmSetOpenStatus(hIMC, FALSE);

			KeyEvent* event = new KeyEvent("KeyDown");
			KeyBoard::KeyDown(key);
			event->SetKey(key);     
			event->SetVKFlag(true);
			controller->AddEvent(event);
				
		}

		//return DefWindowProc(hwnd, msg, wParam, lParam);
        return 0;
	}

	LRESULT TaskWindow::CommonWM_KEYUP(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, Controller* controller)
	{
		
			if (!KeyBoard::GetTextBoxActiveFlag())
			{
				KeyEvent* event = new KeyEvent("KeyUp");
				KeyBoard::KeyUp(wParam);
				event->SetKey(wParam);
				controller->AddEvent(event);
			}
		
		return  DefWindowProc(hwnd, msg, wParam, lParam);
	}

	LRESULT TaskWindow::CommonWM_CHAR(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, Controller* controller)
	{
		
		if (KeyBoard::GetTextBoxActiveFlag())
        {
                KeyEvent* event = new KeyEvent("KeyDown");
                
                if (wParam != VK_BACK)
                {
                    //mKeyBoard->KeyDown(wParam);
                    event->SetCharacter(wParam);
                    event->SetVKFlag(false);

                    controller->AddEvent(event);
                }
		}
		//return DefWindowProc(hwnd, msg, wParam, lParam);
        return 0;
	}

	LRESULT TaskWindow::CommonWM_IME_COMPOSITION(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, Controller* controller)
	{
		

			//한글에서 백스페이스프를 담은 메세지는 진행중인문자에서는 발생하지않는다.
			HIMC hIMC = ImmGetContext(hwnd);
			wchar_t str;
			uint32_t len;
			if (lParam & GCS_RESULTSTR) {
				len = ImmGetCompositionString(hIMC, GCS_RESULTSTR, NULL, 0);
				ImmGetCompositionString(hIMC, GCS_RESULTSTR, &str, len);

				// 완성된 글자
                OutputDebugString(L"CharDown5\n");
				KeyEvent* event = new KeyEvent("KeyDown");

				KeyBoard::KeyDown(wParam);
                KeyBoard::SetHangulCompositionStartFlag(false);
                KeyBoard::SetHangulCompositionContinueFlag(false);
				//event->SetKey(wParam);
				event->SetCharacter(str);
				event->SetHangulFlag(true);
				event->SetHangulCompositionFlag(false);
				event->SetVKFlag(false);
				controller->AddEvent(event);

			}

			if (lParam & GCS_COMPSTR) {
				len = ImmGetCompositionString(hIMC, GCS_COMPSTR, NULL, 0);
                if (len != 0)
                {   //backspace를 눌러 조합중인문자가 완전히 사라지면 0 , 그떈 wm_keydown메세지에서 backspace keydown이벤트륿 보낼것이다.
                    ImmGetCompositionString(hIMC, GCS_COMPSTR, &str, len);


                    // 조합중인 글자 
                    //완성된문자가 아니라 조합중인 문자라는것을알려한다.
                    OutputDebugString(L"CharDown6\n");
                    // backspace를 눌렀을떄 esc 마지막구성문자를 지웠을떄 발생한다.
                    // 근데 backspace는 한글조합중에서는 전달되지는않는다.
                    if ((wchar_t)wParam == 27)
                    {
                        str = wParam;
                    }
                    KeyEvent* event = new KeyEvent("KeyDown");
                    KeyBoard::KeyDown(wParam);

                    
      
                    if (KeyBoard::GetHangulCompositionStartFlag() == false && KeyBoard::GetHangulCompositionContinueFlag() == false )
                    {
                        KeyBoard::SetHangulCompositionStartFlag(true);//조합 첫시작

                    }
                    else
                    {
                        KeyBoard::SetHangulCompositionStartFlag(false);
                        KeyBoard::SetHangulCompositionContinueFlag(true);
                    }



                    //event->SetKey(wParam);
                    event->SetCharacter(str);
                    event->SetHangulFlag(true);
                    event->SetHangulCompositionFlag(true);
                    event->SetVKFlag(false);
                    controller->AddEvent(event);
                }
                else
                {
                    KeyEvent * event = new KeyEvent("KeyDown");
                    KeyBoard::SetHangulCompositionStartFlag(false);
                    KeyBoard::SetHangulCompositionContinueFlag(false);
                    KeyBoard::KeyDown(VK_BACK);
                    event->SetKey(VK_BACK);     //숫자는 가상키
                    event->SetVKFlag(true);
                    controller->AddEvent(event);
                }
			}

            if (len == 0)
                return DefWindowProc(hwnd, msg, wParam, lParam);
            ImmReleaseContext(hwnd, hIMC);

		return 0;
	}

    DockingWindowController* TaskWindow::GetWindowController() const
    {

        return mWindowController;
    }

	

	
}