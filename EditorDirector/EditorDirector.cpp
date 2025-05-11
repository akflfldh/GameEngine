#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "EditorDirector/EditorDirector.h"

#include"EditorDirector/FileUiWindow.h"
#include"EditorDirector/FileUiWindowContoller.h"
#include"EditorDirector/DockingWindowController.h"
#include"EditorDirector/DragAndDropWindowController.h"
#include"EditorDirector/PopupWindowController.h"
#include"EditorDirector/FrameWindowController.h"


#include"EditorDirector/FrameWindow.h"
#include"EditorDirector/DragAndDropWindow.h"
#include"EditorDirector/GameRenderWindow.h"
#include"EditorDirector/AttributeWindow.h"
#include"EditorDirector/PopupWindow.h"

#include"Core/Application.h"
#include"HeapManager/DescriptorHeapManagerMaster.h"
#include"ResourceManager/EffectManager/EffectManager.h"
#include"System/FileUiSystem.h"
#include"System/FileUiUiSystem.h"
#include"System/WindowChromeSystem.h"
#include"System/ChildWindowDockingSystem.h"
#include"System/GamePlaySystem.h"
#include"System/GamePlayUiSystem.h"
#include"System/AttributeSystem.h"
#include"System/AttributeUiSystem.h"
#include"System/DragAndDropSystem.h"
#include"System/PopupWindowUiSystem.h"

#include"EditorDirector/EditorSystem.h"
#include"System/FrameWindowSystem.h"
#include"System/FrameWindowUiSystem.h"
#include"System/FrameWindowDockingSystem.h"

#include"Core/GraphicCommand.h"
#include"ResourceManager/MeshManager.h"
#include"ResourceManager/MaterialManager/MaterialManager.h"
#include"ResourceManager/TextureManager/TextureManager.h"
#include"ResourceManager/EffectManager/EffectManager.h"
#include"Core/MapManager.h"
#include"Core/EventDispatcher.h"

#include"Object/Camera/FrustumCamera.h"
#include"Object/Camera/OrthogoanlCamera.h"

#include"EditorDirector/PopupSystemCamera.h"
#include"EditorDirector/ChromeSystemCamera.h"
#include"EditorDirector/DockingSystemCamera.h"

#include"Collision/CollisionWorld.h"

#include"Core/VectorSpace.h"

#include"Map/Map.h"


#include"DockingWindowController.h"
#include"EditorDirector/Window.h"


#include"EditorDirector/WindowCloseButton.h"
#include"EditorDirector/WindowMinButton.h"
#include"EditorDirector/WindowMaxRestoreButton.h"


#include"Asset/Texture/RenderTargetTexture.h"

#include"EditorMap.h"
#include"Parser/JsonParser.h"


#include"ContentItemUiEntity.h"

#include"EditorAssetImportVersionModule.h"
#include"ImportModule.h"


#include"EditorSpacePartitioningStructrureFactoryImpl.h"
#include"EditorCollisionWorldFactoryImpl.h"

#include"CameraEventComponentFactory.h"

#include"Core/CameraFixWIndowResizeEventComponent.h"

#include"SpacePartitioningStructureFactory.h"
#include"SpacePartitioningStructure.h"
#include"CollisionWorldFactory.h"
#include"KeyBoard.h"

#include"DockingWindowController.h"

#undef EngineMode
Quad::EditorDirector::EditorDirector()
    :mFrameWindowSceneSwitch(0), mWindowSwitchRenderFlag(false)
{
   
    wchar_t path[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, path);

    mEditorPathW = path;
    mEditorPathA = Utility::ConvertToString(mEditorPathW, true);

}
//#define EngineMode
void Quad::EditorDirector::Initialize()
{

    auto app = Application::GetInstance();
    mDevice = app->GetD3D12Device();
    mFactory = app->GetD3DFactory();
    mGraphicCommandObject = &app->GetGraphicCommand();
    mDescriptorHeapManagerMaster = app->GetDescriptorHeapManagerMaster();
    mHinstance = app->GetHinstance();
    mImportMoudle = new ImportModule;
  
#ifdef EngineMode

    ImportModule importModule;

    //default texture
    EditorAssetImortVersionModule::ImportTexture("C:\\Users\\dongd\\gitproject\\GameEngine\\SecenGraphQuadTree\\Resource\\RawAsset\\Texture");

    //create default material
    CreateDefaultMaterial();
    
    CreateDefaultMesh();


    EditorAssetImortVersionModule::ImportMesh("C:\\Users\\dongd\\gitproject\\GameEngine\\SecenGraphQuadTree\\Resource\\RawAsset\\Mesh");


    EditorAssetImortVersionModule::SaveAsset("C:\\Users\\dongd\\gitproject\\GameEngine\\SecenGraphQuadTree\\Asset");

    int a = 2;
#else




   












    //editorModeDirector가 에디터가 가지는 기본 asset들을 로드할것이다.

    //std::vector<Asset*> editorDefaultAssetVector= mEditorModeDirector.LoadAsset("C:\\Users\\dongd\\gitproject\\GameEngine\\SecenGraphQuadTree\\Asset");

    SpacePartitioningStructureFactory<UiCollider>::GetInstance();
    std::vector<Asset*> editorAsset = mEditorModeDirector.LoadAsset("C:\\Users\\dongd\\gitproject\\GameEngine\\SecenGraphQuadTree\\Asset");

   
    InitFileUiWindow();
    InitGamePlayWindow();   //game play window 라고하는데 동시에 월드 에디터이기도한거지
    InitAttributeWindow();
    InitFrameWindow();
    InitDragAndDropWindow();
    InitPopupWindow();

 


    if (!mGraphicCommandObject->GetCloseState())
    {
        mGraphicCommandObject->ExecuteCommandList();
        mGraphicCommandObject->FlushCommandQueue();
    }


    //mResourceController.SendDefaultAssetLoadResponseEvent();

    std::vector<DockingWindowController*>normalWindowControllerVector = { mGameWindowPlayController,mFileUiWindowController,mAttributeWindowController };


    ////  mCreatingProjectDirector.Initialize(mFrameWindow, std::move(normalWindowControllerVector), mPopupWindowController, mFrameWindowController);


    mEditorModeDirector.SetGamePlayWindowChromeSystem(mGamePlayWindowChromeSystem);
    mEditorModeDirector.InitGameWindowLayoutSystem();
//    EditorModeDirector::SendAssetToFileSystem();


    //FileUiUiSystem* fileUiSystem = FileUiUiSystem::GetInstance();

    //Map* map = fileUiSystem->GetMap();

    //for (int i = 0; i < editorDefaultAssetVector.size(); ++i)
    //{
    //    //	ContentItemUiEntity* assetUiEntity = ContentItemUiEntity::Create(uiSystem, assetVector[i], ContentItemUiEntity::EType::eFileUiVersion);
    //        //ContentItemUiEntity* assetUiEntity = static_cast<ContentItemUiEntity*>(mUiSystem->GetMap()->CreateObject("ContentItemUiEntity"));
    //    ContentItemUiEntity* assetUiEntity = ContentItemUiEntity::Create(map, 0, editorDefaultAssetVector[i], ContentItemUiEntity::EType::eFileUiVersion);
    //    //assetUiEntity->Initialize(assetVector[i], ContentItemUiEntity::EType::eFileUiVersion);
    //    fileUiSystem->AddContentItemUiEntity(assetUiEntity);
    //}

   
    AddVisibleEditorAssetToFileWindow("C:\\Users\\dongd\\gitproject\\GameEngine\\EditorConfig\\EditorAssetVisibleConfig.json");






        //여기까지는 에디터를 위한내용
    mProjectDirector.Initialize(mFrameWindow, std::move(normalWindowControllerVector), mPopupWindowController, mFrameWindowController, mFrameWindowUiSystem);


   // FrameWindowMenuDirector::GetInstance();

#endif





}

void Quad::EditorDirector::PreUpdate(float deltaTime)
{
    int modeSwitchFlag = GetSwitchWindowSceneModeFlag();
    if (modeSwitchFlag == 1)
    {
        //mFrameWindow->SetProjectSelectSceneFlag(true);
        SwitchFrameWindow();
       // SwitchFrameWindow();
      //  SetSwitchWindowSceneModeFlag(0);//초기화
        mWindowSwitchRenderFlag = true;
    }
    else if (modeSwitchFlag == 2)
    {
   //     mFrameWindow->SetProjectSelectSceneFlag(false);
        SwitchCommonEditWindow();
      //  SetSwitchWindowSceneModeFlag(0);//초기화
        mWindowSwitchRenderFlag = true;
    }



}

void Quad::EditorDirector::Update(float deltaTime)
{

#ifndef EngineMode

    //int modeSwitchFlag = GetSwitchWindowSceneModeFlag();
    //if (modeSwitchFlag == 1)
    //{
    //    SwitchFrameWindow();
    //    SetSwitchWindowSceneModeFlag(0);//초기화
    //}
    //else if (modeSwitchFlag == 2)
    //{
    //    SwitchCommonEditWindow();
    //    SetSwitchWindowSceneModeFlag(0);//초기화
    //}
    //0은 상태유지

    bool popupWindowActiveFlag = mPopupWindow->GetVisibilityState();
    if (mFrameWindow->GetProjectSelectSceneFlag())
    {
        if (popupWindowActiveFlag)
            mPopupWindow->Update(deltaTime);


        mFrameWindow->Update(deltaTime);

        if (popupWindowActiveFlag)
            mPopupWindow->UploadObjectToRenderSystem();

        
        mFrameWindow->UploadObjectToRenderSystem();     //du여기문제있다

        if (popupWindowActiveFlag)
            mPopupWindow->EndUpdate(deltaTime);


        
        mFrameWindow->EndUpdate(deltaTime);

      
    }
    else
    {

        //if(//활성화되어있다면)
        if (popupWindowActiveFlag)
            mPopupWindow->Update(deltaTime);






        if (!GetPlayModeState())
        {
            mFrameWindow->Update(deltaTime);
            mFileUiWindow->Update(deltaTime);


            mAttributeWindow->Update(deltaTime);

        }

         
        mRenderWindowTest->Update(deltaTime);





        if (DragAndDropWindowController::GetWindowRunningState())
        {
            mDragAndDropWindow->Update(deltaTime);
        }



        //if(//활성화되어있다면)
        if (popupWindowActiveFlag)
            mPopupWindow->UploadObjectToRenderSystem();


        //render system으 object 전달(update내부에서 각각전달하면 다른 외부윈도우,시스템에 영향을 받아서
        //더이상 렌더링될수없는데도 이미 렌더시스템에 전달해버려서 렌더시스템이 이미 삭제되거나 렌더되면안되는 오브젝트들에대해 
        // 렌더링을 시도할 수  있는 문제가있다)

        //sendObjectToRenderSystem()
        if (!GetPlayModeState())
        {
            mFrameWindow->UploadObjectToRenderSystem();
            mFileUiWindow->UploadObjectToRenderSystem();
            mAttributeWindow->UploadObjectToRenderSystem();

        }


        mRenderWindowTest->UploadObjectToRenderSystem();





        if (DragAndDropWindowController::GetWindowRunningState())
        {
            mDragAndDropWindow->UploadObjectToRenderSystem();
        }

        //if(//활성화되어있다면)
        if (popupWindowActiveFlag)
            mPopupWindow->EndUpdate(deltaTime);


        //다음프레임을 위한 초기화 등등
        if (!GetPlayModeState())
        {
            mFrameWindow->EndUpdate(deltaTime);
            mFileUiWindow->EndUpdate(deltaTime);

            mAttributeWindow->EndUpdate(deltaTime);

        }

        mRenderWindowTest->EndUpdate(deltaTime);




        if (DragAndDropWindowController::GetWindowRunningState())
        {
            mDragAndDropWindow->EndUpdate(deltaTime);
        }

    }

#endif


}

void Quad::EditorDirector::EndUpdate(float deltaTime)
{














 

}

void Quad::EditorDirector::Draw()
{

#ifndef EngineMode
    bool popupWindowActiveFlag = mPopupWindow->GetVisibilityState();

    if (mFrameWindow->GetProjectSelectSceneFlag())
    {
        mFrameWindow->Draw();
        if (popupWindowActiveFlag)
            mPopupWindow->Draw();
    }
    else
    {

     
        if (!GetPlayModeState())
        {
            mAttributeWindow->Draw();
            mFileUiWindow->Draw();

            mFrameWindow->Draw();

            if (popupWindowActiveFlag)
                mPopupWindow->Draw();

            if (DragAndDropWindowController::GetWindowRunningState())
            {
                mDragAndDropWindow->Draw();
            }
        }
        mRenderWindowTest->Draw();

   

    }



    if (mWindowSwitchRenderFlag)
    {
        int modeSwitchFlag = GetSwitchWindowSceneModeFlag();

        bool aa =mFrameWindow->GetProjectSelectSceneFlag();
        if (modeSwitchFlag == 1)
        {
            //mFrameWindow->SetProjectSelectSceneFlag(true);
            //SwitchFrameWindow();
            mProjectDirector.SetOtherWindowVisibleOnOff(false);
            SetSwitchWindowSceneModeFlag(0);//초기화
        }
        else if (modeSwitchFlag == 2)
        {
            //mFrameWindow->SetProjectSelectSceneFlag(false);
           // SwitchCommonEditWindow();
            mProjectDirector.SetOtherWindowVisibleOnOff(true);
            SetSwitchWindowSceneModeFlag(0);//초기화
        }

        mWindowSwitchRenderFlag = false;
    }
#endif
}

void Quad::EditorDirector::SetSwitchWindowSceneModeFlag(int flag)
{
    auto instance = GetInstance();
    instance->mFrameWindowSceneSwitch = flag;
}

const int Quad::EditorDirector::GetSwitchWindowSceneModeFlag() const
{
    return mFrameWindowSceneSwitch;
}

void Quad::EditorDirector::InitGamePlayWindow()
{
    mRenderWindowTest = new GameRenderWindow(mHinstance);


    UINT gameWindowClientWidth = mRenderWindowTest->GetClientWidth();
    UINT gameWindowClientHeight = mRenderWindowTest->GetClientHeight();
    HWND gameWindowHandle = mRenderWindowTest->GetWindowHandle();


    //render system 
    RenderSystem* renderSystem = new RenderSystem;
    renderSystem->Initialize(mDevice, mFactory, mGraphicCommandObject,
        gameWindowHandle, gameWindowClientWidth, gameWindowClientHeight, mDescriptorHeapManagerMaster, GAMEWINDOW);


    AddEffect(renderSystem, EffectManager::GetEffect("Default.effect"));
    AddEffect(renderSystem, EffectManager::GetEffect("DefaultUi.effect"));
    AddEffect(renderSystem, EffectManager::GetEffect("WindowLayout.effect"));
    AddEffect(renderSystem, EffectManager::GetEffect("TextBox.effect"));
    AddEffect(renderSystem, EffectManager::GetEffect("TextCharacter.effect"));
    AddEffect(renderSystem, EffectManager::GetEffect("Line.effect"));
    AddEffect(renderSystem, EffectManager::GetEffect("DefaultCollider.effect"));
    AddEffect(renderSystem, EffectManager::GetEffect("GizmoLine.effect"));
    AddEffect(renderSystem, EffectManager::GetEffect("Gizmo.effect"));
    AddEffect(renderSystem, EffectManager::GetEffect("GizmoRotation.effect"));
    AddEffect(renderSystem, EffectManager::GetEffect("Docking.effect"));



    renderSystem->SetColliderWorldRenderState(true);







    //System을 초기화하면서 맵들, 그 맵에있는엔티티들,등등을 로드하고 초기화한다.
        //Controller
    GamePlayWindowController* wcontroller = new GamePlayWindowController(renderSystem);
    mGameWindowPlayController = wcontroller;
    wcontroller->SetName("gameWindowPlayController");
    Controller::AddController("gameWindowPlayController", wcontroller);

    GamePlaySystem* gamePlaySystem = new GamePlaySystem;
   // GamePlayUiSystem* gamePlayUiSystem = new GamePlayUiSystem;
    WindowChromeSystem* windowLayoutSystem = new WindowChromeSystem(ESystemID::eGamePlayLayoutSystem);
    mGamePlayWindowChromeSystem = windowLayoutSystem;
    ChildWindowDockingSystem* windowDockingSystem = new ChildWindowDockingSystem(ESystemID::eGamePlayDockingSystem);


    wcontroller->Initialize(mDevice, mGraphicCommandObject->GetGraphicsCommandList(), MeshManager::GetInstance(),
        MaterialManager::GetInstance(), TextureManager::GetInstance(), MapManager::GetInstance(), mRenderWindowTest,
        gamePlaySystem, windowLayoutSystem, windowDockingSystem, mDescriptorHeapManagerMaster, false);

    gamePlaySystem->SetController(wcontroller);
  //  gamePlayUiSystem->SetController(wcontroller);
    windowLayoutSystem->SetController(wcontroller);
    windowDockingSystem->SetController(wcontroller);

    wcontroller->SetWindowActiveFlag(false);


    InitGameProject(mRenderWindowTest, wcontroller, gamePlaySystem, windowLayoutSystem,
        windowDockingSystem);





    //유일하게 gameWindow만 false로 시작한다.
    //다른 window들은 항상 play mode이기때문에(true) //그리고 이미 ui,main system은 false로 설정되었다 (내부초기화때문에)
    //일단 컨트롤러에서 직접 상태를 가져오는 일이 없는거같지만 그래도 controller와 동기화해주자
    wcontroller->SetPlayMode(false);


    wcontroller->SetTitleBarSize(true, 3000, 40);
    //gamePlayUiSystem->SetDefaultController("gameWindowPlayController");

    mRenderWindowTest->Initialize(wcontroller);


    auto eventDispatcher = EventDispatcher::GetInstance();
    eventDispatcher->RegisterSystem(gamePlaySystem);
    //eventDispatcher->RegisterSystem(gamePlayUiSystem);
    eventDispatcher->RegisterSystem(windowLayoutSystem);
    eventDispatcher->RegisterSystem(windowDockingSystem);

    KeyBoard::SetHwnd(mRenderWindowTest->GetWindowHandle());
}

void Quad::EditorDirector::InitFileUiWindow()
{
    mFileUiWindow = new FileUiWindow(mHinstance);

    //window controller;
    //  Controller* controller = new MapController;// new Controller;

    //rendersystem

    //3d,ui,chrome system

    UINT clientWidth = mFileUiWindow->GetClientWidth();
    UINT clientHeight = mFileUiWindow->GetClientHeight();

    //renderSystem;
    RenderSystem* renderSystem = new RenderSystem;
    renderSystem->Initialize(mDevice,
        mFactory, mGraphicCommandObject, mFileUiWindow->GetWindowHandle(),
        clientWidth, clientHeight, mDescriptorHeapManagerMaster, FILEUIWINDOW);

 
    FileUiWindowContoller* wController = new FileUiWindowContoller(renderSystem);// new Controller;
    mFileUiWindowController = wController;

    AddEffect(renderSystem, EffectManager::GetEffect("Default.effect"));
    AddEffect(renderSystem, EffectManager::GetEffect("DefaultUi.effect"));
    AddEffect(renderSystem, EffectManager::GetEffect("WindowLayout.effect"));
    AddEffect(renderSystem, EffectManager::GetEffect("TextBox.effect"));
    AddEffect(renderSystem, EffectManager::GetEffect("TextCharacter.effect"));
    AddEffect(renderSystem, EffectManager::GetEffect("DefaultCollider.effect"));
    AddEffect(renderSystem, EffectManager::GetEffect("Docking.effect"));

    //  SetCurrentDirectory(beforeDirectory);







    wController->SetName("FileUiWindowController");
    Controller::AddController("FileUiWindowController", wController);

    //ui System;
   // FileUiSystem* mainSystem = new FileUiSystem;
    FileUiUiSystem* mainSystem = new FileUiUiSystem;
    WindowChromeSystem* windowLayoutSystem = new WindowChromeSystem(ESystemID::eFileUiLayoutSystem);
    ChildWindowDockingSystem* windowDockingSystem = new ChildWindowDockingSystem(ESystemID::eFileUiDockingSystem);
    //uiSystem->SetController(wController);
    mainSystem->SetController(wController);
    windowLayoutSystem->SetController(wController);
    windowDockingSystem->SetController(wController);
    windowDockingSystem->SetController(wController);

    wController->Initialize(mDevice, mGraphicCommandObject->GetGraphicsCommandList(), MeshManager::GetInstance(), MaterialManager::GetInstance(), TextureManager::GetInstance(),
        MapManager::GetInstance(), mFileUiWindow,mainSystem, windowLayoutSystem,
        windowDockingSystem, mDescriptorHeapManagerMaster);

    mFileUiWindow->Initialize(wController);
 
    InitFileUiUiSystem(mFileUiWindow, mainSystem);
   // InitFileUiSystem2(mFileUiWindow , mainSystem);
    InitFileUiWindowLayoutSystem(mFileUiWindow, windowLayoutSystem);
    InitChildWindowDockingSystem(mFileUiWindow, windowDockingSystem);


    wController->SetTitleBarSize(true, 3000, 40);
    wController->SetWindowActiveFlag(false);

    auto eventDispatcher = EventDispatcher::GetInstance();
    eventDispatcher->RegisterSystem(mainSystem);
  //  eventDispatcher->RegisterSystem(uiSystem);
    eventDispatcher->RegisterSystem(windowLayoutSystem);
    eventDispatcher->RegisterSystem(windowDockingSystem);


    //윈도우생성
    //윈도우컨트롤러 생성 
    //시스템들을 생성

    //윈도우 컨트롤러 초기화 ( 시스템 패싱)
    //컨트롤러 초기화에서 시스템 초기화수행
    //맵은 없는상태




    //그이후 맵추가 
    //(맵추가할때 카메라등등처리)



    KeyBoard::SetHwnd(mFileUiWindow->GetWindowHandle());

}

void Quad::EditorDirector::InitAttributeWindow()
{

    mAttributeWindow = new AttributeWindow(mHinstance);
    //  Controller* controller = new MapController;// new Controller;


    UINT clientWidth = mAttributeWindow->GetClientWidth();
    UINT clientHeight = mAttributeWindow->GetClientHeight();

    //renderSystem;
    RenderSystem* renderSystem = new RenderSystem;
    renderSystem->Initialize(mDevice,
        mFactory, mGraphicCommandObject, mAttributeWindow->GetWindowHandle(),
        clientWidth, clientHeight, mDescriptorHeapManagerMaster, ATTRIBUTEWINDOW);

    DockingWindowController* wController = new DockingWindowController(renderSystem);// new Controller;
    mAttributeWindowController = wController;

    AddEffect(renderSystem, EffectManager::GetEffect("Default.effect"));
    AddEffect(renderSystem, EffectManager::GetEffect("DefaultUi.effect"));
    AddEffect(renderSystem, EffectManager::GetEffect("WindowLayout.effect"));
    AddEffect(renderSystem, EffectManager::GetEffect("TextBox.effect"));
    AddEffect(renderSystem, EffectManager::GetEffect("TextCharacter.effect"));
    AddEffect(renderSystem, EffectManager::GetEffect("DefaultCollider.effect"));
    AddEffect(renderSystem, EffectManager::GetEffect("Line.effect"));
    AddEffect(renderSystem, EffectManager::GetEffect("ScrollListPanelUi.effect"));
    AddEffect(renderSystem, EffectManager::GetEffect("ScrollListChildPanelUi.effect"));
    AddEffect(renderSystem, EffectManager::GetEffect("Docking.effect"));














    wController->SetName("AttributeWindowController");
    Controller::AddController("AttributeWindowController", wController);
    //ui System;
    AttributeUiSystem* uiSystem = new AttributeUiSystem;
    //AttributeSystem* worldSystem = new AttributeSystem;
    WindowChromeSystem* windowLayoutSystem = new WindowChromeSystem(ESystemID::eAttributeLayoutSystem);
    ChildWindowDockingSystem* windowDockingSystem = new ChildWindowDockingSystem(ESystemID::eAttributeDockingSystem);


    mAttributeWindow->Initialize(wController);
    uiSystem->SetController(wController);
  //  worldSystem->SetController(wController);
    windowLayoutSystem->SetController(wController);
    windowDockingSystem->SetController(wController);
    windowDockingSystem->SetController(wController);



    wController->Initialize(mDevice, mGraphicCommandObject->GetGraphicsCommandList(), MeshManager::GetInstance(), MaterialManager::GetInstance(), TextureManager::GetInstance(),
        MapManager::GetInstance(), mAttributeWindow, uiSystem, windowLayoutSystem, windowDockingSystem,
        mDescriptorHeapManagerMaster);

 //   InitAttributeSystem(mAttributeWindow, worldSystem);
    InitAttributeUiSystem(mAttributeWindow, uiSystem);
    InitAttributeWindowLayoutSystem(mAttributeWindow, windowLayoutSystem);
    InitChildWindowDockingSystem(mAttributeWindow, windowDockingSystem);



    wController->SetWindowActiveFlag(false);




    wController->SetTitleBarSize(true, 3000, 40);

    auto eventDispatcher = EventDispatcher::GetInstance();
    //eventDispatcher->RegisterSystem(worldSystem);
    eventDispatcher->RegisterSystem(uiSystem);
    eventDispatcher->RegisterSystem(windowLayoutSystem);
    eventDispatcher->RegisterSystem(windowDockingSystem);


    KeyBoard::SetHwnd(mAttributeWindow->GetWindowHandle());

}

void Quad::EditorDirector::InitFrameWindow()
{



    mFrameWindow = new FrameWindow(mHinstance);
    
        //  Controller* controller = new MapController;// new Controller;
   
    
        UINT clientWidth = mFrameWindow->GetClientWidth();
        UINT clientHeight = mFrameWindow->GetClientHeight();
    
        //renderSystem;
        RenderSystem* renderSystem = new RenderSystem;
        renderSystem->Initialize(mDevice,
            mFactory, mGraphicCommandObject, mFrameWindow->GetWindowHandle(),
            clientWidth, clientHeight, mDescriptorHeapManagerMaster, FRAMEWINDOW);
    

        FrameWindowController* wController = new FrameWindowController(renderSystem);// new Controller;
        mFrameWindowController = wController;

        AddEffect(renderSystem, EffectManager::GetEffect("Default.effect"));
        AddEffect(renderSystem, EffectManager::GetEffect("DefaultUi.effect"));
        AddEffect(renderSystem, EffectManager::GetEffect("WindowLayout.effect"));
        AddEffect(renderSystem, EffectManager::GetEffect("TextBox.effect"));
        AddEffect(renderSystem, EffectManager::GetEffect("TextCharacter.effect"));
        AddEffect(renderSystem, EffectManager::GetEffect("DefaultCollider.effect"));
        AddEffect(renderSystem, EffectManager::GetEffect("ScrollListPanelUi.effect"));
        AddEffect(renderSystem, EffectManager::GetEffect("ScrollListChildPanelUi.effect"));
        AddEffect(renderSystem, EffectManager::GetEffect("Line.effect"));
        AddEffect(renderSystem, EffectManager::GetEffect("Spline.effect"));
        AddEffect(renderSystem, EffectManager::GetEffect("Thick2DSpline.effect"));
       // EffectManager::GetEffect("Default.effect");
    

    
        wController->SetName("FrameWindowController");
    
        Controller::AddController("FrameWindowController", wController);
        //ui System;
        FrameWindowUiSystem* uiSystem = new FrameWindowUiSystem;
        mFrameWindowUiSystem = uiSystem;    //ProjectDirector초기화를위해
        //FrameWindowSystem* worldSystem = new FrameWindowSystem;
    
        WindowChromeSystem* windowLayoutSystem = new WindowChromeSystem(ESystemID::eFrameLayoutSystem);
        FrameWindowDockingSystem* windowDockingSystem = new FrameWindowDockingSystem;
    
      
    
        wController->Initialize(mDevice, mGraphicCommandObject->GetGraphicsCommandList(), MeshManager::GetInstance(), MaterialManager::GetInstance(), TextureManager::GetInstance(),
            MapManager::GetInstance(), mFrameWindow, uiSystem, windowLayoutSystem,
            windowDockingSystem, mDescriptorHeapManagerMaster);
    
        wController->SetWindowActiveFlag(true);
    
        uiSystem->SetController(wController);
        //worldSystem->SetController(wController);
        windowLayoutSystem->SetController(wController);
        windowDockingSystem->SetController(wController);
    
        //InitFrameWindowSystem(mFrameWindow, worldSystem);
        InitFrameWindowUiSystem(mFrameWindow, uiSystem);
        InitFrameWindowLayoutSystem(mFrameWindow, windowLayoutSystem);
        InitFrameWindowDockingSystem(mFrameWindow, windowDockingSystem);
    
    
        mFrameWindow->Initialize(wController);
    
        mFrameWindow->SetProjectSelectSceneFlag(true);
        uiSystem->SetActiveState(true);
      //  worldSystem->SetActiveState(true);
       windowLayoutSystem->SetActiveState(true);
     //   windowDockingSystem->SetActiveState(false);
    
    
        
    
    
       // wController->SetTitleBarSize(true, 3000, 60);
        
    
        wController->InitChildWindowSetting(mRenderWindowTest, mAttributeWindow,mFileUiWindow);
        mFrameWindow->OnResize(clientWidth, clientHeight, 0);
    
        auto eventDisaptcher = EventDispatcher::GetInstance();
        //eventDisaptcher->RegisterSystem(worldSystem);
        eventDisaptcher->RegisterSystem(uiSystem);
        eventDisaptcher->RegisterSystem(windowLayoutSystem);
        eventDisaptcher->RegisterSystem(windowDockingSystem);
    
    
    
    
    
        mFrameWindowMenuDirector.Initialize(windowLayoutSystem);

        KeyBoard::SetHwnd(mFrameWindow->GetWindowHandle());


}

void Quad::EditorDirector::InitDragAndDropWindow()
{
    //mDragAndDropWindow = new DragAndDropWindow(mHinstance,;



    DragAndDropWindowController* controller = new DragAndDropWindowController;



    DragAndDropSystem* uiSystem = new DragAndDropSystem;


    Map* map = EditorSystem::CreateMap(uiSystem, "DragAndDropWindowMainMap", false, false);
    //map->SetName("DragAndDropWindowMainMap");
  //  map->Initialize(uiSystem);

   // VectorSpace* vectorSpace = new VectorSpace;
   // vectorSpace->Initialize(600);




   // CollisionWorld* collisionWorld = new CollisionWorld(vectorSpace);

  //  map->CreateMapLayer(0, 0, nullptr, collisionWorld, { 0,0,(float)300, (float)100 ,0.0f,1.0f });

    map->CreateMapLayer(0, 0, nullptr, { 0,0,(float)300,(float)400,0.0f,1.0f });
    //default 3D - mapLayer 1
    map->CreateMapLayer(0, 0, nullptr, { 0,0,(float)300,(float)400,0.0f,1.0f });



    //ui collisionWorld
    auto spacePartitioningStructureFactoryUi = SpacePartitioningStructureFactory<UiCollider>::GetInstance();
    SpacePartitioningStructure<UiCollider>* spacePartitioningStructureUi = spacePartitioningStructureFactoryUi->CreateSpacePartitioningStructure("VectorSpace");

    auto collisionWorldFactory = CollisionWorldFactory::GetInstance();
    UiCollisionWorld* CollisionWorldUi = collisionWorldFactory->CreateUiCollisionWorld(spacePartitioningStructureUi);

    map->SetDefaultUiCollisionWorld(CollisionWorldUi);



    //3d collisionWorld
    auto spacePartitioningStructureFactory3D = SpacePartitioningStructureFactory<Collider>::GetInstance();
    SpacePartitioningStructure<Collider>* spacePartitioningStructure3D = spacePartitioningStructureFactory3D->CreateSpacePartitioningStructure("VectorSpace");

    CollisionWorld* CollisionWorld3D = collisionWorldFactory->CreateCollisionWorld(spacePartitioningStructure3D);

    map->SetDefault3DCollisionWorld(CollisionWorld3D);








    uiSystem->Initialize(300, 100, map);

    Map* currMap = uiSystem->GetMap();

    //   OrthogoanlCamera* camera = static_cast<OrthogoanlCamera*>(currMap->CreateObject("OrthogoanlCamera"));
    OrthogoanlCamera* camera = OrthogoanlCamera::Create(map, 0, 300, 100);

    //  camera->Initialize(300, 100);
    currMap->SetMainCamera(camera);

    mDragAndDropWindow = new DragAndDropWindow(mHinstance);
    mDragAndDropWindow->Initialize(controller, 300, 100);

    mDragAndDropRenderSystem.Initialize(mDevice, mFactory,
        mGraphicCommandObject, mDragAndDropWindow->GetWindowHandle(), 300, 100, mDescriptorHeapManagerMaster, DRAGANDDROPWINDOW);


    AddEffect(&mDragAndDropRenderSystem, EffectManager::GetEffect("Default.effect"));
    AddEffect(&mDragAndDropRenderSystem, EffectManager::GetEffect("DefaultUi.effect"));
    AddEffect(&mDragAndDropRenderSystem, EffectManager::GetEffect("WindowLayout.effect"));
    AddEffect(&mDragAndDropRenderSystem, EffectManager::GetEffect("TextBox.effect"));
    AddEffect(&mDragAndDropRenderSystem, EffectManager::GetEffect("TextCharacter.effect"));
    AddEffect(&mDragAndDropRenderSystem, EffectManager::GetEffect("DefaultCollider.effect"));






    //드래그앤드랍윈도우에대한 고유의 시스템이 필요하다.


    controller->Initialize(mDragAndDropWindow, uiSystem, &mDragAndDropRenderSystem);


    auto eventDispatcher = EventDispatcher::GetInstance();

    eventDispatcher->RegisterSystem(uiSystem);



}

void Quad::EditorDirector::InitPopupWindow()
{
    PopupWindowUiSystem* uiSystem = new PopupWindowUiSystem;
    Map* map = EditorSystem::CreateMap(uiSystem, "PopupUiSystem", false, false);
    // map->Initialize(uiSystem);



 //default Ui - mapLayer 0
    map->CreateMapLayer(0, 0, nullptr, { 0,0,(float)300,(float)400,0.0f,1.0f });
    //default 3D - mapLayer 1
    map->CreateMapLayer(0, 0, nullptr, { 0,0,(float)300,(float)400,0.0f,1.0f });



    //ui collisionWorld
    auto spacePartitioningStructureFactoryUi = SpacePartitioningStructureFactory<UiCollider>::GetInstance();
    SpacePartitioningStructure<UiCollider>* spacePartitioningStructureUi = spacePartitioningStructureFactoryUi->CreateSpacePartitioningStructure("VectorSpace");

    auto collisionWorldFactory = CollisionWorldFactory::GetInstance();
    UiCollisionWorld* CollisionWorldUi = collisionWorldFactory->CreateUiCollisionWorld(spacePartitioningStructureUi);

    map->SetDefaultUiCollisionWorld(CollisionWorldUi);



    //3d collisionWorld
    auto spacePartitioningStructureFactory3D = SpacePartitioningStructureFactory<Collider>::GetInstance();
    SpacePartitioningStructure<Collider>* spacePartitioningStructure3D = spacePartitioningStructureFactory3D->CreateSpacePartitioningStructure("VectorSpace");

   // auto collisionWorldFactory = CollisionWorldFactory::GetInstance();
    CollisionWorld* CollisionWorld3D = collisionWorldFactory->CreateCollisionWorld(spacePartitioningStructure3D);

    map->SetDefault3DCollisionWorld(CollisionWorld3D);











  //  map->CreateMapLayer(0, 0, nullptr, collisionWorld, { 0,0,(float)300, (float)400 ,0.0f,1.0f });


   

    //   Camera* camera = new PopupSystemCamera(L"PopupSystemCamera");



    PopupWindowController* controller = new PopupWindowController(&mPopupRenderSystem);
    mPopupWindowController = controller;


    mPopupWindow = new PopupWindow(mHinstance, 300, 400, false);
    mPopupWindow->Initialize(controller);

    uiSystem->Initialize(300, 400, map);
    uiSystem->SetController(controller);

    Map* currMap = uiSystem->GetMap();


    // PopupSystemCamera* camera = static_cast<PopupSystemCamera*>(currMap->CreateObject("PopupSystemCamera"));
    PopupSystemCamera* camera = PopupSystemCamera::Create(currMap, 0, 300, 400);
    //  camera->Initialize(300, 400);
    camera->SetPosition(0, 0, 0);
    currMap->SetMainCamera(camera);

    mPopupRenderSystem.Initialize(mDevice, mFactory, mGraphicCommandObject, mPopupWindow->GetWindowHandle(), 300, 400, mDescriptorHeapManagerMaster, POPUPWINDOW);

    mPopupRenderSystem.SetBackgroundColor(0.2f, 0.2f, 0.2f, 1.0f);
    controller->Initialize(mPopupWindow, uiSystem);

   // AddEffect(&mPopupRenderSystem, EffectManager::GetEffect("Default.effect"));
    AddEffect(&mPopupRenderSystem, EffectManager::GetEffect("DefaultUi.effect"));
    // AddEffect(&mPopupRenderSystem, EffectTable::GetEffect(L"WindowLayout.effect"));
    AddEffect(&mPopupRenderSystem, EffectManager::GetEffect("TextBox.effect"));
    AddEffect(&mPopupRenderSystem, EffectManager::GetEffect("TextCharacter.effect"));
    // AddEffect(mPopupRenderSystem, EffectTable::GetEffect(L"DefaultCollider.effect"));




    KeyBoard::SetHwnd(mPopupWindow->GetWindowHandle());

}

void Quad::EditorDirector::InitGameProject(TaskWindow* window, DockingWindowController* controller, GamePlaySystem* gamePlaySystem,  WindowChromeSystem* windowLayoutSystem, ChildWindowDockingSystem* windowDockingSystem)
{
    //InitGamePlayUiSystem(window, gamePlayUiSystem);
    InitGamePlaySystem(window, gamePlaySystem);
    InitGamePlayWindowLayoutSystem(window, windowLayoutSystem);
    InitChildWindowDockingSystem(window, windowDockingSystem);

    //gamePlayUiSystem->SetController(controller);
    gamePlaySystem->SetController(controller);
    windowLayoutSystem->SetController(controller);
    windowDockingSystem->SetController(controller);

}

void Quad::EditorDirector::InitGamePlaySystem(TaskWindow* window, GamePlaySystem* system)
{
    system->Initialize(window->GetClientWidth(), window->GetClientHeight(), nullptr);
}

void Quad::EditorDirector::InitGamePlayUiSystem(TaskWindow* window, GamePlayUiSystem* system)
{
   // system->Initialize(window->GetClientWidth(), window->GetClientHeight(), nullptr);
}

//void Quad::EditorDirector::InitAttributeSystem(TaskWindow* window, AttributeSystem* system)
//{
//
//
//    Map* map = EditorSystem::CreateMap(system, "AttrMainMap", false, false);
//    //  map->SetName("AttrMainMap");
//    //  map->Initialize(system);
//   /* map->CreateMapLayer(0, 0, nullptr, collisionWorld, { 0,0,(float)window->GetClientWidth(), (float)window->GetClientHeight() ,0.0f,1.0f });*/
//
//    initDefaultMapSetting(window, map);
//    // Camera* gameCamera = new Camera(L"GameCamera");
//
//
//
//    system->Initialize(window->GetClientWidth(), window->GetClientHeight(), map);
//    Map* currMap = system->GetMap();
//    // FrustumCamera* gameCamera = static_cast<FrustumCamera*>(currMap->CreateObject("FrustumCamera"));
//    FrustumCamera* gameCamera = FrustumCamera::Create(currMap, 0, DirectX::XM_PI / 2, (float)window->GetClientWidth() / window->GetClientHeight());
//    //  gameCamera->Initialize(DirectX::XM_PI / 2, (float)window->GetClientWidth() / window->GetClientHeight());
//    currMap->SetMainCamera(gameCamera);
//    // map->SetCameraType(ECameraType::ePerspectiveCamera);
//    currMap->GetMainCamera()->GetTransform().SetPositionLocal({ 0,0,-10.0f });
//
//}

void Quad::EditorDirector::InitAttributeUiSystem(TaskWindow* window, AttributeUiSystem* system)
{
    

    Map* map = EditorSystem::CreateMap(system, "AttrUiMainMap", false, false);
    // map->SetName("AttrUiMainMap");
    // map->Initialize(system);
    initDefaultMapSetting(window, map);


    map->SetViewportAutoFlag(false, 1);


    RenderTargetTexture* renderTargetTexture = TextureManager::CreateRenderTargetTexture("AnimationEdit3DPanelTexture", 300, 300);
    //TextureManager::AddTexture(renderTargetTexture, L"AnimationEdit3DPanelTexture");
    renderTargetTexture->SetEngineContentItemFlag(true);

    Texture* depthStencilBuffer = TextureManager::CreateDepthStencilBuffer("AnimationEdit3DPanelDepthStencilBuffer", 300, 300);
    //map layer 1 
    depthStencilBuffer->SetEngineContentItemFlag(true);
  //  vectorSpace = new VectorSpace;
  //  vectorSpace->Initialize(100);
 //   CollisionWorld* animationEdit3DCollisionWorld = new CollisionWorld(vectorSpace);


  //  map->CreateMapLayer(1, 1, nullptr, animationEdit3DCollisionWorld, { 0,0,300,300,0.0f,1.0f }, renderTargetTexture, //depthStencilBuffer);

    MapLayer & default3DMapLayer =  map->GetMapLayer(1);
    default3DMapLayer.mID = 1;
    default3DMapLayer.mDepthPriority = 1;
    default3DMapLayer.mViewPort = { 0,0,300,300,0.0f,1.0f };
    default3DMapLayer.mRenderTarget = renderTargetTexture;
    default3DMapLayer.mDepthStencilBuffer = depthStencilBuffer;

    //  Camera* animationEdit3DCamera = new Camera(L"AnimationEdit3DCamera");


    system->Initialize(window->GetClientWidth(), window->GetClientHeight(), map);

    Map* currMap = system->GetMap();


    // OrthogoanlCamera* gameCamera = static_cast<OrthogoanlCamera*>(currMap->CreateObject("OrthogoanlCamera"));
    OrthogoanlCamera* gameCamera = OrthogoanlCamera::Create(currMap, 0, window->GetClientWidth(), window->GetClientHeight());
    // gameCamera->Initialize(window->GetClientWidth(), window->GetClientHeight());

    currMap->SetMainCamera(gameCamera);



    //  FrustumCamera* animationEdit3DCamera = static_cast<FrustumCamera*>(currMap->CreateObject("FrustumCamera", 1));
    FrustumCamera* animationEdit3DCamera = FrustumCamera::Create(currMap, 0, 1000, 1000);
    //    animationEdit3DCamera->Initialize(1000, 1000);
    currMap->SetMainCamera(animationEdit3DCamera, 1);


    //map->SetCameraType(ECameraType::ePerspectiveCamera);
    currMap->GetMainCamera()->GetTransform().SetPositionLocal({ 0,0,0.0f });


    currMap->GetMainCamera()->GetTransform().SetPositionLocal({ (float)window->GetClientWidth() / 2,-1.0f * (float)window->GetClientHeight() / 2 , 0 });



}

void Quad::EditorDirector::InitFileUiUiSystem(TaskWindow* window, FileUiUiSystem* system)
{
   // VectorSpace* vectorSpace = new VectorSpace;
   // vectorSpace->Initialize(100000000);
   // CollisionWorld* collisionWorld = new CollisionWorld(vectorSpace);


    Map* map = EditorSystem::CreateMap(system, "FileUiUiMainMap", false, false);
    //  map->SetName("FileUiUiMainMap");
    //  map->Initialize(system);

    initDefaultMapSetting(window, map);


    //gameCamera를 설정은했는데 맵으로 들어가진않은거지

    system->Initialize(window->GetClientWidth(), window->GetClientHeight(), map);
    //Map* currMap = system->GetMap();

    //  OrthogoanlCamera* gameCamera = static_cast<OrthogoanlCamera*>(currMap->CreateObject("OrthogoanlCamera"));
    OrthogoanlCamera* gameCamera = OrthogoanlCamera::Create(map, 0, window->GetClientWidth(), window->GetClientHeight());
    //  gameCamera->Initialize(window->GetClientWidth(), window->GetClientHeight());

    map->SetMainCamera(gameCamera);
    //map->SetCameraType(ECameraType::ePerspectiveCamera);
    map->GetMainCamera()->GetTransform().SetPositionLocal({ 0,0,-10.0f });

}

void Quad::EditorDirector::InitFileUiSystem2(TaskWindow* window, FileUiSystem* system)
{
 

    Map* map = EditorSystem::CreateMap(system, "FileUiMainMap", false, false);
    //   map->SetName("FileUiMainMap");
     //  map->Initialize(system);
    initDefaultMapSetting(window, map);


    system->Initialize(window->GetClientWidth(), window->GetClientHeight(), map);
    Map* currMap = system->GetMap();
    //  OrthogoanlCamera* gameCamera = static_cast<OrthogoanlCamera*>(currMap->CreateObject("OrthogoanlCamera"));
    OrthogoanlCamera* gameCamera = OrthogoanlCamera::Create(currMap, 0, window->GetClientWidth(), window->GetClientHeight());
    //  gameCamera->Initialize(window->GetClientWidth(), window->GetClientHeight());

    currMap->SetMainCamera(gameCamera);
    //map->SetCameraType(ECameraType::ePerspectiveCamera);
    currMap->GetMainCamera()->GetTransform().SetPositionLocal({ 0,0,0.0f });


}






//void Quad::EditorDirector::InitFrameWindowSystem(TaskWindow* window, FrameWindowSystem* system)
//{
//
//   
//
//    Map* map = EditorSystem::CreateMap(system, "FrameWindowMainMap", false, false);
//
//    initDefaultMapSetting(window, map);
//
//
//    system->Initialize(window->GetClientWidth(), window->GetClientHeight(), map);
//
//    Map* currMap = system->GetMap();
//    //OrthogoanlCamera* gameCamera = static_cast<OrthogoanlCamera*>(currMap->CreateObject("OrthogoanlCamera"));
//    OrthogoanlCamera* gameCamera = OrthogoanlCamera::Create(currMap, 0, window->GetClientWidth(), window->GetClientHeight());
//    //  gameCamera->Initialize(window->GetClientWidth(), window->GetClientHeight());
//
//
//    currMap->SetMainCamera(gameCamera);
//
//    //map->SetCameraType(ECameraType::ePerspectiveCamera);
//
//    currMap->GetMainCamera()->GetTransform().SetPositionLocal({ 0,0,0.0f });
//}

void Quad::EditorDirector::InitFrameWindowUiSystem(TaskWindow* window, FrameWindowUiSystem* system)
{
    

    Map* map = EditorSystem::CreateMap(system, "FrameWindowUiMainMap", false, false);
    // map->SetName("FrameWindowUiMainMap");
   //  map->Initialize(system);


    initDefaultMapSetting(window, map);



    system->Initialize(window->GetClientWidth(), window->GetClientHeight(), map);
    Map* currMap = system->GetMap();
    // OrthogoanlCamera* gameCamera = static_cast<OrthogoanlCamera*>(currMap->CreateObject("OrthogoanlCamera"));
    OrthogoanlCamera* gameCamera = OrthogoanlCamera::Create(currMap, 0, window->GetClientWidth(), window->GetClientHeight());

    //  gameCamera->Initialize(window->GetClientWidth(), window->GetClientHeight());

    currMap->SetMainCamera(gameCamera);
    // map->SetCameraType(ECameraType::ePerspectiveCamera);
    currMap->GetMainCamera()->GetTransform().SetPositionLocal({ (float)window->GetClientWidth() / 2, (float)-window->GetClientHeight() / 2,0.0f });
    system->SetViewPort(0, 60, (float)window->GetClientWidth(), (float)window->GetClientHeight(), 0.0f, 1.0f);


    auto cameraEventComponentFactory = CameraEventComponentFactory::GetInstance();

    CameraFixWindowResizeEventComponent * cameraFixWindowResizeEventComponent =  cameraEventComponentFactory->CreateComponent< CameraFixWindowResizeEventComponent>();

    cameraFixWindowResizeEventComponent->Initialize(gameCamera);

    RegisterCameraEventComponentHelperMethod(gameCamera, cameraFixWindowResizeEventComponent);


}

void Quad::EditorDirector::InitGamePlayWindowLayoutSystem(TaskWindow* window, WindowChromeSystem* system)
{

  //  VectorSpace* vectorSpace = new VectorSpace;
  //  vectorSpace->Initialize(100);
  //  CollisionWorld* collisionWorld = new CollisionWorld(vectorSpace);


    Map* map = EditorSystem::CreateMap(system, "GamePlayLayoutMainMap", false, false);
    //  map->SetName("GamePlayLayoutMainMap");
    //  map->Initialize(system);
  //  map->CreateMapLayer(0, 0, nullptr, collisionWorld, { 0,0,(float)window->GetClientWidth(), (float)window->GetClientHeight() ,0.0f,1.0f });


    initDefaultMapSetting(window, map);


    system->Initialize(window->GetClientWidth(), window->GetClientHeight(), map);

    Map* currMap = system->GetMap();

    //ChromeSystemCamera* gameCamera = static_cast<ChromeSystemCamera*>(currMap->CreateObject("ChromeSystemCamera"));
    ChromeSystemCamera* gameCamera = ChromeSystemCamera::Create(currMap, 0, window->GetClientWidth(), window->GetClientHeight());

    // gameCamera->SetSystem(system);
    // gameCamera->Initialize(window->GetClientWidth(), window->GetClientHeight());

    currMap->SetMainCamera(gameCamera);



    // system->SetTitleBarSize(3000, 40);

   //  WindowCloseButton* windowCloseButton = new WindowCloseButton("WindoeCloseButton");
   //  WindowCloseButton* windowCloseButton = static_cast<WindowCloseButton*>(currMap->CreateObject("WindowCloseButton"));
    WindowCloseButton* windowCloseButton = WindowCloseButton::Create(currMap, 0);
    //windowCloseButton->SetSystem(system);
  //  windowCloseButton->Initialize();
    windowCloseButton->SetKeepVisibleBaseHorizontalLineOffset(20);
    windowCloseButton->SetKeepVisibleBaseVerticalLineOffset(20);


}

void Quad::EditorDirector::InitAttributeWindowLayoutSystem(TaskWindow* window, WindowChromeSystem* system)
{

    Map* map = EditorSystem::CreateMap(system, "AttrLayoutMainMap", false, false);


    initDefaultMapSetting(window, map);


    system->Initialize(window->GetClientWidth(), window->GetClientHeight(), map);

    Map* currMap = system->GetMap();
    // system->SetTitleBarSize(3000, 40);
    // ChromeSystemCamera* gameCamera = static_cast<ChromeSystemCamera*>(currMap->CreateObject("ChromeSystemCamera"));
    ChromeSystemCamera* gameCamera = ChromeSystemCamera::Create(currMap, 0, window->GetClientWidth(), window->GetClientHeight());

    //  gameCamera->SetSystem(system);
     // gameCamera->Initialize(window->GetClientWidth(), window->GetClientHeight());

    currMap->SetMainCamera(gameCamera);


    //  WindowCloseButton* windowCloseButton = static_cast<WindowCloseButton*>(currMap->CreateObject("WindowCloseButton"));
    WindowCloseButton* windowCloseButton = WindowCloseButton::Create(currMap, 0);
    // windowCloseButton->SetSystem(system);
  //   windowCloseButton->Initialize();
    windowCloseButton->SetKeepVisibleBaseHorizontalLineOffset(20);
    windowCloseButton->SetKeepVisibleBaseVerticalLineOffset(20);
    windowCloseButton->SetDepth(2);
}

void Quad::EditorDirector::InitFileUiWindowLayoutSystem(TaskWindow* window, WindowChromeSystem* system)
{
   

    Map* map = EditorSystem::CreateMap(system, "FileUiLayoutMainMap", false, false);

    // map->SetName("FileUiLayoutMainMap");
   //  map->Initialize(system);

    initDefaultMapSetting(window, map);


    // map->AddObject(gameCamera);
    system->Initialize(window->GetClientWidth(), window->GetClientHeight(), map);
    // system->SetTitleBarSize(3000, 40);

    Map* currMap = system->GetMap();
    // ChromeSystemCamera* gameCamera = static_cast<ChromeSystemCamera*>(currMap->CreateObject("ChromeSystemCamera"));
    ChromeSystemCamera* gameCamera = ChromeSystemCamera::Create(currMap, 0, window->GetClientWidth(), window->GetClientHeight());

    // gameCamera->SetSystem(system);
     //gameCamera->Initialize(window->GetClientWidth(), window->GetClientHeight());

    currMap->SetMainCamera(gameCamera);

    //   WindowCloseButton* windowCloseButton = new WindowCloseButton("WindoeCloseButton");
      // WindowCloseButton* windowCloseButton = static_cast<WindowCloseButton*>(currMap->CreateObject("WindowCloseButton"));
    WindowCloseButton* windowCloseButton = WindowCloseButton::Create(currMap, 0);
    //windowCloseButton->SetSystem(system);
  //  windowCloseButton->Initialize();
    windowCloseButton->SetKeepVisibleBaseHorizontalLineOffset(20);
    windowCloseButton->SetKeepVisibleBaseVerticalLineOffset(20);
}

void Quad::EditorDirector::InitFrameWindowLayoutSystem(TaskWindow* window, WindowChromeSystem* system)
{
   



    Map* map = EditorSystem::CreateMap(system, "FrameWIindowLayoutMainMap", false, false);
    //  map->SetName("FrameWIindowLayoutMainMap");
     // map->Initialize(system);
    initDefaultMapSetting(window, map);

    //map->AddObject(gameCamera);
    system->Initialize(window->GetClientWidth(), window->GetClientHeight(), map);

    Map* currMap = system->GetMap();
    int mapLayerIndex = 0;
    //  ChromeSystemCamera* gameCamera = static_cast<ChromeSystemCamera*>(currMap->CreateObject("ChromeSystemCamera"));
    ChromeSystemCamera* gameCamera = ChromeSystemCamera::Create(currMap, 0, window->GetClientWidth(), window->GetClientHeight());


    //  gameCamera->SetSystem(system);
    // gameCamera->Initialize(window->GetClientWidth(), window->GetClientHeight());

    currMap->SetMainCamera(gameCamera);



    //system->SetTitleBarSize(3000, 60);

   // WindowCloseButton* windowCloseButton = new WindowCloseButton("WindoeCloseButton");
 //   WindowCloseButton* windowCloseButton = static_cast<WindowCloseButton*>(currMap->CreateObject("WindowCloseButton"));
    WindowCloseButton* windowCloseButton = WindowCloseButton::Create(currMap, mapLayerIndex);
    //   windowCloseButton->SetSystem(system);
     //  windowCloseButton->Initialize();
    windowCloseButton->SetKeepVisibleBaseHorizontalLineOffset(20);
    windowCloseButton->SetKeepVisibleBaseVerticalLineOffset(20);
    windowCloseButton->SetDepth(2);

    //   system->AddEntity(windowCloseButton);


       //프레임윈도우 처음resize가 먼저 전달되서 안보이는것같다 

    //   WindowMaxRestoreButton* windowMaxRestoreButton = new WindowMaxRestoreButton("WindowMaxRestoreButton");
      // WindowMaxRestoreButton* windowMaxRestoreButton = static_cast<WindowMaxRestoreButton*>(currMap->CreateObject("WindowMaxRestoreButton"));
    WindowMaxRestoreButton* windowMaxRestoreButton = WindowMaxRestoreButton::Create(currMap, mapLayerIndex);
    //  windowMaxRestoreButton->SetSystem(system);
    //  windowMaxRestoreButton->Initialize();
    windowMaxRestoreButton->SetKeepVisibleBaseHorizontalLine(0);
    windowMaxRestoreButton->SetKeepVisibleBaseVerticalLine(1);
    windowMaxRestoreButton->SetKeepVisibleBaseHorizontalLineOffset(20);
    windowMaxRestoreButton->SetKeepVisibleBaseVerticalLineOffset(60);

    // system->AddEntity(windowMaxRestoreButton);

    // WindowMinButton* windowMinButton = new WindowMinButton("WindowMinButton");
    // WindowMinButton* windowMinButton = static_cast<WindowMinButton*>(currMap->CreateObject("WindowMinButton"));
    WindowMinButton* windowMinButton = WindowMinButton::Create(currMap, mapLayerIndex);
    // windowMinButton->SetSystem(system);
   //  windowMinButton->Initialize();
    windowMinButton->SetKeepVisibleBaseHorizontalLine(0);
    windowMinButton->SetKeepVisibleBaseVerticalLine(1);
    windowMinButton->SetKeepVisibleBaseHorizontalLineOffset(20);
    windowMinButton->SetKeepVisibleBaseVerticalLineOffset(100);

    //  system->AddEntity(windowMinButton);




}

void Quad::EditorDirector::InitFrameWindowDockingSystem(TaskWindow* window, FrameWindowDockingSystem* system)
{

 
    Map* map = EditorSystem::CreateMap(system, "FrameWindowDockingMainMap", false, false);
  
    initDefaultMapSetting(window, map);

    // map->AddObject(gameCamera);
    system->Initialize(window->GetClientWidth(), window->GetClientHeight(), map);

    Map* currMap = system->GetMap();
    //  DockingSystemCamera* gameCamera = static_cast<DockingSystemCamera*>(currMap->CreateObject("DockingSystemCamera"));
    DockingSystemCamera* gameCamera = DockingSystemCamera::Create(currMap, 0, window->GetClientWidth(), window->GetClientHeight());
    // DockingSystemCamera* gameCamera = ObjectFactory::CreateObject()
   // gameCamera->Initialize(window->GetClientWidth(), window->GetClientHeight());

    currMap->SetMainCamera(gameCamera);
}

void Quad::EditorDirector::InitChildWindowDockingSystem(TaskWindow* window, ChildWindowDockingSystem* system)
{
    



    Map* map = EditorSystem::CreateMap(system, "ChildWindowDockingMainMap" + std::to_string((int)system->GetSystemID()), false, false);
   
    initDefaultMapSetting(window, map);

    system->Initialize(window->GetClientWidth(), window->GetClientHeight(), map);
    Map* currMap = system->GetMap();
    //  DockingSystemCamera* gameCamera = static_cast<DockingSystemCamera*>(currMap->CreateObject("DockingSystemCamera"));
    DockingSystemCamera* gameCamera = DockingSystemCamera::Create(currMap, 0, window->GetClientWidth(), window->GetClientHeight());

    //gameCamera->Initialize(window->GetClientWidth(), window->GetClientHeight());

    currMap->SetMainCamera(gameCamera);

}

void Quad::EditorDirector::initDefaultMapSetting(TaskWindow* window, Map* map)
{
    //default Ui - mapLayer 0
    map->CreateMapLayer(0, 0, nullptr, { 0,0,(float)window->GetClientWidth(),(float)window->GetClientHeight(),0.0f,1.0f });
    //default 3D - mapLayer 1
    map->CreateMapLayer(1, 0, nullptr, { 0,0,(float)window->GetClientWidth(),(float)window->GetClientHeight(),0.0f,1.0f });

  
    //new SpacePartitioningStructureFactory<UiCollider>(nullptr);
    //ui collisionWorld
    auto spacePartitioningStructureFactoryUi = SpacePartitioningStructureFactory<UiCollider>::GetInstance();
    SpacePartitioningStructure<UiCollider>* spacePartitioningStructureUi = spacePartitioningStructureFactoryUi->CreateSpacePartitioningStructure("VectorSpace"); 

    auto collisionWorldFactory = CollisionWorldFactory::GetInstance();
    UiCollisionWorld* CollisionWorldUi = collisionWorldFactory->CreateUiCollisionWorld(spacePartitioningStructureUi);

    map->SetDefaultUiCollisionWorld(CollisionWorldUi);



    //3d collisionWorld
    auto spacePartitioningStructureFactory3D = SpacePartitioningStructureFactory<Collider>::GetInstance();
    SpacePartitioningStructure<Collider>* spacePartitioningStructure3D = spacePartitioningStructureFactory3D->CreateSpacePartitioningStructure("VectorSpace");

    //auto collisionWorldFactory = CollisionWorldFactory::GetInstance();
    CollisionWorld* CollisionWorld3D = collisionWorldFactory->CreateCollisionWorld(spacePartitioningStructure3D);

    map->SetDefault3DCollisionWorld(CollisionWorld3D);






}

void Quad::EditorDirector::AddVisibleEditorAssetToFileWindow(const std::string& configFilePath)
{


    JsonParser::ReadFile(configFilePath);
    JsonParser::ReadStart();



    rapidjson::Value::ConstArray  textureArray = JsonParser::ReadArray("Texture");
    std::vector<Asset*> assetVector;

    for (auto& element : textureArray)
    {

        std::string textureName= element.GetString();
        Asset * asset =  TextureManager::GetTexture(textureName);
        if (asset != nullptr)
        {
            assetVector.push_back(asset);
        }
     }

    rapidjson::Value::ConstArray  matArray = JsonParser::ReadArray("Material");

    for (auto& element : matArray)
    {

        std::string matName = element.GetString();
        Asset* asset = MaterialManager::GetMaterial(matName);
        if (asset != nullptr)
        {
            assetVector.push_back(asset);
        }
    }


    rapidjson::Value::ConstArray  meshArray = JsonParser::ReadArray("Mesh");
    for (auto& element : meshArray)
    {

        std::string meshName = element.GetString();
        Asset* asset = MeshManager::GetMesh(meshName);
        if (asset != nullptr)
        {
            assetVector.push_back(asset);
        }
    }


    FileUiUiSystem* fileUiSystem = FileUiUiSystem::GetInstance();
    fileUiSystem->AddAsset(assetVector);
   

}

void Quad::EditorDirector::CreateDefaultMaterial()
{

   auto * materialManager =  MaterialManager::GetInstance();

   const std::string defaultMaterialName = "Default";
   Material * defaultMaterial =  materialManager->CreateMaterial(defaultMaterialName);

   defaultMaterial->SetDiffuseMap(TextureManager::GetTexture("Default.bmp"));

  
 }

void Quad::EditorDirector::CreateDefaultMesh()
{
    //Squre Mesh

    MeshManager* meshManager = MeshManager::GetInstance();
    StaticMesh * squareMesh  =static_cast<StaticMesh*>( meshManager->CreateMesh("Rectangle", Quad::EMeshType::eStaticMesh));

    
    
    std::vector<StaticVertex> vertexVector(4);
    std::vector<MeshIndexType> indexVector(6);
   

    vertexVector[0].mPos = { -0.5f,0.5f,0.0f };
    vertexVector[1].mPos = { 0.5f,0.5f,0.0f };
    vertexVector[2].mPos = { 0.5f,-0.5f,0.0f };
    vertexVector[3].mPos = { -0.5f,-0.5f,0.0f };


    vertexVector[0].mNormal = { 0.0f,0.0f,-1.0f };
    vertexVector[1].mNormal = { 0.0f,0.0f,-1.0f };
    vertexVector[2].mNormal = { 0.0f,0.0f,-1.0f };
    vertexVector[3].mNormal = { 0.0f,0.0f,-1.0f };


    vertexVector[0].mTex = { 0.0f,0.0f };
    vertexVector[1].mTex = { 1.0f,0.0f };
    vertexVector[2].mTex = { 1.0f,1.0f };
    vertexVector[3].mTex = { 0.0f,1.0f };





    indexVector[0] = 0;
    indexVector[1] = 1;
    indexVector[2] = 2;
    
    indexVector[3] = 0;
    indexVector[4] = 2;
    indexVector[5] = 3;

    
    squareMesh->SetVertexVector(std::move(vertexVector));
    squareMesh->SetIndexVector(std::move(indexVector));

    squareMesh->SetIndexNum(6);
    squareMesh->SetVertexNum(4);


    std::vector<SubMesh> subMeshVector(1);

    subMeshVector[0].mID = 0;
    subMeshVector[0].mIndexRange.first = 0;
    subMeshVector[0].mIndexRange.second = 6;
    subMeshVector[0].mVertexNum = 4;
    subMeshVector[0].mName = "DefaultSubMesh";
    subMeshVector[0].mVertexOffset = 0;
    subMeshVector[0].mMaterialPointer = MaterialManager::GetMaterial("Default");

    squareMesh->SetSubMeshVector(std::move(subMeshVector));
    squareMesh->SetMinMaxPoint({ -0.5f,-0.5f,-0.5f }, { 0.5f,0.5f,0.5f });



}

void Quad::EditorDirector::SwitchFrameWindow()
{

    mProjectDirector.SetOhterWindowSystemOff();
     mFrameWindow->SetProjectSelectSceneFlag(true);
     FrameWindowController::GetInstance()->SetSystemActiveState(2, false);


}

void Quad::EditorDirector::SwitchCommonEditWindow()
{
    mProjectDirector.SetOhterWindowSystemOn();
    mFrameWindow->SetProjectSelectSceneFlag(false);
    FrameWindowController::GetInstance()->SetSystemActiveState(2, true);
}



void Quad::EditorDirector::SetPlayModeState(bool state)
{
    // auto instance = GetInstance();
    mPlayModeState = state;
    mRenderWindowTest->SetPlayMode(state);


}

bool Quad::EditorDirector::GetPlayModeState()
{
    auto instance = GetInstance();
    return instance->mPlayModeState;
}

const std::string& Quad::EditorDirector::GetEditorPathA() const
{

    return mEditorPathA;
    // TODO: 여기에 return 문을 삽입합니다.
}

const std::wstring& Quad::EditorDirector::GetEditorPathW() const
{
    return mEditorPathW;
    // TODO: 여기에 return 문을 삽입합니다.
}


void Quad::EditorDirector::AddEffect(RenderSystem* renderSystem, Effect* effect)
{
    renderSystem->AddEffect(*effect, ESystemType::eMainSystem);
    renderSystem->AddEffect(*effect, ESystemType::eDockingSystem);
    //renderSystem->AddEffect(*effect, ESystemType::eUiSystem);
    renderSystem->AddEffect(*effect, ESystemType::eWindowLayoutSystem);

}








int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
#ifdef _DEBUG
    // 메모리 누수 체크 활성화
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
    int a = 2;

    Quad::Application app;
     
    Quad::EditorDirector editorDirector;

    Quad::AppInitData appInitData;
    appInitData.hInstance = hInstance;
    appInitData.nShowCmd = nCmdShow;
    appInitData.programDirector = &editorDirector;
    appInitData.collisionWorldFactoryImpl = new Quad::EditorCollisionWorldFactoryImpl;
    appInitData.spacePartitoingStructureFactoryImpl= new Quad::EditorSpacePartitioningStructrureFactoryImpl;

    if (!app.Initialize(appInitData))
        return 0;

    return app.Run();
}
