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
//    //��������� ���(�ý���)�ʱ�ȭ
//    /////////////////////////
//
//
//
//
//    //map ����
//
//    //�����������ؼ� quadtree �������� ������ �ܺο��� �ʱ�ȭ�ϰ� map�� �ִ¹������ ó������.
//    //�׷��� octree�� ����Ҽ������Ŵϱ�.
//
//    //scene���� �⺻������ �ʿ��� object�� �߰�
//
//    //
//
//
//    SceneGraph* sceneGraph = new SceneGraph;
//    QuadTree* quadTree = new QuadTree;
//    mCurrMap = mMapManager.CreateMap("quadTreeMap");
//
//    //camera�� �����ؼ� �ѱ��.
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
//        //�ִ� �ּ�
//        //�������ȭ�������� onresizeȣ�������ʴ´�.
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
//    case WM_ENTERSIZEMOVE://â��ũ�������������� Ŭ���Ҷ� ���޵Ǵ� �޽���
//        mGameTimer.Stop();
//        mIspaused = true;
//        mIsResizing = true;
//        return 0;
//    case WM_EXITSIZEMOVE://â��ũ�������׵θ����� ���콺�� ������ 
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
//    case WM_DESTROY://â�� �ı��ɶ� ���޵Ǵ� �޽���
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
//    //camera�� ����Ʈ�� �������ְٴ�.
//    //for(int camera = 0 ; camera < Actiavate_camera.size(); ++camera)
//    /*{  mCurrMap->Frustum(camera);
//
//      list[]=  mCurrMap->GetObjectList();
//    }*/
//    //�׸��� �� ������Ʈ������ ī�޶� � �ٸ�������Ʈ�� �����Ǿ������ʿ䰡����.(1��Ī�̳�3��Ī����ó��)
//    //ī�޶󰡿������ϋ� ����ü ���� ���ļ��� �̿��ؾ��Ұ�
//    
//   
//    //render(list[]);
//
//
//    //controller�� �������´��̺�Ʈ�� �����Ѵ�.
//    //�Է��̵��Դ� �Է��̺�Ʈ�� controller������ 
//    //controller�� �������� �̺�Ʈ ť���� �̺�Ʈ�������ͼ� ������ ������ ����ؼ� ���̺�Ʈ��ó���Ѵ�.
//
//   
//
//   
//    //quadTree update�Ǵ� �籸�� �Ǵ� object�߰�
//
//    //controller update - event  
//
//        
//
//
//     //currmap -> rendersystem���� ī�޶� ������ object����Ʈ�� ����
//
//    //render system������ material ���� �з����Ŀ� ���������ϰڴ�.
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
