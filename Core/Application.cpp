#include "Core/Application.h"
//#include"VectorSpace.h"
//#include"AttributeWindow.h"
//#include"AttributeUiSystem.h"
//#include"DockingWindowController.h"
//#include"GamePlaySystem.h"
//#include"AttributeSystem.h"
//#include"AttributeUiSystem.h"
//
//#include"FileUiWindow.h"
//#include"FileUiSystem.h"
//#include"FileUiUiSystem.h"


//#include"Entity.h"
//#include"ButtonUiEntitiy.h"
//#include"WindowChromeSystem.h"
//#include"FrameWindow.h"
//#include"FrameWindowController.h"
//#include"FrameWindowSystem.h"
//#include"FrameWindowUiSystem.h"
//#include"WindowDockingSystem.h"
//#include"WindowControlEntityHeader.h"
//#include"ChromeSystemCamera.h"
//#include"DockingSystemCamera.h"
//#include"FrameWindowDockingSystem.h"
//#include"ChildWindowDockingSystem.h"
//#include"FileUiWindowContoller.h"
//#include"Text.h"



//#include"DragAndDropWindow.h"
//#include"DragAndDropWindowController.h"
//#include"DragAndDropSystem.h"
//
//#include"EffectTable.h"
//#include"Light.h"
//#include"Grid.h"
//#include"LineFactory.h"
//#include"Line.h"
//
//
//#include"CollisionWorld.h"
//#include"Gizmo.h"
//
//
//#include"GamePlayWindowCamera.h"
//
//#include"PopupWindowController.h"
//#include"PopupWindowUiSystem.h"
//
//#include"PopupSystemCamera.h"
//#include"ButtonUiEntitiy.h"
//#include"WindowResizeEvent.h"

//#include"AnimationClipSplitter.h"


//#include"RenderTargetTexture.h"


#include<tchar.h>

#include"ResourceManager/EffectManager/EffectManager.h"
#include"ObjectFactory/ObjectFactory.h"

//#include"EditorSystem.h"


//#include"EngineModeDirector.h"

//#include"EditorModeDirector.h"

#include"Core/IProgramDirector.h"

#include"Core/SpacePartitioningStructureFactory.h"
#include"Core/CollisionWorldFactory.h"


#undef EngineMode

namespace Quad
{

LRESULT CALLBACK
WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    return   Application::GetInstance()->Proc(hwnd,msg,wParam,lParam);
}



LRESULT Application::Proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

    switch (msg)
    {
    case WM_CREATE:
        //첫 위치정보가져올수도있고, 움직일경우에도 화면상에서의 윈도우의 위치정보가져올수있다.
        //그러면 도킹되어진 창들도 같이 움직이게 한다는거지.
        //당연히 분리된 창들은 영향을 안받는거고


        return 0;

    case WM_SIZE:
        mClientWidth = LOWORD(lParam);
        mClientHeight = HIWORD(lParam);
        //최대 최소
        //사이즈가변화고있을땐 onresize호출하지않는다.
        if (mDevice)
        {
            /*mFileUiWindowWidth = mClientWidth;

            MoveWindow(mFileHwnd, 0, mClientHeight - mFileUiWindowHeight, mFileUiWindowWidth, mFileUiWindowHeight, true);


            mRenderWindowHeight = mClientHeight - mFileUiWindowHeight;
            mRenderWindowWidth= mRenderWindowHeight * 23 / 20;

            MoveWindow(mRenderHwnd, (mClientWidth - mRenderWindowWidth) / 2, 0, mRenderWindowWidth, mRenderWindowHeight,false);

            mResourceController.Resize();*/

            //SendMessage(mFileHwnd, WM_SIZE, wParam, lParam);

            if (wParam == SIZE_MAXIMIZED)
            {
                mIsMinimized = false;
                mIsMaximized = true;
               // OnResize();
            }
            else if (wParam == SIZE_MINIMIZED)
            {
                mIsMinimized = true;
                mIsMaximized = false;
                //OnResize();

            }
            else if (wParam == SIZE_RESTORED)
            {

                if (mIsMinimized)
                {
                    mIsMinimized = false;
                   // OnResize();
                }
                else if (mIsMaximized)
                {
                    mIsMaximized = false;
                    //OnResize();
                }
                else if (mIsResizing)
                {




                }
                else// API call such as SetWindowPos or mSwapChain->SetFullscreenState.
                {
                   // OnResize();
                }
            }
        }
        return 0;
        //return DefFrameProc(hwnd, mHwndClient, msg, wParam, lParam);
    case WM_ENTERSIZEMOVE ://창의크기조절테투리를 클릭할때 전달되는 메시지
        mGameTimer.Stop();
        mIspaused = true;
        mIsResizing = true;
        return 0;
    case WM_EXITSIZEMOVE ://창의크기조절테두리에서 마우스를 땟을때 
        mGameTimer.Start();
        mIspaused = false;
        mIsResizing = false;
        //OnResize();
        return 0;

    case WM_GETMINMAXINFO:
        ((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
        ((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
        return 0;

    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
        MouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

        return 0;
    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
        MouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        return 0;

    case WM_MOUSEMOVE:
        MouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        return 0;

    case WM_DROPFILES:
    
      //  SendMessage(mFileHwnd, WM_DROPFILES, wParam, lParam);

    //    int count = DragQueryFile((HDROP)wParam, 0xFFFFFFFF, nullptr, 0);
    //    wchar_t filePath[MAX_PATH];
    //    std::wstring filePathAll;
    //    for (int i = 0; i < count; ++i)
    //    {
    //        int characterCount = DragQueryFile((HDROP)wParam, i, filePath, MAX_PATH);
    //        //filePath[characterCount] = '\0';

    //        if (!(GetFileAttributes(filePath) & FILE_ATTRIBUTE_DIRECTORY))
    //        {
    //            filePathAll += filePath;


    //            IWICBitmapDecoder* pDecoder = NULL;
    //            IWICBitmapFrameDecode* pSource = NULL;
    //            IWICStream* pStream = NULL;
    //            IWICFormatConverter* pConverter = NULL;
    //            IWICBitmapScaler* pScaler = NULL;

    //            HRESULT hr = mIWICFactory->CreateDecoderFromFilename(
    //                filePath,
    //                NULL,
    //                GENERIC_READ,
    //                WICDecodeMetadataCacheOnLoad,
    //                &pDecoder
    //            );


    //            if (SUCCEEDED(hr))
    //            {
    //                // Create the initial frame.
    //                hr = pDecoder->GetFrame(0, &pSource);
    //            }

    //            if (SUCCEEDED(hr))
    //            {

    //                // Convert the image format to 32bppPBGRA
    //                // (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
    //                hr = mIWICFactory->CreateFormatConverter(&pConverter);

    //            }


    //            if (SUCCEEDED(hr))
    //            {
    //                hr = pConverter->Initialize(
    //                    pSource,
    //                    GUID_WICPixelFormat32bppPBGRA,
    //                    WICBitmapDitherTypeNone,
    //                    NULL,
    //                    0.f,
    //                    WICBitmapPaletteTypeMedianCut
    //                );
    //            }

    //            if (SUCCEEDED(hr))
    //            {
    //                IWICBitmap* pWicBitmap;
    //                HRESULT hr = mIWICFactory->CreateBitmapFromSource(
    //                    pConverter,  // IWICBitmapFrameDecode 인터페이스
    //                    WICBitmapCacheOnDemand,
    //                    &pWicBitmap
    //                );

    //                ID2D1RenderTarget* ID21Rendertarget;
    //                pFactory->CreateWicBitmapRenderTarget(pWicBitmap, D2D1::RenderTargetProperties(), &ID21Rendertarget);
    //                // Create a Direct2D bitmap from the WIC bitmap.
    //             /*   hr = ID21Rendertarget->CreateBitmapFromWicBitmap(
    //                    pConverter,
    //                    NULL,
    //                    pWicBitmap
    //                );*/

    //                ID2D1Bitmap* bitmap=nullptr;
    //                hr = ID21Rendertarget->CreateBitmapFromWicBitmap(pConverter, &bitmap);
    //                auto size =bitmap->GetSize();
    //                D2D1_RECT_F rt;
    //                rt.top = 0;
    //                rt.bottom =500;
    //                rt.left = 0;
    //                rt.right = 500;
    //                
    //                PAINTSTRUCT ps;
    //                HDC hdc = GetDC(mFileHwnd);
    //                m_pRenderTarget->BeginDraw();
    //                m_pRenderTarget->DrawBitmap(bitmap, rt, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR,
    //                    rt);
    //                hr= m_pRenderTarget->EndDraw();
    //                ReleaseDC(mFileHwnd,hdc);
    //                InvalidateRect(mFileHwnd, NULL, FALSE);
    //                UpdateWindow(mFileHwnd);
    //                int a = 2;
    //            }

    //            SafeRelease(&pDecoder);
    //            SafeRelease(&pSource);
    //            SafeRelease(&pStream);
    //            SafeRelease(&pConverter);
    //            SafeRelease(&pScaler);

    //            return hr;
    //        }


    //    }
    //    MessageBox(hwnd, filePathAll.c_str(), L"asd", MB_OK);
    //}


    return 0;




    case WM_DESTROY://창이 파괴될때 전달되는 메시지
        PostQuitMessage(0);
        return 0;
    }

   // return DefWindowProc(hwnd, msg, wParam, lParam);
    return DefWindowProc(hwnd, msg, wParam, lParam);
}


//LRESULT Application::FileWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
//{
//    HDC hdc;
//    PAINTSTRUCT ps;
//
//    switch (msg)
//    {
//    case WM_DROPFILES:
//    {
//        int count = DragQueryFile((HDROP)wParam, 0xFFFFFFFF, nullptr, 0);
//        wchar_t filePath[MAX_PATH];
//        std::wstring filePathAll;
//        for (int i = 0; i < count; ++i)
//        {
//            int characterCount = DragQueryFile((HDROP)wParam, i, filePath, MAX_PATH);
//            //filePath[characterCount] = '\0';
//
//            if (!(GetFileAttributes(filePath) & FILE_ATTRIBUTE_DIRECTORY))
//            {
//                DragAndDropEvent* pEvent = new DragAndDropEvent(EventType::eDragAndDrop);
//                pEvent->SetFilePath(filePath);
//                mResourceController.AddEvent(pEvent);
//                filePathAll += filePath;
//            }
//
//        }
//        MessageBox(hwnd, filePathAll.c_str(), L"asd", MB_OK);
//
//    }
//        break;
//    case WM_PAINT:
//       /*hdc= BeginPaint(hwnd, &ps);
//        TextOut(hdc, 100, 100, _T("aaa"), _tcslen(_T("aaa")));
//        EndPaint(hwnd, &ps);*/
//
//        break;
//
//    case WM_SIZE:
//       /* if (m_pRenderTarget != NULL)
//        {
//           
//            InvalidateRect(hwnd, NULL, FALSE);
//        }*/
//        if (mDevice)
//        {
//            mUiRenderSystem.OnResize(mFileUiWindowWidth, mFileUiWindowHeight);
//            mFileUiSystem.Resize(mFileUiWindowWidth, mFileUiWindowHeight);
//
//
//        }
//          break;
//    case WM_CREATE:
//        
//        break;
//
//    case WM_DESTROY:
//        PostQuitMessage(0);
//        return 0;
//    }
//
//    return DefMDIChildProc(hwnd, msg, wParam, lParam);
//
//}

Application::Application()
{
    mEditObjectFactory = EditObjectFactory::GetInstance();


}

Application::~Application()
{
    FlushCommandQueue();//명령대기열에 명령이남아있는데 종료하게되면 gpu 가 충돌(crash)할수있다.
}


bool Application::Initialize(AppInitData& appInitData)
{
    //SetCurrentDirectory(_T("./SecenGraphQuadTree"));
    //TCHAR dir[MAX_PATH];
   // GetCurrentDirectory(MAX_PATH, dir);
   // OutputDebugString(dir);

    mHinstance = appInitData.hInstance;
    mShowcmd = appInitData.nShowCmd;
    //if (!InitFrameWindow())
    //    return false;

    if (!InitD3d())
        return false;




    mProgramDirector = appInitData.programDirector;

  //  mRenderWindowTest = new  GameRenderWindow(mHinstance);
   // mAttributeWindow = new AttributeWindow(mHinstance);
    
    mCollisionWorldFactory = new CollisionWorldFactory(appInitData.collisionWorldFactoryImpl);


    m3DSpacePartitioningStructureFactory = SpacePartitioningStructureFactory<Collider>::GetInstance(appInitData.spacePartitoingStructureFactoryImpl);
        
        
    
  

    mUiSpacePartitioningStructureFactory = SpacePartitioningStructureFactory<UiCollider>::GetInstance(appInitData.spacePartitoingStructureFactoryImpl);

 
    mGraphicCommandObject.ResetCommandList(nullptr);

   // HarfBuzzTest();





     InitSystems();
    



   return true;
}

int Application::Run()
{
    MSG msg = { 0 };
    
    mGameTimer.Reset();


    while(msg.message != WM_QUIT)
    {
        if (msg.message == WM_QUIT)
        {
            break;
        }
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            mGameTimer.Tick();
            if (!mIspaused)
            {
                CalculateFrameStats();
                //게임코드
                PreUpdate(mGameTimer);
                Update(mGameTimer);
                EndUpdate(mGameTimer);

                Draw(mGameTimer);
            }
            else
            {
                Sleep(100);
            }
        }
    }


    return (int)msg.wParam;
}



//void Application::InitFrameWindow()
//{
//    
//    mFrameWindow = new FrameWindow(mHinstance);
//
//    //  Controller* controller = new MapController;// new Controller;
//    FrameWindowController* wController = new FrameWindowController;// new Controller;
//    mFrameWindowController = wController;
//
//    UINT clientWidth = mFrameWindow->GetClientWidth();
//    UINT clientHeight = mFrameWindow->GetClientHeight();
//
//    //renderSystem;
//    RenderSystem* renderSystem = new RenderSystem;
//    renderSystem->Initialize(mDevice,
//        mFactory, &mGraphicCommandObject, mFrameWindow->GetWindowHandle(),
//        clientWidth, clientHeight, &mDescriptorHeapManagerMaster, FRAMEWINDOW);
//
//    AddEffect(renderSystem, EffectManager::GetEffect("Default.effect"));
//    AddEffect(renderSystem, EffectManager::GetEffect("DefaultUi.effect"));
//    AddEffect(renderSystem, EffectManager::GetEffect("WindowLayout.effect"));
//    AddEffect(renderSystem, EffectManager::GetEffect("TextBox.effect"));
//    AddEffect(renderSystem, EffectManager::GetEffect("TextCharacter.effect"));
//    AddEffect(renderSystem, EffectManager::GetEffect("DefaultCollider.effect"));
//
//   // EffectManager::GetEffect("Default.effect");
//
//
//
//
//
//
//
//    wController->SetName("FrameWindowController");
//
//    Controller::AddController("FrameWindowController", wController);
//    //ui System;
//    FrameWindowUiSystem* uiSystem = new FrameWindowUiSystem;
//    mFrameWindowUiSystem = uiSystem;    //ProjectDirector초기화를위해
//    FrameWindowSystem* worldSystem = new FrameWindowSystem;
//
//    WindowChromeSystem* windowLayoutSystem = new WindowChromeSystem(ESystemID::eFrameLayoutSystem);
//    FrameWindowDockingSystem* windowDockingSystem = new FrameWindowDockingSystem;
//
//  
//
//    wController->Initialize(mDevice, mGraphicCommandObject.GetGraphicsCommandList(), &mMeshManager, &mMaterialManager, &mTextureManager,
//        &mMapManager, mFrameWindow, renderSystem, uiSystem, worldSystem, windowLayoutSystem,
//        windowDockingSystem, &mDescriptorHeapManagerMaster);
//
//    wController->SetWindowActiveFlag(true);
//
//    uiSystem->SetController(wController);
//    worldSystem->SetController(wController);
//    windowLayoutSystem->SetController(wController);
//    windowDockingSystem->SetController(wController);
//
//    InitFrameWindowSystem(mFrameWindow, worldSystem);
//    InitFrameWindowUiSystem(mFrameWindow, uiSystem);
//    InitFrameWindowLayoutSystem(mFrameWindow, windowLayoutSystem);
//    InitFrameWindowDockingSystem(mFrameWindow, windowDockingSystem);
//
//
//    mFrameWindow->Initialize(wController);
//
//    mFrameWindow->SetProjectSelectSceneFlag(true);
//    uiSystem->SetActiveState(true);
//    worldSystem->SetActiveState(true);
//   windowLayoutSystem->SetActiveState(true);
// //   windowDockingSystem->SetActiveState(false);
//
//
//    
//
//
//   // wController->SetTitleBarSize(true, 3000, 60);
//    
//
//    wController->InitChildWindowSetting(mRenderWindowTest, mAttributeWindow,mFileUiWindow);
//    mFrameWindow->OnResize(clientWidth, clientHeight, 0);
//
//    mEventDispatcher.RegisterSystem(worldSystem);
//    mEventDispatcher.RegisterSystem(uiSystem);
//    mEventDispatcher.RegisterSystem(windowLayoutSystem);
//    mEventDispatcher.RegisterSystem(windowDockingSystem);
//
//
//
//
//
//    mFrameWindowMenuDirector.Initialize(windowLayoutSystem);
//
//}
//
//void Application::InitDragAndDropWindow()
//{
//
//
//    //mDragAndDropWindow = new DragAndDropWindow(mHinstance,;
//
//
//
//    DragAndDropWindowController* controller = new DragAndDropWindowController;
//  
//
//
//    DragAndDropSystem* uiSystem = new DragAndDropSystem;
//
//
//    Map* map = EditorSystem::CreateMap(uiSystem, "DragAndDropWindowMainMap", false, false);
//    //map->SetName("DragAndDropWindowMainMap");
//  //  map->Initialize(uiSystem);
//
//    VectorSpace* vectorSpace = new VectorSpace;
//    vectorSpace->Initialize(600);
//    
//    CollisionWorld* collisionWorld = new CollisionWorld(vectorSpace);
//
//    map->CreateMapLayer(0, 0, nullptr, collisionWorld, { 0,0,(float)300, (float)100 ,0.0f,1.0f });
//   
//    
//    uiSystem->Initialize(300, 100, map);
//
//    Map* currMap = uiSystem->GetMap();
//
// //   OrthogoanlCamera* camera = static_cast<OrthogoanlCamera*>(currMap->CreateObject("OrthogoanlCamera"));
//    OrthogoanlCamera* camera = OrthogoanlCamera::Create(map, 0, 300, 100);
//
//  //  camera->Initialize(300, 100);
//    currMap->SetMainCamera(camera);
//
//    mDragAndDropWindow = new DragAndDropWindow(mHinstance);
//    mDragAndDropWindow->Initialize(controller, 300, 100);
//
//    mDragAndDropRenderSystem.Initialize(mDevice, mFactory,
//        &mGraphicCommandObject, mDragAndDropWindow->GetWindowHandle(), 300, 100, &mDescriptorHeapManagerMaster, DRAGANDDROPWINDOW);
//
//
//    AddEffect(&mDragAndDropRenderSystem, EffectManager::GetEffect("Default.effect"));
//    AddEffect(&mDragAndDropRenderSystem, EffectManager::GetEffect("DefaultUi.effect"));
//    AddEffect(&mDragAndDropRenderSystem, EffectManager::GetEffect("WindowLayout.effect"));
//    AddEffect(&mDragAndDropRenderSystem, EffectManager::GetEffect("TextBox.effect"));
//    AddEffect(&mDragAndDropRenderSystem, EffectManager::GetEffect("TextCharacter.effect"));
//    AddEffect(&mDragAndDropRenderSystem, EffectManager::GetEffect("DefaultCollider.effect"));
//
//    
//
//
//
//
//    //드래그앤드랍윈도우에대한 고유의 시스템이 필요하다.
//
//
//    controller->Initialize(mDragAndDropWindow, uiSystem,&mDragAndDropRenderSystem);
//
//
//    mEventDispatcher.RegisterSystem(uiSystem);
//
//}
//
//void Application::InitPopupWindow()
//{
//
//
//
//
//    PopupWindowUiSystem* uiSystem = new PopupWindowUiSystem;
//    Map* map = EditorSystem::CreateMap(uiSystem, "PopupUiSystem", false, false);
//   // map->Initialize(uiSystem);
//
//    VectorSpace* vectorSpace = new VectorSpace;
//    vectorSpace->Initialize(10000);
//    CollisionWorld* collisionWorld = new CollisionWorld(vectorSpace);
//    
//    map->CreateMapLayer(0, 0, nullptr, collisionWorld, { 0,0,(float)300, (float)400 ,0.0f,1.0f });
//
// //   Camera* camera = new PopupSystemCamera(L"PopupSystemCamera");
//
//
//
//    PopupWindowController* controller = new PopupWindowController;
//    mPopupWindowController = controller;
//
//
//    mPopupWindow = new PopupWindow(mHinstance, 300, 400, false);
//    mPopupWindow->Initialize(controller);
//
//    uiSystem->Initialize(300, 400, map);
//    uiSystem->SetController(controller);
//
//    Map* currMap = uiSystem->GetMap();
//
//
//   // PopupSystemCamera* camera = static_cast<PopupSystemCamera*>(currMap->CreateObject("PopupSystemCamera"));
//    PopupSystemCamera* camera = PopupSystemCamera::Create(currMap, 0, 300, 400);
//  //  camera->Initialize(300, 400);
//    camera->SetPosition(0, 0,0);
//    currMap->SetMainCamera(camera);
//
//    mPopupRenderSystem.Initialize(mDevice, mFactory, &mGraphicCommandObject, mPopupWindow->GetWindowHandle(), 300, 400, &mDescriptorHeapManagerMaster, POPUPWINDOW);
//
//    mPopupRenderSystem.SetBackgroundColor(0.2f, 0.2f, 0.2f, 1.0f);
//    controller->Initialize(mPopupWindow, uiSystem, &mPopupRenderSystem);
//
//  //  AddEffect(&mPopupRenderSystem, EffectTable::GetEffect(L"Default.effect"));
//    AddEffect(&mPopupRenderSystem, EffectManager::GetEffect("DefaultUi.effect"));
//   // AddEffect(&mPopupRenderSystem, EffectTable::GetEffect(L"WindowLayout.effect"));
//    AddEffect(&mPopupRenderSystem, EffectManager::GetEffect("TextBox.effect"));
//    AddEffect(&mPopupRenderSystem, EffectManager::GetEffect("TextCharacter.effect"));
//   // AddEffect(mPopupRenderSystem, EffectTable::GetEffect(L"DefaultCollider.effect"));
//
//}

bool Application::InitD3d()
{
#if defined(DEBUG)||defined(_DEBUG)
    //D3D12 디버그층 활성화 
    {
        ComPtr<ID3D12Debug> debugController;
        ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))); //&debugController을 사용하면 Comptr과 연관된 인터페이스를 해제후 comptr 객체의 주소를 리턴한다.
                                                                              //GetAddressOf는 인터페이스를 참조하는 포인터의 주소를 리턴한다. //둘다 2번씩 참조하면 인터페이스를 나타내는다는것은 똑같다.
        debugController->EnableDebugLayer();//디버그계층활성화

        //디버그계층활성화는 장치생성전에 수행해야한다.
        //그렇지 않으면 생성한 장치가 제거된다.
    }
#endif

    ThrowIfFailed( CreateDXGIFactory1(IID_PPV_ARGS(&mFactory)));

    HRESULT hardwareResult = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&mDevice));


    if (FAILED(hardwareResult))
    {
        ComPtr<IDXGIAdapter> pWarpAdapter;
      ThrowIfFailed(mFactory->EnumWarpAdapter(IID_PPV_ARGS(&pWarpAdapter)));//소프트웨어 어댑터를 제공한다.

       ThrowIfFailed(D3D12CreateDevice(pWarpAdapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&mDevice)));
        //실패하면 종료 
    }

    ThrowIfFailed(mDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFence)));

    mRtvdescriptorSize = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    mDsvdescriptorSize = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
    mCbvsrvdescriptorSize = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    mSamplerdescriptorSize = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);



    D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
    msQualityLevels.Format = mBackBufferForamt;
    msQualityLevels.SampleCount = 4;
    msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
    msQualityLevels.NumQualityLevels = 0;

    ThrowIfFailed(mDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &msQualityLevels, sizeof(msQualityLevels)));
    
    m4xmsaaQuality = msQualityLevels.NumQualityLevels;

    assert(m4xmsaaQuality > 0 && " Unexpected MSAA quality level.");


    //순서 중요
    CreateCommandObjects();
    
 








   // CreateSwapChain();
   // CreateDescriptorHeaps();
  //  CreateSwapChainRtv();
  //  CreateSwapchainDsv();

    //mClientViewPort.TopLeftX = 0;
    //mClientViewPort.TopLeftY = 0;
    //mClientViewPort.Width =(float) mClientWidth;
    //mClientViewPort.Height = (float)mClientHeight;
    //mClientViewPort.MinDepth = 0.0f;
    //mClientViewPort.MaxDepth = 1.0f;
    ////m_graphicscommandList->RSSetViewports(1, &m_clientViewPort);

    //mScissorRect.left = 0;
    //mScissorRect.right = (LONG)mClientWidth;
    //mScissorRect.top = 0;
    //mScissorRect.bottom = (LONG)mClientHeight;
    //m_graphicscommandList->RSSetScissorRects(1, &m_scissorRect);


  /*  ThrowIfFailed(mGraphicscommandList->Close());
    ID3D12CommandList* commandList[] = { mGraphicscommandList.Get() };
    mCommandQueue->ExecuteCommandLists(_countof(commandList), commandList);

	FlushCommandQueue();*/
	//OnResize();
    //commandlist에서 reset을호출하기위해서는 닫혀있어야한다.
     return true;
}

void Application::InitSystems()
{
    InitCommonSystems();

    if (!mGraphicCommandObject.GetCloseState())
    {
        mGraphicCommandObject.ExecuteCommandList();
        mGraphicCommandObject.FlushCommandQueue();
    }
        mGraphicCommandObject.ResetCommandList(nullptr);
    

    mResourceController.Initialize(mDevice, &mGraphicCommandObject, &mMeshManager, &mMaterialManager, &mTextureManager,
        &mMapManager, &mRenderSystem, &mResourceLoader, &mResourceStorer,nullptr, nullptr, &mDescriptorHeapManagerMaster);

    mMapController.Initialize(&mRenderSystem, &mMeshManager, &mMapManager);
 
    Controller::AddController("resourceController", &mResourceController);
    Controller::AddController("mapController", &mMapController);

    if (!mGraphicCommandObject.GetCloseState())
    {
        mGraphicCommandObject.ExecuteCommandList();
        mGraphicCommandObject.FlushCommandQueue();
    }


   // mResourceLoader.LoadProjectData();

 //   SpacePartitioningStructureFactory<UiCollider>::GetInstance();
    mProgramDirector->Initialize();








   // InitFileUiWindow();
   //// mResourceController.LoadFile(L"Resource\\black_bison.fbx");
   //// mResourceController.LoadFile(L"Resource\\chair.fbx");
   //// mResourceController.LoadFile(L"C:\\Users\\dongd\\source\\repos\\SecenGraphQuadTree\\SecenGraphQuadTree\\Resource\\cone.fbx");

   // InitGamePlayWindow();   //game play window 라고하는데 동시에 월드 에디터이기도한거지
   // InitAttributeWindow();

   // InitFrameWindow();
   //
   // InitDragAndDropWindow();
   // InitPopupWindow();


   // if (!mGraphicCommandObject.GetCloseState())
   // {
   //     mGraphicCommandObject.ExecuteCommandList();
   //     mGraphicCommandObject.FlushCommandQueue();
   // }


  //  mResourceController.SendDefaultAssetLoadResponseEvent();
 
  //  std::vector<DockingWindowController*>normalWindowControllerVector = { mGameWindowPlayController,mFileUiWindowController,mAttributeWindowController };


  ////  mCreatingProjectDirector.Initialize(mFrameWindow, std::move(normalWindowControllerVector), mPopupWindowController, mFrameWindowController);






//#ifdef EditorMode
//    mEditorModeDirector.Initialize(mGamePlayWindowChromeSystem);
//    mEditorModeDirector.InitGameWindowLayoutSystem();
//
//
//    EditorModeDirector::SendAssetToFileSystem();
//#endif 


    ////여기까지는 에디터를 위한내용
    //mProjectDirector.Initialize(mFrameWindow, std::move(normalWindowControllerVector), mPopupWindowController, mFrameWindowController,mFrameWindowUiSystem);
    //


    //user data처리









   return ;
}

void Application::InitCommonSystems()
{
    D3D12_DESCRIPTOR_HEAP_DESC cbvSrvUavHeapDesc;
    cbvSrvUavHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    cbvSrvUavHeapDesc.NodeMask = 0;
    cbvSrvUavHeapDesc.NumDescriptors = VIEW_MAXNUM;
    cbvSrvUavHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

    mCbvSrvUavHeapManager.Initialize(mDevice, mCbvsrvdescriptorSize, cbvSrvUavHeapDesc);

    D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
    dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    dsvHeapDesc.NodeMask = 0;
    dsvHeapDesc.NumDescriptors = VIEW_MAXNUM;
    dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;

    mDsvHeapManager.Initialize(mDevice, mDsvdescriptorSize, dsvHeapDesc);


    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    rtvHeapDesc.NodeMask = 0;
    rtvHeapDesc.NumDescriptors = VIEW_MAXNUM;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

    mRtvHeapManager.Initialize(mDevice, mDsvdescriptorSize, rtvHeapDesc);


    D3D12_DESCRIPTOR_HEAP_DESC samplerHeapDesc;
    samplerHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    samplerHeapDesc.NodeMask = 0;
    samplerHeapDesc.NumDescriptors = 30;
    samplerHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;

    mSamplerHeapManager.Initialize(mDevice, mDsvdescriptorSize, samplerHeapDesc);





    mDescriptorHeapManagerMaster.Initialize(&mCbvSrvUavHeapManager, &mDsvHeapManager, &mRtvHeapManager, &mSamplerHeapManager);

    mMeshManager.Initialize(&mDescriptorHeapManagerMaster);

    mMaterialManager.Initialize();


    mTextureManager.Initialize(mDevice,&mDescriptorHeapManagerMaster,&mGraphicCommandObject);

    mMapManager.Initialize();

    mResourceLoader.Initialize(mDevice, &mGraphicCommandObject,&mDescriptorHeapManagerMaster);
    mResourceStorer.Initialize();


    mTextFactory.initialize(mDevice, &mGraphicCommandObject, &mTextureManager, &mDescriptorHeapManagerMaster);

   // mTextFactory.ReadFontFile(L"C:\\Users\\dongd\\source\\repos\\SecenGraphQuadTree\\SecenGraphQuadTree\\Font\\DefaultFont.ttf");
//    mTextFactory.ReadFontFile(L".\\Font\\DefaultFont.ttf");
    mTextFactory.ReadFontFile(L"C:\\Users\\dongd\\gitproject\\GameEngine\\Include\\Font\\DefaultFont.ttf");

    mLineFactory.Initialize(mDevice);


    mKeyBoard.Initialize();
    mMouse.Initlaize(mHinstance);

    mCollisionHelper.Initialize();
    mColliderGenerator.Initialize(mDevice, &mGraphicCommandObject);

    mEventDispatcher.Initialize();



   // mTextureFactory.Initialize(mDevice, &mDescriptorHeapManagerMaster, &mGraphicCommandObject);
}

//void Application::InitGamePlayWindow()
//{
//
//    UINT gameWindowClientWidth = mRenderWindowTest->GetClientWidth();
//    UINT gameWindowClientHeight = mRenderWindowTest->GetClientHeight();
//    HWND gameWindowHandle = mRenderWindowTest->GetWindowHandle();
//
//    
//    //render system 
//    RenderSystem* renderSystem = new RenderSystem;
//    renderSystem->Initialize(mDevice,mFactory,&mGraphicCommandObject,
//        gameWindowHandle, gameWindowClientWidth, gameWindowClientHeight, &mDescriptorHeapManagerMaster,GAMEWINDOW);
//
//
//    AddEffect(renderSystem, EffectManager::GetEffect("Default.effect"));
//    AddEffect(renderSystem, EffectManager::GetEffect("DefaultUi.effect"));
//    AddEffect(renderSystem, EffectManager::GetEffect("WindowLayout.effect"));
//    AddEffect(renderSystem, EffectManager::GetEffect("TextBox.effect"));
//    AddEffect(renderSystem, EffectManager::GetEffect("TextCharacter.effect"));
//    AddEffect(renderSystem, EffectManager::GetEffect("Line.effect"));
//    AddEffect(renderSystem, EffectManager::GetEffect("DefaultCollider.effect"));
//    AddEffect(renderSystem, EffectManager::GetEffect("GizmoLine.effect"));
//    AddEffect(renderSystem, EffectManager::GetEffect("Gizmo.effect"));
//    AddEffect(renderSystem, EffectManager::GetEffect("GizmoRotation.effect"));
//    AddEffect(renderSystem, EffectManager::GetEffect("Docking.effect"));
//
//
//
//    renderSystem->SetColliderWorldRenderState(true);
//
//
//
//
//
//
//
//    //System을 초기화하면서 맵들, 그 맵에있는엔티티들,등등을 로드하고 초기화한다.
//        //Controller
//    GamePlayWindowController* wcontroller = new GamePlayWindowController();
//    mGameWindowPlayController = wcontroller;
//    wcontroller->SetName("gameWindowPlayController");
//    Controller::AddController("gameWindowPlayController", wcontroller);
//
//    GamePlaySystem* gamePlaySystem = new GamePlaySystem;
//    GamePlayUiSystem* gamePlayUiSystem = new GamePlayUiSystem;
//    WindowChromeSystem* windowLayoutSystem = new WindowChromeSystem(ESystemID::eGamePlayLayoutSystem);
//    mGamePlayWindowChromeSystem = windowLayoutSystem;
//    ChildWindowDockingSystem* windowDockingSystem = new ChildWindowDockingSystem(ESystemID::eGamePlayDockingSystem);
//
//
//    wcontroller->Initialize(mDevice, mGraphicscommandList, &mMeshManager,
//        &mMaterialManager, &mTextureManager, &mMapManager, mRenderWindowTest, renderSystem, gamePlayUiSystem,
//        gamePlaySystem, windowLayoutSystem, windowDockingSystem, &mDescriptorHeapManagerMaster, false);
//
//    gamePlaySystem->SetController(wcontroller);
//    gamePlayUiSystem->SetController(wcontroller);
//    windowLayoutSystem->SetController(wcontroller);
//    windowDockingSystem->SetController(wcontroller);
//
//    wcontroller->SetWindowActiveFlag(false);
//
//
//    InitGameProject(mRenderWindowTest, wcontroller,gamePlaySystem, gamePlayUiSystem, windowLayoutSystem,
//        windowDockingSystem);
//
//
// 
//
//
//    //유일하게 gameWindow만 false로 시작한다.
//    //다른 window들은 항상 play mode이기때문에(true) //그리고 이미 ui,main system은 false로 설정되었다 (내부초기화때문에)
//    //일단 컨트롤러에서 직접 상태를 가져오는 일이 없는거같지만 그래도 controller와 동기화해주자
//    wcontroller->SetPlayMode(false);
//
//
//    wcontroller->SetTitleBarSize(true, 3000, 40);
//    gamePlayUiSystem->SetDefaultController("gameWindowPlayController");
//
//    mRenderWindowTest->Initialize(wcontroller);
//
//
//
//    mEventDispatcher.RegisterSystem(gamePlaySystem);
//    mEventDispatcher.RegisterSystem(gamePlayUiSystem);
//    mEventDispatcher.RegisterSystem(windowLayoutSystem);
//    mEventDispatcher.RegisterSystem(windowDockingSystem);
//
//
//
//
//
//
//
//  //  CreateInitGameWindowEntity(gamePlaySystem);
//
//}
//
//void Application::InitFileUiWindow()
//{
//
//    mFileUiWindow = new FileUiWindow(mHinstance);
//    //window controller;
//    //  Controller* controller = new MapController;// new Controller;
//    FileUiWindowContoller* wController = new FileUiWindowContoller;// new Controller;
//    mFileUiWindowController = wController;
//    //rendersystem
//
//    //3d,ui,chrome system
//
//    UINT clientWidth = mFileUiWindow->GetClientWidth();
//    UINT clientHeight = mFileUiWindow->GetClientHeight();
//
//    //renderSystem;
//    RenderSystem* renderSystem = new RenderSystem;
//    renderSystem->Initialize(mDevice,
//        mFactory, &mGraphicCommandObject, mFileUiWindow->GetWindowHandle(),
//        clientWidth, clientHeight, &mDescriptorHeapManagerMaster, FILEUIWINDOW);
//
//
//    AddEffect(renderSystem, EffectManager::GetEffect("Default.effect"));
//    AddEffect(renderSystem, EffectManager::GetEffect("DefaultUi.effect"));
//    AddEffect(renderSystem, EffectManager::GetEffect("WindowLayout.effect"));
//    AddEffect(renderSystem, EffectManager::GetEffect("TextBox.effect"));
//    AddEffect(renderSystem, EffectManager::GetEffect("TextCharacter.effect"));
//    AddEffect(renderSystem, EffectManager::GetEffect("DefaultCollider.effect"));
//    AddEffect(renderSystem, EffectManager::GetEffect("Docking.effect"));
//
//  //  SetCurrentDirectory(beforeDirectory);
//
//
//
//
//
//
//
//    wController->SetName("FileUiWindowController");
//    Controller::AddController("FileUiWindowController", wController);
//
//    //ui System;
//    FileUiSystem* mainSystem = new FileUiSystem;
//    FileUiUiSystem* uiSystem = new FileUiUiSystem;
//    WindowChromeSystem* windowLayoutSystem = new WindowChromeSystem(ESystemID::eFileUiLayoutSystem);
//    ChildWindowDockingSystem* windowDockingSystem = new ChildWindowDockingSystem(ESystemID::eFileUiDockingSystem);
//    uiSystem->SetController(wController);
//    mainSystem->SetController(wController);
//    windowLayoutSystem->SetController(wController);
//    windowDockingSystem->SetController(wController);
//    windowDockingSystem->SetController(wController);
//
//    wController->Initialize(mDevice, mGraphicCommandObject.GetGraphicsCommandList(), &mMeshManager, &mMaterialManager, &mTextureManager,
//        &mMapManager, mFileUiWindow, renderSystem, uiSystem, mainSystem, windowLayoutSystem,
//        windowDockingSystem ,&mDescriptorHeapManagerMaster);
//   
//    mFileUiWindow->Initialize(wController);
//
//    InitFileUiUiSystem(mFileUiWindow, uiSystem);
//    InitFileUiSystem(mFileUiWindow, mainSystem);
//    InitFileUiWindowLayoutSystem(mFileUiWindow, windowLayoutSystem);
//    InitChildWindowDockingSystem(mFileUiWindow, windowDockingSystem);
//
//
//    wController->SetTitleBarSize(true, 3000, 40);
//    wController->SetWindowActiveFlag(false);
//
//    mEventDispatcher.RegisterSystem(mainSystem);
//    mEventDispatcher.RegisterSystem(uiSystem);
//    mEventDispatcher.RegisterSystem(windowLayoutSystem);
//    mEventDispatcher.RegisterSystem(windowDockingSystem);
//
//
//    //윈도우생성
//    //윈도우컨트롤러 생성 
//    //시스템들을 생성
//
//    //윈도우 컨트롤러 초기화 ( 시스템 패싱)
//    //컨트롤러 초기화에서 시스템 초기화수행
//    //맵은 없는상태
//
//
//     
//    //그이후 맵추가 
//    //(맵추가할때 카메라등등처리)
//
//}
//
//void Application::InitAttributeWindow()
//{
//  //  Controller* controller = new MapController;// new Controller;
//    DockingWindowController* wController = new DockingWindowController;// new Controller;
//    mAttributeWindowController = wController;
//
//    UINT clientWidth = mAttributeWindow->GetClientWidth();
//    UINT clientHeight = mAttributeWindow->GetClientHeight();
//
//    //renderSystem;
//    RenderSystem* renderSystem = new RenderSystem;
//    renderSystem->Initialize(mDevice,
//        mFactory, &mGraphicCommandObject, mAttributeWindow->GetWindowHandle(),
//        clientWidth, clientHeight, &mDescriptorHeapManagerMaster,ATTRIBUTEWINDOW);
//
//
//
//    AddEffect(renderSystem, EffectManager::GetEffect("Default.effect"));
//    AddEffect(renderSystem, EffectManager::GetEffect("DefaultUi.effect"));
//    AddEffect(renderSystem, EffectManager::GetEffect("WindowLayout.effect"));
//    AddEffect(renderSystem, EffectManager::GetEffect("TextBox.effect"));
//    AddEffect(renderSystem, EffectManager::GetEffect("TextCharacter.effect"));
//    AddEffect(renderSystem, EffectManager::GetEffect("DefaultCollider.effect"));
//    AddEffect(renderSystem, EffectManager::GetEffect("Line.effect"));
//    AddEffect(renderSystem, EffectManager::GetEffect("ScrollListPanelUi.effect"));
//    AddEffect(renderSystem, EffectManager::GetEffect("ScrollListChildPanelUi.effect"));
//    AddEffect(renderSystem, EffectManager::GetEffect("Docking.effect"));
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//    wController->SetName("AttributeWindowController");
//    Controller::AddController("AttributeWindowController", wController);
//    //ui System;
//    AttributeUiSystem* uiSystem = new AttributeUiSystem;
//    AttributeSystem* worldSystem = new AttributeSystem;
//    WindowChromeSystem* windowLayoutSystem = new WindowChromeSystem(ESystemID::eAttributeLayoutSystem);
//    ChildWindowDockingSystem* windowDockingSystem = new ChildWindowDockingSystem(ESystemID::eAttributeDockingSystem);
//
//
//    mAttributeWindow->Initialize(wController);
//    uiSystem->SetController(wController);
//    worldSystem->SetController(wController);
//    windowLayoutSystem->SetController(wController);
//    windowDockingSystem->SetController(wController);
//    windowDockingSystem->SetController(wController);
//
//
//
//    wController->Initialize(mDevice, mGraphicCommandObject.GetGraphicsCommandList(), &mMeshManager, &mMaterialManager, &mTextureManager,
//        &mMapManager, mAttributeWindow, renderSystem, uiSystem, worldSystem, windowLayoutSystem, windowDockingSystem,
//        &mDescriptorHeapManagerMaster);
//
//    InitAttributeSystem(mAttributeWindow, worldSystem);
//    InitAttributeUiSystem(mAttributeWindow, uiSystem);
//    InitAttributeWindowLayoutSystem(mAttributeWindow, windowLayoutSystem);
//    InitChildWindowDockingSystem(mAttributeWindow, windowDockingSystem);
//
//
//
//    wController->SetWindowActiveFlag(false);
//
//   
// 
//    
//    wController->SetTitleBarSize(true, 3000, 40);
//    mEventDispatcher.RegisterSystem(worldSystem);
//    mEventDispatcher.RegisterSystem(uiSystem);
//    mEventDispatcher.RegisterSystem(windowLayoutSystem);
//    mEventDispatcher.RegisterSystem(windowDockingSystem);
//  
//
//
//
//}

//void Application::InitGameProject(TaskWindow * window , DockingWindowController* controller , 
//    GamePlaySystem * gamePlaySystem , GamePlayUiSystem * gamePlayUiSystem ,
//    WindowChromeSystem * windowLayoutSystem, ChildWindowDockingSystem* windowDockingSystem)
//{
//
//    //entity type  sch load
//    //mesh ,materal ,texture load 
//    
//    std::wstring meshFilePath = L"C:\\Users\\dongd\\source\\repos\\SecenGraphQuadTree\\SecenGraphQuadTree\\Project\\Mesh\\*.mesh";
//    WIN32_FIND_DATA fileData;
//    HANDLE handle;
//    //load mesh 
//    handle = FindFirstFile(meshFilePath.c_str(), &fileData);
//    if (handle == INVALID_HANDLE_VALUE)
//    {
//            
//
//    }
//    else
//    {
//
//   /*     mResourceController.CreateNewMesh(fileData.cFileName);
//        while (FindNextFile(handle, &fileData) != 0)
//        {
//            mResourceController.CreateNewMesh(fileData.cFileName);
//        }*/
//
//    }
//    FindClose(handle);
//
//    //load texture
//    //load material
//    //load shader
//    
//
//    //entity
//
//
//
//
//    InitGamePlayUiSystem(window,gamePlayUiSystem);
//    InitGamePlaySystem(window,gamePlaySystem);
//    InitGamePlayWindowLayoutSystem(window, windowLayoutSystem);
//    InitChildWindowDockingSystem(window, windowDockingSystem);
//
//    gamePlayUiSystem->SetController(controller);
//    gamePlaySystem->SetController(controller);
//    windowLayoutSystem->SetController(controller);
//    windowDockingSystem->SetController(controller);
//
//
//
// }
//
//void Application::InitGamePlaySystem(TaskWindow* window,GamePlaySystem* system)
//{
//    //유저가 생성한 map들의 정보를 로드하고 
//    //start point로 지정된map과 맵 그래프에서 그 start point맵과 연결된 맵들도 로드 한다.
//    //또한 그 맵들에 존재하는 엔티티들도 로드
//    //1차적으로 연결되지않는 맵들은 정보만 로드
//
//
//    //project파일이 있어야하고
//    //project파일안에는 start point map이있어야돼고 또 엔티티타입들이 있어야돼고 
//    //map 그래프정보가있어야돼 
//    //일단 project에있는 맵정보는 다로드해
//    //map그래프를보고 가시적인 map들에대해서만 ,entity를 메모리로 로드한다.
//    //
//   
//
//
//
//    //map.- scenegraph.spatialSpace
//
//
////    VectorSpace* vectorSpace = new VectorSpace;
////    vectorSpace->Initialize(10000);
////
////    CollisionWorld* collisionInitWorld = new CollisionWorld(vectorSpace);
////
////    vectorSpace = new VectorSpace;
////    vectorSpace->Initialize(10000);
////
////    CollisionWorld* collisionRunTimeWorld = new CollisionWorld(vectorSpace);
////
////
////    Map* map = EditorSystem::CreateMap(system, "GamePlayMainMap", false, true);
////
////    map->CreateMapLayer(0, 0, nullptr, collisionRunTimeWorld, { 0,0,(float)window->GetClientWidth(), (float)window->GetClientHeight() ,0.0f,1.0f });
//// 
//////    GamePlayWindowCamera* gameCamera = static_cast<GamePlayWindowCamera*>(map->CreateObject("GamePlayWindowCamera"));
////    GamePlayWindowCamera* gameCamera = GamePlayWindowCamera::Create(map, 0, DirectX::XM_PI / 2, (float)window->GetClientWidth() / window->GetClientHeight());
////
////
////    map->SetMainCamera(gameCamera);
////
//
//
//
//
//   // gameCamera->SetEngineObjectFlag(true);
//
//
//   // system->Initialize(window->GetClientWidth(), window->GetClientHeight(), map);
//    system->Initialize(window->GetClientWidth(), window->GetClientHeight(), nullptr);
//   //system->SetPlayModeState(false);
//
//  // map->GetMainCamera()->GetTransform().SetPositionLocal({ 0,0,-10.0f });
//
//
//
//
//
// 
//
//}
//
//void Application::InitGamePlayUiSystem(TaskWindow * window ,GamePlayUiSystem* system)
//{
//
//  
//
// //   VectorSpace* vectorSpace = new VectorSpace;
// //   vectorSpace->Initialize(10000);
// //   CollisionWorld* collisionInitWorld = new CollisionWorld(vectorSpace);
//
//
// //   vectorSpace = new VectorSpace;
// //   vectorSpace->Initialize(10000);
// //   CollisionWorld* collisionRunTimeWorld = new CollisionWorld(vectorSpace);
//
//
//
//
// //   Map* map = EditorSystem::CreateMap(system, "GamePlayUiMainMap", false,true);
// //  // map->SetName("GamePlayUiMainMap");
// ////   map->Initialize(system);
// //   map->CreateMapLayer(0, 0, nullptr, collisionRunTimeWorld, { 0,0,(float)window->GetClientWidth(), (float)window->GetClientHeight() ,0.0f,1.0f });
// // //  OrthogoanlCamera* gameCamera = static_cast<OrthogoanlCamera*>(map->CreateObject("OrthogoanlCamera"));
// //   OrthogoanlCamera* gameCamera = OrthogoanlCamera::Create(map, 0, window->GetClientWidth(), window->GetClientHeight());
// //  // gameCamera->Initialize(window->GetClientWidth(), window->GetClientHeight());  
// //   map->SetMainCamera(gameCamera);
//
// //   system->Initialize(window->GetClientWidth(), window->GetClientHeight(),map);
//
//
//
//
//
//
//
//    system->Initialize(window->GetClientWidth(), window->GetClientHeight(),nullptr);
//
//
//   // system->SetPlayModeState(false);
//    //
//    //ButtonUiEntitiy* button = new ButtonUiEntitiy;
//    //button->Initialize(L"button", MeshManager::GetMesh(L"Rectangle"));
//    //button->GetModel().SetTextureToAllSubMesh(TextureManager::GetTexture(L"Button.png"));
//    //button->GetTransform().SetScaleLocal({ 100.0f,100.0f,1.0f });
//    //button->GetTransform().SetPositionLocal({ 0.0F,0.0F,20.0F });
//    //button->GetModel().GetSubMesh()[0].mMateiral->SetEffectName(L"DefaultUi.effect");
//
//    //system->AddEntity(button);
//
//
//    //Entity* heroOne = new Entity;
//    //heroOne->Initialize(L"hereOne", MeshManager::GetMesh(L"black_Bison"));
//
//    //heroOne->GetTransform().SetPositionLocal({ 0.0F,-100.0F,100.0F });
//    //heroOne->GetTransform().SetScaleLocal({ 10.0F,10.0F,10.0F });
//    //system->AddEntity(heroOne);
//
//    //Text* textBox = new Text(L"text");
//    //textBox->SetSystem(system);
//    //textBox->Initialize(200, 50);
//    //system->AddEntity(textBox);
//    //textBox->SetText(L"test입니다.");
//    //textBox->SetSelectAvailableFlag(false);
//    //textBox->SetTexture(L"Black.png");
//    
//
//}
//
//void Application::InitAttributeSystem(TaskWindow* window,AttributeSystem* system)
//{
//    //game play system말고 다른 시스템들은 
//    // 일단 map이 하나뿐이다 라고 간소화하자.
//
//   
//
//    VectorSpace* vectorSpace = new VectorSpace;
//    vectorSpace->Initialize(10000);
//    CollisionWorld* collisionWorld = new CollisionWorld(vectorSpace);
//
//
//    Map* map = EditorSystem::CreateMap(system, "AttrMainMap", false, false);
//  //  map->SetName("AttrMainMap");
//  //  map->Initialize(system);
//    map->CreateMapLayer(0, 0, nullptr, collisionWorld, { 0,0,(float)window->GetClientWidth(), (float)window->GetClientHeight() ,0.0f,1.0f });
//   // Camera* gameCamera = new Camera(L"GameCamera");
//   
// 
//
//    system->Initialize(window->GetClientWidth(), window->GetClientHeight(), map);
//    Map* currMap = system->GetMap();
//   // FrustumCamera* gameCamera = static_cast<FrustumCamera*>(currMap->CreateObject("FrustumCamera"));
//    FrustumCamera* gameCamera = FrustumCamera::Create(currMap, 0, DirectX::XM_PI / 2, (float)window->GetClientWidth() / window->GetClientHeight());
// //  gameCamera->Initialize(DirectX::XM_PI / 2, (float)window->GetClientWidth() / window->GetClientHeight());
//    currMap->SetMainCamera(gameCamera);
//   // map->SetCameraType(ECameraType::ePerspectiveCamera);
//    currMap->GetMainCamera()->GetTransform().SetPositionLocal({ 0,0,-10.0f });
//
//
//
//}
//
//void Application::InitAttributeUiSystem(TaskWindow* window,AttributeUiSystem* system)
//{
//
//
//    VectorSpace* vectorSpace = new VectorSpace;
//    vectorSpace->Initialize(10000);
//
//    CollisionWorld* collisionWorld = new CollisionWorld(vectorSpace);
//
//    Map* map = EditorSystem::CreateMap(system, "AttrUiMainMap", false, false);
//   // map->SetName("AttrUiMainMap");
//   // map->Initialize(system);
//    map->CreateMapLayer(0, 0, nullptr, collisionWorld, { 0,0,(float)window->GetClientWidth(), (float)window->GetClientHeight() ,0.0f,1.0f });
//
//  
//    
//
//
//    RenderTargetTexture * renderTargetTexture = TextureManager::CreateRenderTargetTexture("AnimationEdit3DPanelTexture", 300, 300);
//    //TextureManager::AddTexture(renderTargetTexture, L"AnimationEdit3DPanelTexture");
//    renderTargetTexture->SetEngineContentItemFlag(true);
//
//    Texture* depthStencilBuffer = TextureManager::CreateDepthStencilBuffer("AnimationEdit3DPanelDepthStencilBuffer", 300, 300);
//    //map layer 1 
//    depthStencilBuffer->SetEngineContentItemFlag(true);
//    vectorSpace = new VectorSpace;
//    vectorSpace->Initialize(100);
//    CollisionWorld* animationEdit3DCollisionWorld = new CollisionWorld(vectorSpace);
//
//
//    map->CreateMapLayer(1, 1, nullptr, animationEdit3DCollisionWorld, { 0,0,300,300,0.0f,1.0f }, renderTargetTexture, depthStencilBuffer);
//
//  //  Camera* animationEdit3DCamera = new Camera(L"AnimationEdit3DCamera");
//   
//
//    system->Initialize(window->GetClientWidth(), window->GetClientHeight(), map);
//
//    Map* currMap = system->GetMap();
//
//
//   // OrthogoanlCamera* gameCamera = static_cast<OrthogoanlCamera*>(currMap->CreateObject("OrthogoanlCamera"));
//    OrthogoanlCamera* gameCamera = OrthogoanlCamera::Create(currMap, 0, window->GetClientWidth(), window->GetClientHeight());
//   // gameCamera->Initialize(window->GetClientWidth(), window->GetClientHeight());
//
//    currMap->SetMainCamera(gameCamera);
//
//
//
//  //  FrustumCamera* animationEdit3DCamera = static_cast<FrustumCamera*>(currMap->CreateObject("FrustumCamera", 1));
//    FrustumCamera* animationEdit3DCamera = FrustumCamera::Create(currMap, 0, 1000, 1000);
////    animationEdit3DCamera->Initialize(1000, 1000);
//    currMap->SetMainCamera(animationEdit3DCamera, 1);
//
//
//    //map->SetCameraType(ECameraType::ePerspectiveCamera);
//    currMap->GetMainCamera()->GetTransform().SetPositionLocal({ 0,0,0.0f });
//
//
//    currMap->GetMainCamera()->GetTransform().SetPositionLocal({ (float)window->GetClientWidth() / 2,-1.0f * (float)window->GetClientHeight() / 2 , 0 });
//
//
//
//
//}
//
//void Application::InitFileUiUiSystem(TaskWindow* window, FileUiUiSystem* system)
//{
//
//   
//    VectorSpace* vectorSpace = new VectorSpace;
//    vectorSpace->Initialize(100000000);
//    CollisionWorld* collisionWorld = new CollisionWorld(vectorSpace);
//
//
//    Map* map = EditorSystem::CreateMap(system, "FileUiUiMainMap", false, false);
//  //  map->SetName("FileUiUiMainMap");
//  //  map->Initialize(system);
//    map->CreateMapLayer(0, 0, nullptr, collisionWorld, { 0,0,(float)window->GetClientWidth(),(float)window->GetClientHeight(),0.0f,1.0f });
// 
//
//
//    //gameCamera를 설정은했는데 맵으로 들어가진않은거지
//
//    system->Initialize(window->GetClientWidth(), window->GetClientHeight(), map);
//    Map* currMap = system->GetMap();
//
//  //  OrthogoanlCamera* gameCamera = static_cast<OrthogoanlCamera*>(currMap->CreateObject("OrthogoanlCamera"));
//    OrthogoanlCamera* gameCamera = OrthogoanlCamera::Create(currMap, 0, window->GetClientWidth(), window->GetClientHeight());
//  //  gameCamera->Initialize(window->GetClientWidth(), window->GetClientHeight());
//
//    currMap->SetMainCamera(gameCamera);
//    //map->SetCameraType(ECameraType::ePerspectiveCamera);
//    currMap->GetMainCamera()->GetTransform().SetPositionLocal({ 0,0,-10.0f });
//
//
//}
//
//void Application::InitFileUiSystem(TaskWindow* window, FileUiSystem* system)
//{
//
//   
//
//    VectorSpace* vectorSpace = new VectorSpace;
//    vectorSpace->Initialize(10000);
//
//    CollisionWorld* collisionWorld = new CollisionWorld(vectorSpace);
//
//    Map* map = EditorSystem::CreateMap(system, "FileUiMainMap", false, false);
// //   map->SetName("FileUiMainMap");
//  //  map->Initialize(system);
//    map->CreateMapLayer(0, 0, nullptr, collisionWorld, { 0,0,(float)window->GetClientWidth(), (float)window->GetClientHeight() ,0.0f,1.0f });
//
//  
//    //map->AddObject(gameCamera);
//
//    system->Initialize(window->GetClientWidth(), window->GetClientHeight(), map);
//    Map* currMap = system->GetMap();
//  //  OrthogoanlCamera* gameCamera = static_cast<OrthogoanlCamera*>(currMap->CreateObject("OrthogoanlCamera"));
//    OrthogoanlCamera* gameCamera = OrthogoanlCamera::Create(currMap, 0, window->GetClientWidth(), window->GetClientHeight());
//  //  gameCamera->Initialize(window->GetClientWidth(), window->GetClientHeight());
//
//    currMap->SetMainCamera(gameCamera);
//    //map->SetCameraType(ECameraType::ePerspectiveCamera);
//    currMap->GetMainCamera()->GetTransform().SetPositionLocal({ 0,0,0.0f });
//
//
//
//
//}
//
//void Application::InitFrameWindowSystem(TaskWindow* window, FrameWindowSystem* system)
//{
//
//
//
//    VectorSpace* vectorSpace = new VectorSpace;
//    vectorSpace->Initialize(10000);
//    CollisionWorld* collisionWorld = new CollisionWorld(vectorSpace);
//
//    Map* map = EditorSystem::CreateMap(system, "FrameWindowMainMap", false, false);
//
//   // map->SetName("FrameWindowMainMap");
// //   map->Initialize(system);
//    map->CreateMapLayer(0, 0, nullptr, collisionWorld, { 0,0,(float)window->GetClientWidth(), (float)window->GetClientHeight() ,0.0f,1.0f });
//  
//
//    system->Initialize(window->GetClientWidth(), window->GetClientHeight(), map);
//
//    Map* currMap = system->GetMap();
//    //OrthogoanlCamera* gameCamera = static_cast<OrthogoanlCamera*>(currMap->CreateObject("OrthogoanlCamera"));
//    OrthogoanlCamera* gameCamera = OrthogoanlCamera::Create(currMap, 0, window->GetClientWidth(), window->GetClientHeight());
//  //  gameCamera->Initialize(window->GetClientWidth(), window->GetClientHeight());
//
//
//    currMap->SetMainCamera(gameCamera);
//
//    //map->SetCameraType(ECameraType::ePerspectiveCamera);
//
//    currMap->GetMainCamera()->GetTransform().SetPositionLocal({ 0,0,0.0f });
//
//
//}
//
//void Application::InitFrameWindowUiSystem(TaskWindow* window, FrameWindowUiSystem* system)
//{
//
//
//
//    VectorSpace* vectorSpace = new VectorSpace;
//    vectorSpace->Initialize(10000);
//
//    CollisionWorld* collisionWorld = new CollisionWorld(vectorSpace);
//
//    Map* map = EditorSystem::CreateMap(system, "FrameWindowUiMainMap", false, false);
//    // map->SetName("FrameWindowUiMainMap");
//   //  map->Initialize(system);
//    map->CreateMapLayer(0, 0, nullptr, collisionWorld, { 0,0,(float)window->GetClientWidth(), (float)window->GetClientHeight() ,0.0f,1.0f });
//
//
//    system->Initialize(window->GetClientWidth(), window->GetClientHeight(), map);
//    Map* currMap = system->GetMap();
//    // OrthogoanlCamera* gameCamera = static_cast<OrthogoanlCamera*>(currMap->CreateObject("OrthogoanlCamera"));
//    OrthogoanlCamera* gameCamera = OrthogoanlCamera::Create(currMap, 0, window->GetClientWidth(), window->GetClientHeight());
//
//    //  gameCamera->Initialize(window->GetClientWidth(), window->GetClientHeight());
//
//    currMap->SetMainCamera(gameCamera);
//    // map->SetCameraType(ECameraType::ePerspectiveCamera);
//    currMap->GetMainCamera()->GetTransform().SetPositionLocal({ (float)window->GetClientWidth() / 2, (float)-window->GetClientHeight() / 2,0.0f });
//    system->SetViewPort(0, 60, (float)window->GetClientWidth(), (float)window->GetClientHeight(), 0.0f, 1.0f);
//
//
//}
////
//
//
//
//void Application::InitGamePlayWindowLayoutSystem(TaskWindow* window, WindowChromeSystem* system)
//{
//
//
//    VectorSpace* vectorSpace = new VectorSpace;
//    vectorSpace->Initialize(100);
//    CollisionWorld* collisionWorld = new CollisionWorld(vectorSpace);
//
//
//    Map* map = EditorSystem::CreateMap(system, "GamePlayLayoutMainMap", false, false);
//  //  map->SetName("GamePlayLayoutMainMap");
//  //  map->Initialize(system);
//    map->CreateMapLayer(0, 0, nullptr, collisionWorld, { 0,0,(float)window->GetClientWidth(), (float)window->GetClientHeight() ,0.0f,1.0f });
//   
//
//    system->Initialize(window->GetClientWidth(), window->GetClientHeight(), map);
//
//    Map* currMap = system->GetMap();
//
//    //ChromeSystemCamera* gameCamera = static_cast<ChromeSystemCamera*>(currMap->CreateObject("ChromeSystemCamera"));
//    ChromeSystemCamera* gameCamera = ChromeSystemCamera::Create(currMap, 0, window->GetClientWidth(), window->GetClientHeight());
//
//   // gameCamera->SetSystem(system);
//   // gameCamera->Initialize(window->GetClientWidth(), window->GetClientHeight());
//
//    currMap->SetMainCamera(gameCamera);
//
//
//
//   // system->SetTitleBarSize(3000, 40);
//
//  //  WindowCloseButton* windowCloseButton = new WindowCloseButton("WindoeCloseButton");
//  //  WindowCloseButton* windowCloseButton = static_cast<WindowCloseButton*>(currMap->CreateObject("WindowCloseButton"));
//    WindowCloseButton* windowCloseButton = WindowCloseButton::Create(currMap, 0);
//    //windowCloseButton->SetSystem(system);
//  //  windowCloseButton->Initialize();
//    windowCloseButton->SetKeepVisibleBaseHorizontalLineOffset(20);
//    windowCloseButton->SetKeepVisibleBaseVerticalLineOffset(20);
//
// 
//
//
//
//
//
//
//
//
// //  // system->AddEntity(windowCloseButton);
//
// ////   ButtonUiEntitiy* gamePlayButton = new ButtonUiEntitiy("GamePlayButton");
// //  // ButtonUiEntity* gamePlayButton = static_cast<ButtonUiEntity*>(currMap->CreateObject("ButtonUiEntity"));
// //   ButtonUiEntity* gamePlayButton = ButtonUiEntity::Create(currMap, 0);
// //   std::pair<float, float> buttonSize = { 35,35 };
// //  // gamePlayButton->SetSystem(system);
// // //  gamePlayButton->Initialize();
// //   gamePlayButton->SetSize(buttonSize.first, buttonSize.second);
// //   gamePlayButton->SetTexture("gamePlayButtonOff.png");
// //   gamePlayButton->SetPosition(100, 3000 - 20, 5);
// //   gamePlayButton->RegisterAcceptEvent("WindowResizeEvent");
// //   gamePlayButton->RegisterAcceptEvent("LButtonDown");
// //   gamePlayButton->RegisterAcceptEvent("LButtonUp");
// //   gamePlayButton->RegisterEventCallback("WindowResizeEvent", [button = gamePlayButton](Event* pEvent) {
//
// //       WindowResizeEvent* resizeEvent = (WindowResizeEvent*)pEvent;
//
// //       float windowWidth = resizeEvent->GetWindowWidth();
// //       DirectX::XMFLOAT3 pos = button->GetTransform().GetPositionLocal();
// //       pos.x = windowWidth / 2;
// //       button->GetTransform().SetPositionLocal(pos);
// //       });
//
// //   
//
// //   gamePlayButton->SetHoverCallback([button = gamePlayButton]() {
//
// //       button->SetColorItensity({ 1.3f,1.3f,1.3f });
//
// //       });
//
// //   gamePlayButton->SetReleaseHoverCallback([button = gamePlayButton]() {
//
// //       button->SetColorItensity({ 1.0f,1.0f,1.0f });
// //       });
// //   gamePlayButton->SetLButtonDownCallback([button = gamePlayButton]() {
//
// //      // button->SetColorItensity({ 0.0f,0.0f,1.0f });
//
//
// //       });
// //   gamePlayButton->SetLButtonUpCallback([button = gamePlayButton ,application = this , gamePlayWindowController  = 
// //       mGameWindowPlayController]() {
//
// //       
// //       //play mode시작.
// //       bool prePlayMode = application->GetPlayModeState();
//
//
// //       if (prePlayMode == false)
// //       {
// //           application->SetPlayModeState(true);
// //           gamePlayWindowController->SetPause(false);
// //           button->SetTexture("gamePlayButtonOn.png");
// //       }
// //       else
// //       {
// //         // application->SetPlayModeState(false);   //stop
//
// //           if (gamePlayWindowController->GetPause())
// //           {
// //               gamePlayWindowController->SetPause(false);
// //               button->SetTexture("gamePlayButtonOn.png");
// //           }
// //           else
// //           {
// //               gamePlayWindowController->SetPause(true);
// //               button->SetTexture("gamePlayButtonOff.png");
// //           }
//
// //
//
// //       }
//
// //       //esc 는 play mode 종료
//
// //       });
//
//
// // //  ButtonUiEntitiy * gamePlayEndButton = new ButtonUiEntitiy("GamePlayEndButton");
// //  // ButtonUiEntity * gamePlayEndButton = static_cast<ButtonUiEntity*>(currMap->CreateObject("ButtonUiEntity"));
// //   ButtonUiEntity* gamePlayEndButton = ButtonUiEntity::Create(currMap, 0);
// //  // gamePlayEndButton->SetSystem(system);
// // //  gamePlayEndButton->Initialize();
// //   gamePlayEndButton->GetTransform().SetIndependentRotationFlag(true);
// //   gamePlayEndButton->GetTransform().SetIndependentScaleFlag(true);
// //   gamePlayEndButton->SetSize(buttonSize.first, buttonSize.second);
// //   gamePlayEndButton->SetTexture("gamePlayEndButton.png");
//
// //   DirectX::XMFLOAT3 pos = gamePlayButton->GetTransform().GetPositionWorld();
// //   pos.x += gamePlayButton->GetWidth() / 2 +5 + gamePlayEndButton->GetWidth() / 2;
// // //  system->AddEntity(gamePlayEndButton);
// //   gamePlayEndButton->GetTransform().SetPositionWorld(pos);
//
// //   gamePlayEndButton->RegisterAcceptEvent("WindowResizeEvent");
// //   gamePlayEndButton->RegisterEventCallback("WindowResizeEvent", [button = gamePlayEndButton](Event* pEvent) {
//
// //       WindowResizeEvent* resizeEvent = (WindowResizeEvent*)pEvent;
//
//
// //       float windowWidth = resizeEvent->GetWindowWidth();
// //       DirectX::XMFLOAT3 pos = button->GetTransform().GetPositionLocal();
// //       pos.x = windowWidth / 2 + button->GetWidth() + 5;
// //       button->GetTransform().SetPositionLocal(pos);
//
// //       });
//
//
// //   gamePlayEndButton->SetHoverCallback([button = gamePlayEndButton]() {
//
// //       button->SetColorItensity({ 1.3f,1.3f,1.3f });
// //       });
// //   gamePlayEndButton->SetReleaseHoverCallback([button = gamePlayEndButton] {
//
// //       button->SetColorItensity({ 1.0f,1.0f,1.0f });
//
// //       });
//
//
// //   gamePlayEndButton->RegisterAcceptEvent("LButtonDown");
// //   gamePlayEndButton->SetLButtonDownCallback([gamePlayEndButton = gamePlayEndButton , playOnButton = gamePlayButton, application = this]() {
//
// //       //리셋         - playmode 가 false가되고 ,다 초기상태로
// //       application->SetPlayModeState(false);
//
// //       playOnButton->SetTexture("gamePlayButtonOff.png");
//
// //       });
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//}
//
//void Application::InitAttributeWindowLayoutSystem(TaskWindow* window, WindowChromeSystem* system)
//{
// 
//
//    VectorSpace* vectorSpace = new VectorSpace;
//    vectorSpace->Initialize(100);
//    CollisionWorld* collisionWorld = new CollisionWorld(vectorSpace);
//
//
//    Map* map = EditorSystem::CreateMap(system, "AttrLayoutMainMap", false, false);
//  //  map->SetName("AttrLayoutMainMap");
//  // map->Initialize(system);
//    map->CreateMapLayer(0, 0, nullptr, collisionWorld, { 0,0,(float)window->GetClientWidth(), (float)window->GetClientHeight() ,0.0f,1.0f });
//    //  map->AddObject(gameCamera);
//    //ChromeSystemCamera* gameCamera = new ChromeSystemCamera(L"GameCamera");
// 
//   
//    system->Initialize(window->GetClientWidth(), window->GetClientHeight(), map);
//
//    Map* currMap = system->GetMap();
//   // system->SetTitleBarSize(3000, 40);
//   // ChromeSystemCamera* gameCamera = static_cast<ChromeSystemCamera*>(currMap->CreateObject("ChromeSystemCamera"));
//    ChromeSystemCamera* gameCamera = ChromeSystemCamera::Create(currMap, 0, window->GetClientWidth(), window->GetClientHeight());
//
//  //  gameCamera->SetSystem(system);
//   // gameCamera->Initialize(window->GetClientWidth(), window->GetClientHeight());
//
//    currMap->SetMainCamera(gameCamera);
//
//
//  //  WindowCloseButton* windowCloseButton = static_cast<WindowCloseButton*>(currMap->CreateObject("WindowCloseButton"));
//    WindowCloseButton* windowCloseButton = WindowCloseButton::Create(currMap, 0);
//   // windowCloseButton->SetSystem(system);
// //   windowCloseButton->Initialize();
//    windowCloseButton->SetKeepVisibleBaseHorizontalLineOffset(20);
//    windowCloseButton->SetKeepVisibleBaseVerticalLineOffset(20);
//    windowCloseButton->SetDepth(2);
//  //  system->AddEntity(windowCloseButton);
//
//
//}
//
//void Application::InitFileUiWindowLayoutSystem(TaskWindow* window, WindowChromeSystem* system)
//{
//
//
//
//    VectorSpace* vectorSpace = new VectorSpace;
//    vectorSpace->Initialize(100);
//    CollisionWorld* collisionWorld = new CollisionWorld(vectorSpace);
//
//
//    Map* map = EditorSystem::CreateMap(system, "FileUiLayoutMainMap", false, false);
//
//   // map->SetName("FileUiLayoutMainMap");
//  //  map->Initialize(system);
//    map->CreateMapLayer(0, 0, nullptr, collisionWorld, { 0,0,(float)window->GetClientWidth(), (float)window->GetClientHeight() ,0.0f,1.0f });
//   
//   // map->AddObject(gameCamera);
//    system->Initialize(window->GetClientWidth(), window->GetClientHeight(), map);
//   // system->SetTitleBarSize(3000, 40);
//
//    Map* currMap = system->GetMap();
//   // ChromeSystemCamera* gameCamera = static_cast<ChromeSystemCamera*>(currMap->CreateObject("ChromeSystemCamera"));
//    ChromeSystemCamera* gameCamera = ChromeSystemCamera::Create(currMap, 0, window->GetClientWidth(), window->GetClientHeight());
//
//   // gameCamera->SetSystem(system);
//    //gameCamera->Initialize(window->GetClientWidth(), window->GetClientHeight());
//
//    currMap->SetMainCamera(gameCamera);
//
// //   WindowCloseButton* windowCloseButton = new WindowCloseButton("WindoeCloseButton");
//   // WindowCloseButton* windowCloseButton = static_cast<WindowCloseButton*>(currMap->CreateObject("WindowCloseButton"));
//    WindowCloseButton* windowCloseButton = WindowCloseButton::Create(currMap, 0);
//    //windowCloseButton->SetSystem(system);
//  //  windowCloseButton->Initialize();
//    windowCloseButton->SetKeepVisibleBaseHorizontalLineOffset(20);
//    windowCloseButton->SetKeepVisibleBaseVerticalLineOffset(20);
//
//  //  system->AddEntity(windowCloseButton);
//
//}
//
//void Application::InitFrameWindowLayoutSystem(TaskWindow* window, WindowChromeSystem* system)
//{
//
//
//
//    VectorSpace* vectorSpace = new VectorSpace;
//    vectorSpace->Initialize(100);
//
//    CollisionWorld* collisionWorld = new CollisionWorld(vectorSpace);
//
//
//
//    Map* map = EditorSystem::CreateMap(system, "FrameWIindowLayoutMainMap", false, false);
//  //  map->SetName("FrameWIindowLayoutMainMap");
//   // map->Initialize(system);
//    map->CreateMapLayer(0, 0, nullptr, collisionWorld, { 0,0,(float)window->GetClientWidth(), (float)window->GetClientHeight() ,0.0f,1.0f });
//   
//    //map->AddObject(gameCamera);
//    system->Initialize(window->GetClientWidth(), window->GetClientHeight(), map);
//
//    Map* currMap = system->GetMap();
//    int mapLayerIndex = 0;
//  //  ChromeSystemCamera* gameCamera = static_cast<ChromeSystemCamera*>(currMap->CreateObject("ChromeSystemCamera"));
//    ChromeSystemCamera* gameCamera = ChromeSystemCamera::Create(currMap, 0, window->GetClientWidth(), window->GetClientHeight());
//
//
//    //  gameCamera->SetSystem(system);
//    // gameCamera->Initialize(window->GetClientWidth(), window->GetClientHeight());
//
//    currMap->SetMainCamera(gameCamera);
//
//
//
//    //system->SetTitleBarSize(3000, 60);
//
//   // WindowCloseButton* windowCloseButton = new WindowCloseButton("WindoeCloseButton");
// //   WindowCloseButton* windowCloseButton = static_cast<WindowCloseButton*>(currMap->CreateObject("WindowCloseButton"));
//    WindowCloseButton* windowCloseButton = WindowCloseButton::Create(currMap, mapLayerIndex);
// //   windowCloseButton->SetSystem(system);
//  //  windowCloseButton->Initialize();
//    windowCloseButton->SetKeepVisibleBaseHorizontalLineOffset(20);
//    windowCloseButton->SetKeepVisibleBaseVerticalLineOffset(20);
//    windowCloseButton->SetDepth(2);
//
// //   system->AddEntity(windowCloseButton);
//
//
//    //프레임윈도우 처음resize가 먼저 전달되서 안보이는것같다 
//
// //   WindowMaxRestoreButton* windowMaxRestoreButton = new WindowMaxRestoreButton("WindowMaxRestoreButton");
//   // WindowMaxRestoreButton* windowMaxRestoreButton = static_cast<WindowMaxRestoreButton*>(currMap->CreateObject("WindowMaxRestoreButton"));
//    WindowMaxRestoreButton* windowMaxRestoreButton = WindowMaxRestoreButton::Create(currMap, mapLayerIndex);
//  //  windowMaxRestoreButton->SetSystem(system);
//  //  windowMaxRestoreButton->Initialize();
//    windowMaxRestoreButton->SetKeepVisibleBaseHorizontalLine(0);
//    windowMaxRestoreButton->SetKeepVisibleBaseVerticalLine(1);
//    windowMaxRestoreButton->SetKeepVisibleBaseHorizontalLineOffset(20);
//    windowMaxRestoreButton->SetKeepVisibleBaseVerticalLineOffset(60);
//
//   // system->AddEntity(windowMaxRestoreButton);
//
//   // WindowMinButton* windowMinButton = new WindowMinButton("WindowMinButton");
//   // WindowMinButton* windowMinButton = static_cast<WindowMinButton*>(currMap->CreateObject("WindowMinButton"));
//    WindowMinButton* windowMinButton = WindowMinButton::Create(currMap, mapLayerIndex);
//   // windowMinButton->SetSystem(system);
//  //  windowMinButton->Initialize();
//    windowMinButton->SetKeepVisibleBaseHorizontalLine(0);
//    windowMinButton->SetKeepVisibleBaseVerticalLine(1);
//    windowMinButton->SetKeepVisibleBaseHorizontalLineOffset(20);
//    windowMinButton->SetKeepVisibleBaseVerticalLineOffset(100);
//
//  //  system->AddEntity(windowMinButton);
//
//
//
//
//
//
//}
//
//
//
//void Application::InitFrameWindowDockingSystem(TaskWindow* window, FrameWindowDockingSystem* system)
//{
//
//
//  
//
//    VectorSpace* vectorSpace = new VectorSpace;
//    vectorSpace->Initialize(50);
//
//    CollisionWorld* collisionWorld = new CollisionWorld(vectorSpace);
//
//    Map* map = EditorSystem::CreateMap(system, "FrameWindowDockingMainMap", false, false);
// //   map->SetName("FrameWindowDockingMainMap");
// //   map->Initialize(system);
//    map->CreateMapLayer(0, 0, nullptr, collisionWorld, { 0,0,(float)window->GetClientWidth(), (float)window->GetClientHeight() ,0.0f,1.0f });
//   // DockingSystemCamera* gameCamera = new DockingSystemCamera("GameCamera");
//  
//
//
//   // map->AddObject(gameCamera);
//    system->Initialize(window->GetClientWidth(), window->GetClientHeight(), map);
//
//    Map* currMap = system->GetMap();
//  //  DockingSystemCamera* gameCamera = static_cast<DockingSystemCamera*>(currMap->CreateObject("DockingSystemCamera"));
//    DockingSystemCamera* gameCamera = DockingSystemCamera::Create(currMap, 0, window->GetClientWidth(), window->GetClientHeight());
//    // DockingSystemCamera* gameCamera = ObjectFactory::CreateObject()
//   // gameCamera->Initialize(window->GetClientWidth(), window->GetClientHeight());
//
//    currMap->SetMainCamera(gameCamera);
//
//}
//
//
//void Application::InitChildWindowDockingSystem(TaskWindow* window, ChildWindowDockingSystem* system)
//{
//
//   
//
//    VectorSpace* vectorSpace = new VectorSpace;
//    vectorSpace->Initialize(10);
//
//    CollisionWorld* collisionWorld = new CollisionWorld(vectorSpace);
//
//
//
//    Map* map = EditorSystem::CreateMap(system, "ChildWindowDockingMainMap" + std::to_string((int)system->GetSystemID()), false, false);
//   // map->SetName("ChildWindowDockingMainMap"+std::to_string((int)system->GetSystemID()));
//    //map->Initialize(system);
//    map->CreateMapLayer(0, 0, nullptr, collisionWorld, { 0,0,(float)window->GetClientWidth(), (float)window->GetClientHeight() ,0.0f,1.0f });
// //   DockingSystemCamera* gameCamera = new DockingSystemCamera("GameCamera");
//
//    system->Initialize(window->GetClientWidth(), window->GetClientHeight(), map);
//    Map* currMap = system->GetMap();
//  //  DockingSystemCamera* gameCamera = static_cast<DockingSystemCamera*>(currMap->CreateObject("DockingSystemCamera"));
//    DockingSystemCamera* gameCamera = DockingSystemCamera::Create(currMap, 0, window->GetClientWidth(), window->GetClientHeight());
//
//    //gameCamera->Initialize(window->GetClientWidth(), window->GetClientHeight());
//
//    currMap->SetMainCamera(gameCamera);
//
//
//
//}
//
//void Application::CreateInitGameWindowEntity(GamePlaySystem* system)
//{
//
//
//  /*  Map* currMap = system->GetMap();
//    Light* light =static_cast<Light*>(currMap->CreateObject("Light"));
//    light->SetLight(1.0f, 1.0f, 1.0f);*/
//
//
//
//
//
// //   //현재 맵에넣어도되고, 시스템에넣어도 현재 맵에 넣어지고
//
//
// //   Line* line = LineFactory::CreateLine({ -50,5,10 }, { 50, 5, 10 });
// //   line->SetSystem(system);
// //   system->AddEntity(line, true);
//
// //   Grid* grid = LineFactory::CreateGrid(1000, 1000, 1, 1);
// //   grid->SetSystem(system);
// //   system->AddEntity(grid, true);
// //   grid->GetMaterial().SetColor({ 1.0f,1.0f,1.0f });
// //   // Grid * grid = Grid::Create(L"BaseGrid", 500, 500, 1);
// //   grid->SetSelectAvailableFlag(false);
// //   grid->SetSelectBlockFlag(false);
// //   //system->AddEntity(grid);
//
//
//
//
//
//
//
//
//
//
//
// //      Entity* heroOne = new Entity("hereOne");
// //   heroOne->SetSystem(system);
// //   heroOne->Initialize(MeshManager::GetMesh("black_Bison"));
// //  // heroOne->Initialize(MeshManager::GetMesh(L"black_Bison"));
// // //  heroOne->AddModelComponent(MeshManager::GetMesh("black_Bison"));
//
//
// //   heroOne->GetTransform().SetPositionLocal({ 6.0F,-2.0F,0.0F });
// //   heroOne->GetTransform().SetScaleLocal({ 1.0F,1.0F,1.0F });
// //    system->AddEntity(heroOne);
//
// //    Entity* cube = new Entity("Cube");
// //    //cube->Initialize(MeshManager::GetMesh(L"Cube"));
//
// //   Entity* cube = static_cast<Entity*>(currMap->CreateObject("Entity"));
// //   cube->GetModel()->GetMeshComponent()->SetMesh(MeshManager::GetMesh("black_Bison"));
// ////    cube->SetSystem(system);
// ////    cube->Initialize(MeshManager::GetMesh("Cube"));
// //// //   cube->AddModelComponent(MeshManager::GetMesh("Cube"));
// ////   // cube->GetTransform().SetPositionLocal({ -10.0F,2.0F,5.0F });
// //    cube->GetTransform().SetPositionWorld({ 0.0f,-2.0f,5.0f });
// //  
// //    cube->GetTransform().SetScaleLocal({ 0.5F,0.5F,0.5F });
// //    cube->SetName("Cube");
// //   
// //    cube->SetUpdateCallback([dir = -1 ](Object * object ,float deltaTime)mutable {
//
// //        Entity* cube = (Entity*)object;
// //        DirectX::XMFLOAT3 posWorld = cube->GetTransform().GetPositionWorld();
// //         //dir -1: left , 1: right
// //        float shiftX = 3.0f;
//
// //        if (posWorld.x <= -30.0f)
// //        {
// //            dir = 1;
// //        }
// //        else if (posWorld.x >= 30.0f)
// //        {
// //            dir = -1;
// //        }
//
// //        posWorld.x += dir * shiftX * deltaTime;
// //        cube->GetTransform().SetPositionWorld(posWorld);
//
// //        });
//
//
// //    system->AddEntity(cube);
//
//
//
//
// //    //이 heroOne에 에니메이션을 입히고 처리
// //    //에니메이션에대한 effect도필요하고
// //    //update처리 
// //    //매프레임마다 두 키프레임보간, 뼈대들을 변환행렬 계산.
// //    
// //    //그엔티티에 skeleton,그리고 animationclip을 붙여보자
// //    //animationclip은나중에 여러개가 들어갈수있을거고
// //    
// //    //뼈대에서 오프셋변환행렬, 그리고 에니메이션클립에서 그 시간에서 뼈대의 루트변환행렬
// //    //이둘이 함께처리되어서 최종적인 뼈대팔레트행렬을 완성해야돼
// //    //그럼 model에 anim부분을 만들고, 그안에 다가 아니메이션클립하고 스켈레톤은 설정할수있게하고
// //    //그 anim에서 그둘을 이용해 최종행렬들을 계산하자.
// //    //upload는 자동으로 될것이고.
// //    //에니메이션클립은 당연히 공유되겠지 그럼, 그 에니메이션클립에 시간을 넘겨서 
// //    //변환행렬을 얻는방식으로 하는게좋고
// //    
// //    AnimationClip* black_bsionDefaultAnimClip = AnimationClipManager::GetAnimationClip("black_bison_Anim");
//
// //    AnimationClip* black_bsionTestAnimClip = AnimationClipSplitter::SplitAnimationClip(black_bsionDefaultAnimClip, "black_bisonTestAnimClip", 60, 77);
//
// //    if (black_bsionTestAnimClip == nullptr)
// //    {
// //        OutputDebugString(L"널이였다");
// //        assert(-1);
// //    }
//
//
//
// //    AnimationComponent & animationComponent=  *heroOne->GetModel()->GetAnimationComponent();
// //    animationComponent.AddState("Default", AnimationClipManager::GetAnimationClip("black_bison_Anim"));
// //    animationComponent.SetSkeleton(SkeletonManager::GetSkeleton("black_bison_Skeleton"));
//
//
// //    animationComponent.AddState("Test",black_bsionTestAnimClip);
// //   // animationComponent.SetCurrentAnimationState(L"Default");
// //    animationComponent.SetCurrentAnimationState("Default");
//
//
// ////    heroOne->GetModel()->SetAnimationClip(AnimationClipManager::GetAnimationClip(L"black_bison_Anim"));
// //  //  heroOne->GetModel()->SetSkeleton(SkeletonManager::GetSkeleton(L"black_bison_Skeleton"));
// //      
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//}


void Application::CreateCommandObjects()
{
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;    //gpu가 실행할 명령을 담을 큐이다.
    queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.NodeMask = 0;

   ThrowIfFailed(mDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&mCommandQueue)));

    ThrowIfFailed(mDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&mCommandAllocator)));

    ThrowIfFailed(mDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mCommandAllocator.Get(), nullptr, IID_PPV_ARGS(&mGraphicscommandList)));

    mGraphicscommandList->Close();


    mGraphicCommandObject.Initialize(mGraphicscommandList, mCommandAllocator, mCommandQueue, mFence);

    //commandList를 생성하면 열린상태가된다.

}



void Application::PreUpdate(GameTimer& timer)
{

    float deltaTime = timer.GetDeltaTime();
    mResourceController.Update();

    mInputSystem.Update(deltaTime);

    mProgramDirector->PreUpdate(deltaTime);
}

void Application::Update(GameTimer& timer)
{

  //  mResourceController.Update();

    //사전에 effect들의 shader resource들을 초기화(특히,상수버퍼,구조적버퍼)

    //아마 리소스 업로드문제 윈도우별,윈도우안의 시스템별
    //업데이트순서문제로인해 깜빡거림 발생한는거같다.
 
    float deltaTime = timer.GetDeltaTime();

    mProgramDirector->Update(deltaTime);
 

}

void Application::EndUpdate(GameTimer& timer)
{

    float deltaTime = timer.GetDeltaTime();
    mProgramDirector->EndUpdate(deltaTime);

    mMouse.EndUpdate();

    mEditGameObjectManager.RemoveDeadObject();
    mEditObjectManager.RemoveDeadObject();
    mRuntimeGameObjectManager.RemoveDeadObject();
    mRuntimeObjectManager.RemoveDeadObject();

}

void Application::Draw(GameTimer& timer)
{
    
    mProgramDirector->Draw();

    /*if (mFrameWindow->GetProjectSelectSceneFlag())
    {
        mFrameWindow->Draw();
    }
    else
    {

        mRenderWindowTest->Draw();

        if (GetPlayModeState())
            return;


        mAttributeWindow->Draw();
        mFileUiWindow->Draw();

        mFrameWindow->Draw();

        if (mPopupWindow->GetVisibilityState())
            mPopupWindow->Draw();

        if (DragAndDropWindowController::GetWindowRunningState())
        {
            mDragAndDropWindow->Draw();
        }
    }*/



}

void Application::OnResize()
{

  /*  switch (mCurrentActiveWindow)
    {
    case 0:
        mRenderSystem.OnResize(mRenderWindowWidth, mRenderWindowHeight);
        break;
    case 1:
        mUiRenderSystem.OnResize(mRenderWindowWidth, mRenderWindowHeight);
        break;
    }*/
   // mRenderSystem.OnResize(mRenderWindowWidth, mRenderWindowHeight);
   // mUiRenderSystem.OnResize(mFileUiWindowWidth, mFileUiWindowHeight);
}

void Application::MouseDown(WPARAM wParam, int x, int y)
{
}

void Application::MouseUp(WPARAM wParam, int x, int y)
{
}

void Application::MouseMove(WPARAM wParam, int x, int y)
{
}

void Application::FlushCommandQueue()
{
    mCurrentFence++;
    ThrowIfFailed(mCommandQueue->Signal(mFence.Get(), mCurrentFence));

    if (mFence->GetCompletedValue() < mCurrentFence)
    {
        HANDLE eventHandle = CreateEventEx(nullptr, nullptr, false, EVENT_ALL_ACCESS);
        ThrowIfFailed(mFence->SetEventOnCompletion(mCurrentFence, eventHandle));//이울타리지점에 도달하면 event발생

        WaitForSingleObject(eventHandle, INFINITE);
        CloseHandle(eventHandle);
    }
}

float Application::GetAspectRatio() const
{
    return static_cast<float>(mClientWidth) / mClientHeight;
}

Microsoft::WRL::ComPtr<ID3D12Device> Application::GetD3D12Device() const
{
    return mDevice;
}

Microsoft::WRL::ComPtr<IDXGIFactory4> Application::GetD3DFactory() const
{
    return mFactory;
}

GraphicCommand& Application::GetGraphicCommand()
{
    return mGraphicCommandObject;
    // TODO: 여기에 return 문을 삽입합니다.
}

DescriptorHeapManagerMaster* Application::GetDescriptorHeapManagerMaster() 
{
    return &mDescriptorHeapManagerMaster;
}

HINSTANCE Application::GetHinstance() const
{
    return mHinstance;
}



//bool Application::GetPlayModeState() 
//{
//   auto instance = GetInstance();
//    return instance->mPlayModeState;
//}

void Application::CalculateFrameStats()
{
    static int frameCnt = 0;

    static float timeElapsed = 0.0f;
    frameCnt++;

    if (mGameTimer.TotalTime() - timeElapsed >= 1.0f)
    {

        float fps = (float)frameCnt;        //fps=frameCnt/1(s);

        float mspf = 1.000f / fps;

        std::wstring fpsStr = std::to_wstring(fps);
        std::wstring mspfStr = std::to_wstring(mspf);

        std::wstring windowText = L"fps : " + fpsStr + L" mspf : " + mspfStr;
        SetWindowTextW(mHwnd,windowText.c_str());

        frameCnt = 0;
        timeElapsed += 1.0f;
    }
}

void Application::AddEffect(RenderSystem* renderSystem, Effect* effect)
{
    renderSystem->AddEffect(*effect, ESystemType::eMainSystem);
    renderSystem->AddEffect(*effect, ESystemType::eDockingSystem);
    //renderSystem->AddEffect(*effect, ESystemType::eUiSystem);
    renderSystem->AddEffect(*effect, ESystemType::eWindowLayoutSystem);

}

void Application::HarfBuzzTest()
{
    FT_Library  library;        //FT_Library는 핸들이다 (모든 타입이 다 그런거같다 ,객체는내부적으로 존재하는거고)
   FT_Error error = FT_Init_FreeType(&library);
   if (error)
   {
       MessageBox(nullptr, L"error", L"error", MB_OK);
      // ... an error occurred during library initialization ...
   }

   FT_Face faceFt;
//   error = FT_New_Face(library, "C:\\Users\\dongd\\Downloads\\aver-font\\AverBold-4YlW.ttf",
   error = FT_New_Face(library, "C:\\Users\\dongd\\source\\repos\\SecenGraphQuadTree\\SecenGraphQuadTree\\Font\\DefaultFont.ttf",
       0, &faceFt);
   if (error == FT_Err_Unknown_File_Format)
   {
       MessageBox(nullptr, L"face error", L"error", MB_OK);
   }
   else if (error)
   {
       MessageBox(nullptr, L"face error", L"error", MB_OK);

   }
   faceFt->size->metrics.x_scale;



   // FT_UInt charA_glyph_index=  FT_Get_Char_Index(faceFt, 0x00000041);
    FT_UInt charA_glyph_index=  FT_Get_Char_Index(faceFt, 0x00000043);

    //glyph가여기서는 glyph slot(컨테이너)

    faceFt->size;

    //FT_Set_Char_Size(faceFt, 0,72 * 64, 96, 96);
    FT_Set_Pixel_Sizes(faceFt, 300, 300);
    FT_Load_Glyph(faceFt, charA_glyph_index, FT_LOAD_DEFAULT);
    
    FT_Render_Glyph(faceFt->glyph,
        FT_RENDER_MODE_NORMAL);

    FT_Glyph_Format format = faceFt->glyph->format;


   unsigned int w= faceFt->glyph->bitmap.width;

   BitmapToTextureResource(faceFt->glyph->bitmap);
 //  CreateBitMap(faceFt->glyph->bitmap);

    hb_buffer_t* buf;
    buf = hb_buffer_create();
    hb_buffer_add_utf8(buf, "hello World", -1, 0, -1);
    
    //2 Set the script, language and direction of the buffer
    //script는 여기서 문자체계 ,시스템을 의미한다.

    hb_buffer_set_direction(buf, HB_DIRECTION_LTR);
    hb_buffer_set_script(buf, HB_SCRIPT_LATIN);
    hb_buffer_set_language(buf, hb_language_from_string("en", -1));

   // hb_blob_t* blob = hb_blob_create_from_file("C:\\Users\\dongd\\source\\repos\\SecenGraphQuadTree\\SecenGraphQuadTree\\Lemon Shake Shake.otf");
    hb_blob_t* blob = hb_blob_create_from_file("C:\\Users\\dongd\\source\\repos\\SecenGraphQuadTree\\SecenGraphQuadTree\\Font\\DefaultFont.ttf");
    
    hb_face_t* face = hb_face_create(blob, 0);
    hb_font_t* font = hb_font_create(face);

    hb_shape(font, buf, NULL, 0);
    
    //hb_ft_face_create_referenced()

    unsigned int glyph_count;
    hb_glyph_info_t* glyph_info = hb_buffer_get_glyph_infos(buf, &glyph_count);
    hb_glyph_position_t* glyph_pos = hb_buffer_get_glyph_positions(buf, &glyph_count);
  
    hb_position_t cursor_x = 0;
    hb_position_t cursor_y = 0;
    for (unsigned int i = 0; i < glyph_count; i++) {
        hb_codepoint_t glyphid = glyph_info[i].codepoint;
        hb_position_t x_offset = glyph_pos[i].x_offset;
        hb_position_t y_offset = glyph_pos[i].y_offset;
        hb_position_t x_advance = glyph_pos[i].x_advance;
        hb_position_t  y_advance = glyph_pos[i].y_advance;
        
       // draw_glyph(glyphid, cursor_x + x_offset, cursor_y + y_offset);
        cursor_x += x_advance;
        cursor_y += y_advance;
    }

    hb_buffer_destroy(buf);
    hb_font_destroy(font);
    hb_face_destroy(face);
    hb_blob_destroy(blob);












}

void Application::CreateBitMap(FT_Bitmap bitmap)
{
    // Calculate aligned RowPitch (must be a multiple of 256 bytes for DirectX 12)
    UINT alignedRowPitch = (bitmap.pitch + D3D12_TEXTURE_DATA_PITCH_ALIGNMENT - 1) & ~(D3D12_TEXTURE_DATA_PITCH_ALIGNMENT - 1);

    // Create upload buffer
    D3D12_RESOURCE_DESC uploadBufferDesc = {};
    uploadBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    uploadBufferDesc.Alignment = 0;
    uploadBufferDesc.Width = alignedRowPitch * bitmap.rows;  // Buffer size needs to accommodate the entire texture
    uploadBufferDesc.Height = 1;
    uploadBufferDesc.DepthOrArraySize = 1;
    uploadBufferDesc.MipLevels = 1;
    uploadBufferDesc.Format = DXGI_FORMAT_UNKNOWN;
    uploadBufferDesc.SampleDesc.Count = 1;
    uploadBufferDesc.SampleDesc.Quality = 0;
    uploadBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    uploadBufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

    D3D12_HEAP_PROPERTIES uploadHeapProperties = {};
    uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
    uploadHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    uploadHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

    ThrowIfFailed(mDevice->CreateCommittedResource(
        &uploadHeapProperties,
        D3D12_HEAP_FLAG_NONE,
        &uploadBufferDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&uploadBuffer)
    ));

    // Map and copy bitmap data to the upload buffer
    uint8_t* pData = nullptr;
    D3D12_RANGE readRange = { 0, 0 }; // We do not intend to read this resource on the CPU
    ThrowIfFailed(uploadBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pData)));

    for (unsigned int row = 0; row < bitmap.rows; ++row)
    {
        memcpy(pData + row * alignedRowPitch, bitmap.buffer + row * bitmap.pitch, bitmap.width);
    }

    uploadBuffer->Unmap(0, nullptr);

    // Create texture resource in default heap
    D3D12_RESOURCE_DESC texDesc = {};
    texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    texDesc.Alignment = 0;
    texDesc.Width = bitmap.width;
    texDesc.Height = bitmap.rows;
    texDesc.DepthOrArraySize = 1;
    texDesc.MipLevels = 1;
    texDesc.Format = DXGI_FORMAT_R8_UNORM;
    texDesc.SampleDesc.Count = 1;
    texDesc.SampleDesc.Quality = 0;
    texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    texDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

    D3D12_HEAP_PROPERTIES defaultHeapProperties = {};
    defaultHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
    defaultHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    defaultHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;


    ThrowIfFailed(mDevice->CreateCommittedResource(
        &defaultHeapProperties,
        D3D12_HEAP_FLAG_NONE,
        &texDesc,
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_PPV_ARGS(&mTextTexture)
    ));

    // Copy data from upload buffer to texture
    D3D12_TEXTURE_COPY_LOCATION srcLocation = {};
    srcLocation.pResource = uploadBuffer.Get();
    srcLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
    srcLocation.PlacedFootprint.Footprint.Format = DXGI_FORMAT_R8_UNORM;
    srcLocation.PlacedFootprint.Footprint.Width = bitmap.width;
    srcLocation.PlacedFootprint.Footprint.Height = bitmap.rows;
    srcLocation.PlacedFootprint.Footprint.Depth = 1;
    srcLocation.PlacedFootprint.Footprint.RowPitch = alignedRowPitch; // Align to 256 bytes
    srcLocation.PlacedFootprint.Offset = 0;

    D3D12_TEXTURE_COPY_LOCATION dstLocation = {};
    dstLocation.pResource = mTextTexture.Get();
    dstLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    dstLocation.SubresourceIndex = 0;

    mGraphicscommandList->CopyTextureRegion(&dstLocation, 0, 0, 0, &srcLocation, nullptr);

    // Transition texture to PIXEL_SHADER_RESOURCE state for rendering
    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = mTextTexture.Get();
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

    mGraphicscommandList->ResourceBarrier(1, &barrier);

    // At this point, 'texture' contains the bitmap data and is ready to be used as a texture in shaders
    mGraphicscommandList->Close();  // Make sure the command list is closed before executing

    // Submit the command list to the GPU and wait for execution to finish
    ID3D12CommandList* commandLists[] = { mGraphicscommandList.Get() };
    mCommandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

    mGraphicCommandObject.FlushCommandQueue();

}



void Application::BitmapToTextureResource(FT_Bitmap bitmap)
{
    //texture
    mTextTexture;
    D3D12_RESOURCE_DESC texDesc;
    texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    texDesc.Alignment = 0;
    texDesc.Width = bitmap.width;
    texDesc.Height = bitmap.rows;
    texDesc.DepthOrArraySize = 1;
    texDesc.MipLevels = 1;
    texDesc.Format = DXGI_FORMAT_R8_UNORM;
    texDesc.SampleDesc.Count = 1;
    texDesc.SampleDesc.Quality = 0;
    texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    texDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

    D3D12_HEAP_PROPERTIES uploadHeapPropertiesTex;
    uploadHeapPropertiesTex.Type = D3D12_HEAP_TYPE_DEFAULT;
    uploadHeapPropertiesTex.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    uploadHeapPropertiesTex.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    uploadHeapPropertiesTex.CreationNodeMask = 0;
    uploadHeapPropertiesTex.VisibleNodeMask = 0;


    ThrowIfFailed(mDevice->CreateCommittedResource(&uploadHeapPropertiesTex, D3D12_HEAP_FLAG_NONE,
        &texDesc, D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr, IID_PPV_ARGS(mTextTexture.GetAddressOf())));




    D3D12_PLACED_SUBRESOURCE_FOOTPRINT subFootPrint;
    UINT numRow;
    UINT64 rowSizeInByte;
    UINT64 totalByte;
    mDevice->GetCopyableFootprints(
        &texDesc, 0, 1, 0, &subFootPrint,
        &numRow, &rowSizeInByte, &totalByte);




    //default heap의  texture로 업로드



    //uploadbuffer만들기
    

    D3D12_RESOURCE_DESC uploadBufferDesc;
    uploadBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    uploadBufferDesc.Alignment = 0;
    uploadBufferDesc.Width = totalByte;// bitmap.width* bitmap.rows;// bitmap.width * bitmap.rows;
    uploadBufferDesc.Height = 1;
    uploadBufferDesc.DepthOrArraySize = 1;
    uploadBufferDesc.MipLevels = 1;
    uploadBufferDesc.Format = DXGI_FORMAT_UNKNOWN;
    uploadBufferDesc.SampleDesc.Count = 1;
    uploadBufferDesc.SampleDesc.Quality = 0;
    uploadBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    uploadBufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

    D3D12_HEAP_PROPERTIES uploadHeapProperties;
    uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
    uploadHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    uploadHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    uploadHeapProperties.CreationNodeMask = 0;
    uploadHeapProperties.VisibleNodeMask = 0;


   ThrowIfFailed( mDevice->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE,
        &uploadBufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr, IID_PPV_ARGS(uploadBuffer.GetAddressOf())));


    //uploadbuffer로 복사
   D3D12_RANGE range{ 0,0 };
   uint8_t* pData = nullptr;
  //uploadBuffer->Map(0, &range, (void**)&pData);
   uploadBuffer->Map(0, nullptr, (void**)&pData);

  // memset(pData, 255, bitmap.pitch * bitmap.rows);

   for (unsigned int row = 0; row < bitmap.rows; ++row)
   {
       memcpy(&pData[subFootPrint.Footprint.RowPitch *row], &bitmap.buffer[bitmap.width * row], bitmap.width);
   }
   
   for (unsigned int row = 0; row < bitmap.rows; ++row)
   {
       for (unsigned int i = 0; i < bitmap.width; ++i)
       {
            uint8_t t =   bitmap.buffer[i + row * bitmap.width];
        if (t != 0)
        {
            int a = 2;
        }
       }
   }

   for (unsigned int row = 0; row < bitmap.rows; ++row)
   {
       for (unsigned int i = 0; i < bitmap.width; ++i)
       {
            uint8_t t =  pData[row * 256 + i];
            if (t != 0)
            {
                int a = 2;
            }
       }
   }




   uploadBuffer->Unmap(0, nullptr);

   D3D12_RESOURCE_BARRIER uploadBufferTransferState;
   uploadBufferTransferState.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
   uploadBufferTransferState.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;

   uploadBufferTransferState.Transition.pResource = uploadBuffer.Get();
   uploadBufferTransferState.Transition.StateBefore = D3D12_RESOURCE_STATE_GENERIC_READ;
   uploadBufferTransferState.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_SOURCE;
   uploadBufferTransferState.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

   mGraphicscommandList->ResourceBarrier(1, &uploadBufferTransferState);

   //texture resource 생성



    D3D12_SUBRESOURCE_FOOTPRINT uploadBufferFootprint;
    uploadBufferFootprint.Format = DXGI_FORMAT_R8_UNORM;
    uploadBufferFootprint.Depth = 1;
    uploadBufferFootprint.Height = bitmap.rows;
    uploadBufferFootprint.Width = bitmap.width;
    uploadBufferFootprint.RowPitch = 256;
    D3D12_PLACED_SUBRESOURCE_FOOTPRINT uploadbufferPlacedFootPrint;
    uploadbufferPlacedFootPrint.Offset = 0;
    uploadbufferPlacedFootPrint.Footprint = uploadBufferFootprint;

    D3D12_TEXTURE_COPY_LOCATION src{ uploadBuffer.Get(),D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT ,subFootPrint };

    D3D12_TEXTURE_COPY_LOCATION dst{ mTextTexture.Get(),D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX,(UINT64)0 };

    mGraphicscommandList->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);
    
  // mGraphicscommandList->CopyResource(mTextTexture.Get(), uploadBuffer.Get());


   D3D12_RESOURCE_BARRIER texTransferState;
   texTransferState.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
   texTransferState.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;

   texTransferState.Transition.pResource = mTextTexture.Get();
   texTransferState.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
   texTransferState.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
   texTransferState.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
 
   mGraphicscommandList->ResourceBarrier(1, &texTransferState);




}

}



//
//int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
//{
//
//	Quad::Application app;
//	if (!app.Initialize(hInstance, nCmdShow,nullptr))
//		return 0;
//
//	return app.Run();
//}

