//#include "QuadTreeApp.h"
//
//
//
//
//
//QuadTreeApp::QuadTreeApp():mCurrMap(nullptr)
//{
//
//}
//
//bool QuadTreeApp::Initialize(HINSTANCE hInstance, int nShowCmd)
//{
//	Application::Initialize(hInstance, nShowCmd);
//
//    InitWindow();
//
//    D3D12_DESCRIPTOR_HEAP_DESC cbvSrvUavHeapDesc;
//    cbvSrvUavHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
//    cbvSrvUavHeapDesc.NodeMask = 0;
//    cbvSrvUavHeapDesc.NumDescriptors = VIEW_MAXNUM;
//    cbvSrvUavHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
//
//    mCbvSrvUavHeapManager.Initialize(mDevice, mCbvsrvdescriptorSize, cbvSrvUavHeapDesc);
//
//    D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
//    dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
//    dsvHeapDesc.NodeMask = 0;
//    dsvHeapDesc.NumDescriptors = VIEW_MAXNUM;
//    dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
//
//    mDsvHeapManager.Initialize(mDevice, mDsvdescriptorSize, dsvHeapDesc);
//
//
//    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
//    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
//    rtvHeapDesc.NodeMask = 0;
//    rtvHeapDesc.NumDescriptors = VIEW_MAXNUM;
//    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
//
//    mRtvHeapManager.Initialize(mDevice, mDsvdescriptorSize, rtvHeapDesc);
//
//
//    D3D12_DESCRIPTOR_HEAP_DESC samplerHeapDesc;
//    samplerHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
//    samplerHeapDesc.NodeMask = 0;
//    samplerHeapDesc.NumDescriptors = 30;
//    samplerHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
//
//    mSamplerHeapManager.Initialize(mDevice, mDsvdescriptorSize, samplerHeapDesc);
//
//
//
//
//
//    mDescriptorHeapManagerMaster.Initialize(&mCbvSrvUavHeapManager, &mDsvHeapManager, &mRtvHeapManager, &mSamplerHeapManager);
//
//    mMeshManager.Initialize(&mDescriptorHeapManagerMaster);
//
//    mMaterialManager.Initialize();
//    
//    mTextureManager.Initialize(&mDescriptorHeapManagerMaster);
//    
//    mMapManager.Initialize();
//
//    mResourceLoader.Initialize(mDevice, mGraphicscommandList);
//
//    mRenderSystem.Initialize(mDevice,mCommandQueue,mCommandAllocator,mGraphicscommandList,mFactory,mFence, 
//        mRenderHwnd,mRenderWindowWidth, mRenderWindowHeight, &mDescriptorHeapManagerMaster);
//
//    mResourceController.Initialize(&mMeshManager, &mMaterialManager, &mTextureManager, 
//                                    &mMapManager,&mRenderSystem ,&mResourceLoader);
//
//    mMapController.Initialize(&mRenderSystem, &mMeshManager, &mMapManager);
//
//
//    //여기까지가 모듈(시스템)초기화
//    /////////////////////////
//
//
//
//
//    //map 생성
//
//    //유연성을위해서 quadtree 공간분할 구조를 외부에서 초기화하고 map에 넣는방식으로 처리하자.
//    //그러면 octree도 사용할수있을거니깐.
//
//    //scene내에 기본적으로 필요한 object들 추가
//
//    //
//
//
//    SceneGraph* sceneGraph = new SceneGraph;
//    QuadTree* quadTree = new QuadTree;
//    mCurrMap = mMapManager.CreateMap("quadTreeMap");
//
//    //camera라도 셋팅해서 넘긴다.
//
//    mCurrMap->Initialize(sceneGraph,quadTree);
//
//
//
// 
//    return true;
//}
//
//LRESULT QuadTreeApp::RenderWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
//{
//
//    
//    switch (msg)
//    {
//    
//       
//    case WM_SIZE:
//        mRenderWindowWidth = LOWORD(lParam);
//        mRenderWindowHeight = HIWORD(lParam);
//        //최대 최소
//        //사이즈가변화고있을땐 onresize호출하지않는다.
//        if (mDevice)
//        {
//            if (wParam == SIZE_MAXIMIZED)
//            {
//                mIsMinimized = false;
//                mIsMaximized = true;
//                OnResize();
//            }
//            else if (wParam == SIZE_MINIMIZED)
//            {
//                mIsMinimized = true;
//                mIsMaximized = false;
//                //OnResize();
//
//            }
//            else if (wParam == SIZE_RESTORED)
//            {
//
//                if (mIsMinimized)
//                {
//                    mIsMinimized = false;
//                    OnResize();
//                }
//                else if (mIsMaximized)
//                {
//                    mIsMaximized = false;
//                    OnResize();
//                }
//                else if (mIsResizing)
//                {
//
//
//
//
//                }
//                else// API call such as SetWindowPos or mSwapChain->SetFullscreenState.
//                {
//                    OnResize();
//                }
//            }
//        }
//        return 0;
//    case WM_ENTERSIZEMOVE://창의크기조절테투리를 클릭할때 전달되는 메시지
//        mGameTimer.Stop();
//        mIspaused = true;
//        mIsResizing = true;
//        return 0;
//    case WM_EXITSIZEMOVE://창의크기조절테두리에서 마우스를 땟을때 
//        mGameTimer.Start();
//        mIspaused = false;
//        mIsResizing = false;
//        OnResize();
//        return 0;
//
//    case WM_GETMINMAXINFO:
//        ((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
//        ((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
//        return 0;
//
//    case WM_LBUTTONDOWN:
//    case WM_RBUTTONDOWN:
//        MouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
//
//        return 0;
//    case WM_LBUTTONUP:
//    case WM_RBUTTONUP:
//        MouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
//        return 0;
//
//    case WM_MOUSEMOVE:
//        MouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
//        return 0;
//
//
//    case WM_DESTROY://창이 파괴될때 전달되는 메시지
//        PostQuitMessage(0);
//        return 0;
//    }
//
//    return DefWindowProc(hwnd, msg, wParam, lParam);
//}
//
//
//INT_PTR QuadTreeApp::DlgCreateObjectProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
//{
//
//    switch (msg)
//    {
//
//
//    case WM_INITDIALOG:
//        return 1;
//    case WM_COMMAND:
//        switch (LOWORD(wParam))
//        {
//        case IDOK:
//            DestroyWindow(hwnd);
//            break;
//        case IDCANCEL:
//
//            DestroyWindow(hwnd);
//            break;
//
//        }
//
//        break;
//    }
//
//
//    return 0;
//
//
//
//
//    return TRUE;
//}
//
//void QuadTreeApp::Update(GameTimer& timer)
//{
//
//
//    //
//    ////scene update
//
//    mCurrMap->Update();
//
//    std::vector<Object*> objectVector;
//    mCurrMap->GetObjectVector(objectVector);
//
//
//    mRenderSystem.SetEntityVector(std::move(objectVector));
//
//    
//    //camera랑 뷰포트와 연관이있겟다.
//    //for(int camera = 0 ; camera < Actiavate_camera.size(); ++camera)
//    /*{  mCurrMap->Frustum(camera);
//
//      list[]=  mCurrMap->GetObjectList();
//    }*/
//    //그리고 이 프로젝트에서는 카메라가 어떤 다른오브젝트에 연관되어있을필요가없다.(1인칭이나3인칭게임처럼)
//    //카메라가여러개일떄 절두체 판정 병렬성을 이용해야할것
//    
//   
//    //render(list[]);
//
//
//    //controller가 각각에맞는이벤트를 수행한다.
//    //입력이들어왔다 입력이벤트가 controller에들어가고 
//    //controller는 매프레임 이벤트 큐에서 이벤트를가져와서 적절한 모듈들을 사용해서 그이벤트를처리한다.
//
//   
//
//   
//    //quadTree update또는 재구축 또는 object추가
//
//    //controller update - event  
//
//        
//
//
//     //currmap -> rendersystem으로 카메라에 보여질 object리스트를 전달
//
//    //render system에서는 material 별로 분류한후에 렌더링을하겠다.
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
//void QuadTreeApp::Draw(GameTimer& timer)
//{
//
//
//    mRenderSystem.Draw();
//
//
//
//
//}
//
//void QuadTreeApp::OnResize()
//{
//    mRenderSystem.OnResize(mRenderWindowWidth, mRenderWindowHeight);
//}
//
//void QuadTreeApp::MouseDown(WPARAM wParam, int x, int y)
//{
//}
//
//void QuadTreeApp::MouseUp(WPARAM wParam, int x, int y)
//{
//}
//
//void QuadTreeApp::MouseMove(WPARAM wParam, int x, int y)
//{
//}
//
//void QuadTreeApp::InitWindow()
//{
// /*  HWND mDlgHwnd = CreateDialog(mHinstance, MAKEINTRESOURCE(IDD_OBJECTS), mHwnd, DlgCreateObjectProc);
//
//    
//    if (mDlgHwnd != NULL)
//    {
//        ShowWindow(mDlgHwnd, SW_SHOW);
//    }
//    else
//    {
//        MessageBox(mHwnd, L"Failed to create dialog", L"Error", MB_OK | MB_ICONERROR);
//    }*/
//
//
//
//
//}
//
//
//
//
