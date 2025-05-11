#include "EditorDirector/AttributeWindow.h"
#include"Core/RenderSystem.h"
#include"System/UiSystem.h"
#include"Map/Map.h"

#include"EditorDirector/BoxEntity.h"
#include"Core/MouseEvent.h"
#include"Core/Controller.h"
#include"EditorDirector/DockingWindowController.h"

#include"ResourceManager/MeshManager.h"
#include"ResourceManager/TextureManager/TextureManager.h"

Quad::AttributeWindow::AttributeWindow(HINSTANCE hInstance)
	:TaskWindow(hInstance,EWindowType::eAttributeWindow)/*,mUiSystem(nullptr),mRenderSystem(nullptr)*/
{
    SetClientWidth(600);
    SetClientHeight(1300);
    CreateWindowClass();

}



void Quad::AttributeWindow::Initialize(BaseWindowController* windowController)
{
    TaskWindow::Initialize(windowController);

    //window option
    SetEdgeDragResizeFlag(true);



}

void Quad::AttributeWindow::CreateWindowClass()
{
	WNDCLASS wc;
	wc.hInstance = GetHInstance();
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground= (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.hCursor= LoadCursor(0, IDC_ARROW);
	wc.hIcon= LoadIcon(0, IDI_APPLICATION);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = L"AttributeWindow";
	wc.lpfnWndProc = AttributeWindow::WndProc;
    wc.style= CS_HREDRAW | CS_VREDRAW;

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

    HWND hwnd = CreateWindowEx( WS_EX_ACCEPTFILES | WS_EX_LAYERED, L"AttributeWindow", L"AttributeWindow", WS_POPUP| WS_MINIMIZEBOX,
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

LRESULT Quad::AttributeWindow::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return GetInstance()->InnerWndProc(hwnd, msg, wParam, lParam);
}

//LRESULT Quad::AttributeWindow::InnerWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
//{
//	
//	/*switch (msg)
//	{
//	case WM_CREATE:
//		MessageBox(hwnd, L"Create", L"Create", MB_OK);
//		return 0;
//    case WM_SIZE:
//        SetClientWidth(LOWORD(lParam));
//        SetClientHeight(HIWORD(lParam));
//        OnResize();
//        return 0;
//	}
//
//	return DefWindowProc(hwnd, msg, wParam, lParam);*/
//
//
//
//    ////////////////////////////////////////////
//
//
//    switch (msg)
//    {
//    case WM_CREATE:
//
//        break;
//
//    case WM_SIZE:
//        //mRenderWindowWidth = LOWORD(lParam);
//       // mRenderWindowHeight = HIWORD(lParam);
//
//        SetClientWidth(LOWORD(lParam));
//        SetClientHeight(HIWORD(lParam));
//
//        //최대 최소
//        //사이즈가변화고있을땐 onresize호출하지않는다.
//        //if (mDevice)
//       // {
//        if (wParam == SIZE_MAXIMIZED)
//        {
//            // mIsMinimized = false;
//            // mIsMaximized = true;
//            OnResize();
//        }
//        else if (wParam == SIZE_MINIMIZED)
//        {
//            // mIsMinimized = true;
//             // mIsMaximized = false;
//              //OnResize();
//
//        }
//        else if (wParam == SIZE_RESTORED)
//        {
//
//            //if (mIsMinimized)
//            {
//                //   mIsMinimized = false;
//                OnResize();
//            }
//            // else if (mIsMaximized)
//            {
//                //    mIsMaximized = false;
//                   // OnResize();
//            }
//            // else if (mIsResizing)
//            {
//
//
//
//
//            }
//            // else// API call such as SetWindowPos or mSwapChain->SetFullscreenState.
//            {
//                //  OnResize();
//            }
//        }
//        //}
//        return 0;
//    case WM_ENTERSIZEMOVE://창의크기조절테투리를 클릭할때 전달되는 메시지
//      //  mGameTimer.Stop();
//      //  mIspaused = true;
//       // mIsResizing = true;
//        return 0;
//    case WM_EXITSIZEMOVE://창의크기조절테두리에서 마우스를 땟을때 
//        //mGameTimer.Start();
//       // mIspaused = false;
//       // mIsResizing = false;
//        OnResize();
//        return 0;
//
//    case WM_GETMINMAXINFO:
//        ((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
//        ((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
//        return 0;
//
//    case WM_LBUTTONDOWN:
//    {   MouseEvent* event = new MouseEvent("LButtonDown");
//    event->SetPreClientPosition(mClientMousePosX, mClientMousePosY);
//    event->SetClientPosition(LOWORD(lParam), HIWORD(lParam));
//    mClientMousePosX = LOWORD(lParam);
//    mClientMousePosY = HIWORD(lParam);
//
//    mAttributeController->AddEvent(event);
//    }
//    return 0;
//    case WM_RBUTTONDOWN:
//        // MouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
//    {   MouseEvent* event = new MouseEvent("RButtonDown");
//    event->SetPreClientPosition(mClientMousePosX, mClientMousePosY);
//    event->SetClientPosition(LOWORD(lParam), HIWORD(lParam));
//    mClientMousePosX = LOWORD(lParam);
//    mClientMousePosY = HIWORD(lParam);
//    mAttributeController->AddEvent(event);
//    }
//    return 0;
//    case WM_LBUTTONUP:
//    {
//        MouseEvent* event = new MouseEvent("LButtonUp");
//        event->SetPreClientPosition(mClientMousePosX, mClientMousePosY);
//        event->SetClientPosition(LOWORD(lParam), HIWORD(lParam));
//        mClientMousePosX = LOWORD(lParam);
//        mClientMousePosY = HIWORD(lParam);
//        mAttributeController->AddEvent(event);
//    }
//    return 0;
//    case WM_RBUTTONUP:
//        // MouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
//        return 0;
//
//    case WM_MOUSEMOVE:
//    {
//        if (mAttributeController != nullptr)
//        {
//            MouseEvent* event = new MouseEvent("MouseMove");
//            event->SetPreClientPosition(mClientMousePosX, mClientMousePosY);
//            event->SetClientPosition(LOWORD(lParam), HIWORD(lParam));
//            mClientMousePosX = LOWORD(lParam);
//            mClientMousePosY = HIWORD(lParam);
//            mAttributeController->AddEvent(event);
//        }
//    }
//    return 0;
//    case WM_KEYDOWN:
//        return CommonWM_KEYDOWN(hwnd, msg, wParam, lParam, mAttributeController);
//        //{
//
//
//
//        //    VK_KEY key = wParam;
//        //    {
//        //        if (!mKeyBoard->GetTextBoxActiveFlag())
//        //        {
//        //            if (key == 229)
//        //            {
//        //                HIMC hIMC = ImmGetContext(hwnd);
//        //                DWORD currentCMODE;
//        //                ImmGetConversionStatus(hIMC, &currentCMODE, nullptr);
//        //                if (currentCMODE == IME_CMODE_NATIVE)
//        //                {
//        //                    ImmSetConversionStatus(hIMC, IME_CMODE_ALPHANUMERIC, IME_SMODE_AUTOMATIC);
//        //                }
//        //            }
//        //            else
//        //            {
//
//        //                KeyEvent* event = new KeyEvent("KeyDown");
//
//        //                mKeyBoard->KeyDown(key);
//        //                event->SetKey(key);     //숫자는 가상키
//        //                event->SetVKFlag(true);
//        //                mGamePlayWindowController->AddEvent(event);
//        //            }
//        //        }
//        //    }
//        //}
//        //    return DefWindowProc(hwnd, msg, wParam, lParam);
//    case WM_KEYUP:
//        return CommonWM_KEYUP(hwnd, msg, wParam, lParam, mAttributeController);
//    case WM_CHAR:
//        return CommonWM_CHAR(hwnd, msg, wParam, lParam, mAttributeController);
//    case WM_IME_CHAR:
//    {
//        wchar_t te = wParam;
//        int a = 2;
//        //이거는 wm_ime_composition에서 보내는데 우리는 composition에서 처리를하고 default보내고
//        //이메세지는 처리하면안되는데 문제는 char로 wm_char로 보낸다는거야 여기서 또 default로보내면
//        //그러니 여기서는 보내지않도록해야한다.
//    }
//    return 0;       //그래서 그냥 return 0이다 또생각해보면 compositon에서도 그냥 return 0을해도될거같고
//       //return DefWindowProc(hwnd, msg, wParam, lParam);
//    case WM_IME_COMPOSITION:
//        return  CommonWM_IME_COMPOSITION(hwnd, msg, wParam, lParam, mAttributeController);
//
//    case WM_DESTROY://창이 파괴될때 전달되는 메시지
//        PostQuitMessage(0);
//        return 0;
//    }
//
//    // return DefWindowProc(hwnd, msg, wParam, lParam);
//    return DefWindowProc(hwnd, msg, wParam, lParam);
//
//
//
//
//
//
//
//
//}

//void Quad::AttributeWindow::OnResize()
//{
//    if(mUiSystem!=nullptr)
//    mUiSystem->OnResize(GetClientWidth(), GetClientHeight());
//    if(mRenderSystem!=nullptr)
//    mRenderSystem->OnResize(GetClientWidth(), GetClientHeight());
//}



void Quad::AttributeWindow::CreateDefaultPage()
{

   // WorldEditUiEntity* textureAttribute =
   //     (WorldEditUiEntity*)WorldEditEntityFactory::GetEntityClass(L"Texture", L"ButtonEntity");

   // textureAttribute->GetTransform().SetScaleLocal({ 200,200,1.0f });
   //mUiSystem->AddEntity(textureAttribute);


  // Vector3Box* boxEntity = new Vector3Box;
  // boxEntity->Initialize("box", MeshManager::GetMesh("Rectangle"));
  // boxEntity->GetModel().SetTextureToAllSubMesh(TextureManager::GetTexture("Default.bmp"));

  //// boxEntity->GetTransform().SetScaleLocal({ 600,50,1.0f });
  // boxEntity->GetTransform().SetPositionLocal({ 0.0f,0.0f,10.0f });

  // mUiSystem->AddUiEntity(boxEntity);

}
