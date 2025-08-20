#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "EditorDirector/EditorDirector.h"
#include <Core/Application.h>
#include <Core/ProjectConfig.h>
#include <Utility/Utility.h>

#include "EditorDirector/EditorProjectManager.h"
#include "EditorDirector/EditorShaderImporter.h"
#include "EditorDirector/EditorTextureImporter.h"
#include "EditorDirector/SerializedAssetContainer.h"
#include "EditorDirector/SuperAssetBrowerController.h"
#include "EditorDirector/SuperFrameController.h"
#include <BinaryReaderWriter/BinaryReader.h>
#include <Core/GpuBufferContextSystem.h>
#include <Core/UIRenderItemBuilder.h>
#include <CoreAsset/AssetCommon.h>
#include <CoreAsset/AssetLoader.h>
#include <CoreAsset/AssetMetaDataManager.h>
#include <CoreAsset/Texture.h>
#include <CoreAsset/TextureManager.h>
#include <CoreAsset/TextureStorer.h>
#include <EditorDirector/EditorConfig.h>
#include <ImportModule/TextureImporter.h>
#include <InputSystem/InputSystem.h>
#include <Logger/Logger.h>
#include <LogicalFileSystem/LogicalFile.h>
#include <LogicalFileSystem/LogicalFileSystem.h>
#include <LogicalFileSystem/LogicalFolder.h>
#include <PhysicalFileSystem/PhysicalFileSystem.h>
#include <RenderSystem/IMaterialManager.h>
#include <SystemInitializer/ISystemInitializer.h>
#include <UISystem/UIManager.h>
#include <Window/BaseWindow.h>
#include <memory>
#pragma comment(lib, "ImportModule.lib")

#define D3DX

#ifdef D3DX

#include <SystemInitializer/D3DSystemInitializer.h>

#endif

#include <fstream>

#undef EngineMode

std::string testProjectPath = "C:/Users/dongd/gitproject/GameEngine";

Quad::EditorDirector *Quad::EditorDirector::GetInstance()
{
    static EditorDirector instance;
    return &instance;
}

Quad::EditorDirector::EditorDirector()
/* :mFrameWindowSceneSwitch(0), mWindowSwitchRenderFlag(false)*/
{

    wchar_t path[MAX_PATH];
    GetCurrentDirectoryW(MAX_PATH, path);

    //  mEditorPathW = path;
    //  mEditorPathA = Utility::ConvertToString(mEditorPathW, true);
}
Quad::EditorDirector::~EditorDirector() {}
// #define EngineMode
void Quad::EditorDirector::Initialize()
{

    auto app = Application::GetInstance();

    app->GetHinstance();

    // import module test

    mProjectInitializer = EditorProjectManager::GetInstance();
    mProjectInitializer->Initialize();

    {

        // 에셋 임포트, 저장 테스트 ,

        QuadLF::LogicalFileSystem *logicalFileSystem = QuadLF::LogicalFileSystem::GetInstance();
        EditorTextureImporter *textureImporter = EditorTextureImporter::GetInstance();

        bool ret = true;
        // 논리적 폴더 테스트
        bool logicalFolderCreationTestFlag = false;
        if (logicalFolderCreationTestFlag)
        {

            // 논리적 폴더생성 ,저장 테스트

            logicalFileSystem->CreateFolder("MYTEST", logicalFileSystem->GetRootFolder(), true);
            QuadRW::BinaryWriter writer;
            ret =
                logicalFileSystem->SaveLogicalDirectoryStructureAsBinaryWriter(writer, testProjectPath, "testLDSFile");
        }

        // 2025 - 07 - 14
        // 텍스처 임포트, 매니저, 리소스매니저 테스트 :Good

        bool testImport = false;
        if (testImport)
        {

            // 텍스처 임포트 테스트
            std::ifstream fin("C:\\Users\\dongd\\gitproject\\GameEngine\\TestAssetPathFile.txt");
            std::string assetFilePath;
            fin >> assetFilePath;

            // 현재 논리적폴더에서 해당 파일과 동일한 이름을 가진 논리적파일이존재하면실패하도록한다.
            // Check Logical File Name

            // 없으면 임포트수행
            CoreAsset::Texture *texture = textureImporter->Import(assetFilePath, "Asset");

            // 임포트를수행한후에는 논리적파일만 생성한다.

            // 후에 저장버튼을 누르면
            // 논리적 파일(물리적파일)을 생성
            // 따라서 저장하지않고 에디터를 종료하면 임포트한 에셋들은 증발한다.

            QuadLF::LogicalFileAssetInfo texturelogicalFileInfo;
            texturelogicalFileInfo.mAssetID = texture->GetID();
            texturelogicalFileInfo.mAssetType = CoreAsset::EAssetType::eTexture;
            texturelogicalFileInfo.mName = texture->GetName();

            QuadLF::LogicalFile *textureLogicalFile = logicalFileSystem->MakeFile(
                texturelogicalFileInfo, texture->GetName(), logicalFileSystem->GetRootFolder(), true);

            // 텍스처 저장 테스트
            CoreAsset::TextureStorer *textureStorer = CoreAsset::TextureStorer::GetInstance();
            CoreAsset::AssetMetaDataManager *assetMetaManager = CoreAsset::AssetMetaDataManager::GetInstance();

            CoreAsset::TextureMetaData *textureMetaData =
                static_cast<CoreAsset::TextureMetaData *>(assetMetaManager->GetMetaData(texture->GetID()));
            std::string physicalFilePath = logicalFileSystem->GetPhysicalFullPath(textureLogicalFile);
            ret = textureStorer->Store(texture, textureMetaData, physicalFilePath);

            std::string rawTextureName = CoreAsset::GetAssetRawFileName(textureLogicalFile->GetFullPath());
            std::string rawTexturePhysicalPath = testProjectPath + "/" + rawTextureName;
            ret = textureStorer->StoreTextureRaw(rawTexturePhysicalPath, *texture->GetRawData());
        }
    }

    mSuperFrameController = SuperFrameController::GetInstance();
    mSuperFrameController->Initialize();

    mSuperControllerVector.push_back(mSuperFrameController);

    /* mSuperAssetBrowerController = SuperAssetBrowerController::GetInstance();
     mSuperAssetBrowerController->Initialize();*/

    // editorModeDirector가 에디터가 가지는 기본 asset들을 로드할것이다.

    // std::vector<Asset*> editorDefaultAssetVector=
    // mEditorModeDirector.LoadAsset("C:\\Users\\dongd\\gitproject\\GameEngine\\SecenGraphQuadTree\\Asset");

    // SpacePartitioningStructureFactory<UiCollider>::GetInstance();

    // std::vector<Asset*> editorAsset = mEditorModeDirector.LoadAsset(".\\Asset");

    //  InitFileUiWindow();
    // InitGamePlayWindow();   //game play window 라고하는데 동시에 월드 에디터이기도한거지
    //   InitAttributeWindow();
    // InitFrameWindow();
    //  InitDragAndDropWindow();
    //  InitPopupWindow();

    // if (!mGraphicCommandObject->GetCloseState())
    //{
    //     mGraphicCommandObject->ExecuteCommandList();
    //     mGraphicCommandObject->FlushCommandQueue();
    // }

    //
    //    std::vector<DockingWindowController*>normalWindowControllerVector = {
    //    mGameWindowPlayController,mFileUiWindowController,mAttributeWindowController };
    //
    //
    //    mEditorModeDirector.SetGamePlayWindowChromeSystem(mGamePlayWindowChromeSystem);
    //    mEditorModeDirector.InitGameWindowLayoutSystem();
    ////    EditorModeDirector::SendAssetToFileSystem();
    //
    //   // AddVisibleEditorAssetToFileWindow(".\\EditorConfig\\EditorAssetVisibleConfig.json");

    //    //여기까지는 에디터를 위한내용
    // mProjectDirector.Initialize(mFrameWindow, std::move(normalWindowControllerVector), mPopupWindowController,
    // mFrameWindowController, mFrameWindowUiSystem);

    // FrameWindowMenuDirector::GetInstance();
}

void Quad::EditorDirector::PreUpdate(float deltaTime)
{

    InputSystem *inputSystem = InputSystem::GetInstance();

    Render::WindowHandle currActiveWindowHandle = inputSystem->GetCurrentWindowHandle();

    for (auto element : mSuperControllerVector)
    {
        SuperController *controller = element;
        if (controller->GetWindow()->GetWindowHandle() == currActiveWindowHandle)
        {
            MouseContext mouseContext = inputSystem->GetMouseContext();
            controller->UpdateMouseInput(mouseContext);
            // 키보드 update

            break;
        }
    }

    mSuperFrameController->PreUpdate();
    // mSuperAssetBrowerController->PreUpdate();

    // int modeSwitchFlag = GetSwitchWindowSceneModeFlag();
    // if (modeSwitchFlag == 1)
    // {
    //     //mFrameWindow->SetProjectSelectSceneFlag(true);
    //     SwitchFrameWindow();
    //    // SwitchFrameWindow();
    //   //  SetSwitchWindowSceneModeFlag(0);//초기화
    //     mWindowSwitchRenderFlag = true;
    // }
    // else if (modeSwitchFlag == 2)
    // {
    ////     mFrameWindow->SetProjectSelectSceneFlag(false);
    //     SwitchCommonEditWindow();
    //   //  SetSwitchWindowSceneModeFlag(0);//초기화
    //     mWindowSwitchRenderFlag = true;
    // }
}

void Quad::EditorDirector::Update(float deltaTime)
{

    // 지금이렇게 순서대로하는데 멀티스레드 고려를해봐야한다.
    mSuperFrameController->Update(deltaTime);
    // mSuperAssetBrowerController->Update(deltaTime);

    // #ifndef EngineMode
    //
    //     //int modeSwitchFlag = GetSwitchWindowSceneModeFlag();
    //     //if (modeSwitchFlag == 1)
    //     //{
    //     //    SwitchFrameWindow();
    //     //    SetSwitchWindowSceneModeFlag(0);//초기화
    //     //}
    //     //else if (modeSwitchFlag == 2)
    //     //{
    //     //    SwitchCommonEditWindow();
    //     //    SetSwitchWindowSceneModeFlag(0);//초기화
    //     //}
    //     //0은 상태유지
    //
    //     bool popupWindowActiveFlag = mPopupWindow->GetVisibilityState();
    //     if (mFrameWindow->GetProjectSelectSceneFlag())
    //     {
    //         if (popupWindowActiveFlag)
    //             mPopupWindow->Update(deltaTime);
    //
    //
    //         mFrameWindow->Update(deltaTime);
    //
    //         if (popupWindowActiveFlag)
    //             mPopupWindow->UploadObjectToRenderSystem();
    //
    //
    //         mFrameWindow->UploadObjectToRenderSystem();     //du여기문제있다
    //
    //         if (popupWindowActiveFlag)
    //             mPopupWindow->EndUpdate(deltaTime);
    //
    //
    //
    //         mFrameWindow->EndUpdate(deltaTime);
    //
    //
    //     }
    //     else
    //     {
    //
    //         //if(//활성화되어있다면)
    //         if (popupWindowActiveFlag)
    //             mPopupWindow->Update(deltaTime);
    //
    //
    //
    //
    //
    //
    //         if (!GetPlayModeState())
    //         {
    //             mFrameWindow->Update(deltaTime);
    //             mFileUiWindow->Update(deltaTime);
    //
    //
    //             mAttributeWindow->Update(deltaTime);
    //
    //         }
    //
    //
    //         mRenderWindowTest->Update(deltaTime);
    //
    //
    //
    //
    //
    //         if (DragAndDropWindowController::GetWindowRunningState())
    //         {
    //             mDragAndDropWindow->Update(deltaTime);
    //         }
    //
    //
    //
    //         //if(//활성화되어있다면)
    //         if (popupWindowActiveFlag)
    //             mPopupWindow->UploadObjectToRenderSystem();
    //
    //
    //         //render system으 object 전달(update내부에서 각각전달하면 다른 외부윈도우,시스템에 영향을 받아서
    //         //더이상 렌더링될수없는데도 이미 렌더시스템에 전달해버려서 렌더시스템이 이미 삭제되거나 렌더되면안되는
    //         오브젝트들에대해
    //         // 렌더링을 시도할 수  있는 문제가있다)
    //
    //         //sendObjectToRenderSystem()
    //         if (!GetPlayModeState())
    //         {
    //             mFrameWindow->UploadObjectToRenderSystem();
    //             mFileUiWindow->UploadObjectToRenderSystem();
    //             mAttributeWindow->UploadObjectToRenderSystem();
    //
    //         }
    //
    //
    //         mRenderWindowTest->UploadObjectToRenderSystem();
    //
    //
    //
    //
    //
    //         if (DragAndDropWindowController::GetWindowRunningState())
    //         {
    //             mDragAndDropWindow->UploadObjectToRenderSystem();
    //         }
    //
    //         //if(//활성화되어있다면)
    //         if (popupWindowActiveFlag)
    //             mPopupWindow->EndUpdate(deltaTime);
    //
    //
    //         //다음프레임을 위한 초기화 등등
    //         if (!GetPlayModeState())
    //         {
    //             mFrameWindow->EndUpdate(deltaTime);
    //             mFileUiWindow->EndUpdate(deltaTime);
    //
    //             mAttributeWindow->EndUpdate(deltaTime);
    //
    //         }
    //
    //         mRenderWindowTest->EndUpdate(deltaTime);
    //
    //
    //
    //
    //         if (DragAndDropWindowController::GetWindowRunningState())
    //         {
    //             mDragAndDropWindow->EndUpdate(deltaTime);
    //         }
    //
    //     }
    //
    // #endif
}

void Quad::EditorDirector::EndUpdate(float deltaTime)
{

    // 마지막에는 각 가지고있는 채널들에대해서 렌더아이템구축후 렌더시스템으로 전달
    //  mSuperFrameController
    // mSuperAssetBrowerController

    mSuperFrameController->EndUpdate();
    // mSuperAssetBrowerController->EndUpdate();
}

void Quad::EditorDirector::Draw()
{

    // 렌더시스템이 렌더 혹은

    // 각 컨트롤러들이 draw  (이미 렌더아이템들은 다 제출된상황)
    mSuperFrameController->Draw();
    // mSuperAssetBrowerController->Draw();

    UIRenderItemBuilder::GetInstance()->EndFrame();

    // #ifndef EngineMode
    //     bool popupWindowActiveFlag = mPopupWindow->GetVisibilityState();
    //
    //     if (mFrameWindow->GetProjectSelectSceneFlag())
    //     {
    //         mFrameWindow->Draw();
    //         if (popupWindowActiveFlag)
    //             mPopupWindow->Draw();
    //     }
    //     else
    //     {
    //
    //
    //         if (!GetPlayModeState())
    //         {
    //             mAttributeWindow->Draw();
    //             mFileUiWindow->Draw();
    //
    //             mFrameWindow->Draw();
    //
    //             if (popupWindowActiveFlag)
    //                 mPopupWindow->Draw();
    //
    //             if (DragAndDropWindowController::GetWindowRunningState())
    //             {
    //                 mDragAndDropWindow->Draw();
    //             }
    //         }
    //         mRenderWindowTest->Draw();
    //
    //
    //
    //     }
    //
    //
    //
    //     if (mWindowSwitchRenderFlag)
    //     {
    //         int modeSwitchFlag = GetSwitchWindowSceneModeFlag();
    //
    //         bool aa =mFrameWindow->GetProjectSelectSceneFlag();
    //         if (modeSwitchFlag == 1)
    //         {
    //             //mFrameWindow->SetProjectSelectSceneFlag(true);
    //             //SwitchFrameWindow();
    //             mProjectDirector.SetOtherWindowVisibleOnOff(false);
    //             SetSwitchWindowSceneModeFlag(0);//초기화
    //         }
    //         else if (modeSwitchFlag == 2)
    //         {
    //             //mFrameWindow->SetProjectSelectSceneFlag(false);
    //            // SwitchCommonEditWindow();
    //             mProjectDirector.SetOtherWindowVisibleOnOff(true);
    //             SetSwitchWindowSceneModeFlag(0);//초기화
    //         }
    //
    //         mWindowSwitchRenderFlag = false;
    //     }
    // #endif
}

void Quad::EditorDirector::SetSwitchWindowSceneModeFlag(int flag)
{
    auto instance = GetInstance();
    //  instance->mFrameWindowSceneSwitch = flag;
}

const int Quad::EditorDirector::GetSwitchWindowSceneModeFlag() const
{
    return 0; // mFrameWindowSceneSwitch;
}

// void Quad::EditorDirector::InitSystem()
//{
//
//     QuadLog::Logger* log = QuadLog::Logger::GetInstance();
//     bool logRet = log->SetLoggerFile("EditorLogFile.txt");
//
//
//     mProjectInitializer = EditorProjectManager::GetInstance();
//
//     mProjectInitializer->Initialize();
//
//
//
//     mTextureImporter = std::make_unique<EditorTextureImporter>(Import::TextureImporter::GetInstance(),
//     CoreAsset::TextureManager::GetInstance(), CoreAsset::AssetMetaDataManager::GetInstance());
//
//
//     QuadPF::PhysicalFileSystem *  physicalFileSystem = QuadPF::PhysicalFileSystem::GetInstance();
//
//     QuadLF::LogicalFileSystem * logicalFileSystem = QuadLF::LogicalFileSystem::GetInstance();
//
//
//     CoreAsset::AssetLoader * assetLoader = CoreAsset::AssetLoader::GetInstance();
//
//
//
//
// }

// void Quad::EditorDirector::InitGamePlayWindow()
//{
//     mRenderWindowTest = new GameRenderWindow(mHinstance);
//
//
//     UINT gameWindowClientWidth = mRenderWindowTest->GetClientWidth();
//     UINT gameWindowClientHeight = mRenderWindowTest->GetClientHeight();
//     HWND gameWindowHandle = mRenderWindowTest->GetWindowHandle();
//
//
//     //render system
//     RenderSystem* renderSystem = new RenderSystem;
//     renderSystem->Initialize(mDevice, mFactory, mGraphicCommandObject,
//         gameWindowHandle, gameWindowClientWidth, gameWindowClientHeight, mDescriptorHeapManagerMaster, GAMEWINDOW);
//
//
//     AddEffect(renderSystem, EffectManager::GetEffect("Default.effect"),ESystemType::eMainSystem);
//
//     AddEffect(renderSystem, EffectManager::GetEffect("DefaultUi.effect"),ESystemType::eMainSystem);
//     AddEffect(renderSystem, EffectManager::GetEffect("DefaultUi.effect"),ESystemType::eWindowLayoutSystem);
//     AddEffect(renderSystem, EffectManager::GetEffect("DefaultUi.effect"),ESystemType::eDockingSystem);
//
//     AddEffect(renderSystem, EffectManager::GetEffect("WindowLayout.effect"),ESystemType::eWindowLayoutSystem);
//
//     AddEffect(renderSystem, EffectManager::GetEffect("TextBox.effect"),ESystemType::eMainSystem);
//     AddEffect(renderSystem, EffectManager::GetEffect("TextBox.effect"),ESystemType::eWindowLayoutSystem);
//    // AddEffect(renderSystem, EffectManager::GetEffect("TextBox.effect"),ESystemType::eDockingSystem);
//
//     AddEffect(renderSystem, EffectManager::GetEffect("TextCharacter.effect"),ESystemType::eMainSystem);
//     AddEffect(renderSystem, EffectManager::GetEffect("TextCharacter.effect"),ESystemType::eWindowLayoutSystem);
//     //AddEffect(renderSystem, EffectManager::GetEffect("TextCharacter.effect"), ESystemType::eDockingSystem);
//
//
//     AddEffect(renderSystem, EffectManager::GetEffect("Line.effect"),ESystemType::eMainSystem);
//
//
//     AddEffect(renderSystem, EffectManager::GetEffect("DefaultCollider.effect"),ESystemType::eMainSystem);
//
//     AddEffect(renderSystem, EffectManager::GetEffect("GizmoLine.effect"),ESystemType::eMainSystem);
//
//     AddEffect(renderSystem, EffectManager::GetEffect("Gizmo.effect"), ESystemType::eMainSystem);
//     AddEffect(renderSystem, EffectManager::GetEffect("GizmoRotation.effect"), ESystemType::eMainSystem);
//
//   //  AddEffect(renderSystem, EffectManager::GetEffect("Docking.effect"), ESystemType::eMainSystem);
//     AddEffect(renderSystem, EffectManager::GetEffect("Docking.effect"), ESystemType::eWindowLayoutSystem);
//     AddEffect(renderSystem, EffectManager::GetEffect("Docking.effect"), ESystemType::eDockingSystem);
//
//     AddEffect(renderSystem, EffectManager::GetEffect("TestEffect"), ESystemType::eMainSystem);
//    // AddEffect(renderSystem, EffectManager::GetEffect("TestEffect"), ESystemType::eWindowLayoutSystem);
//   //  AddEffect(renderSystem, EffectManager::GetEffect("TestEffect"), ESystemType::eDockingSystem);
//
//
//
//     renderSystem->SetColliderWorldRenderState(true);
//
//
//
//
//
//
//
//     //System을 초기화하면서 맵들, 그 맵에있는엔티티들,등등을 로드하고 초기화한다.
//         //Controller
//     GamePlayWindowController* wcontroller = new GamePlayWindowController(renderSystem);
//     mGameWindowPlayController = wcontroller;
//     wcontroller->SetName("gameWindowPlayController");
//     Controller::AddController("gameWindowPlayController", wcontroller);
//
//     GamePlaySystem* gamePlaySystem = new GamePlaySystem;
//    // GamePlayUiSystem* gamePlayUiSystem = new GamePlayUiSystem;
//     WindowChromeSystem* windowLayoutSystem = new WindowChromeSystem(ESystemID::eGamePlayLayoutSystem);
//     mGamePlayWindowChromeSystem = windowLayoutSystem;
//     ChildWindowDockingSystem* windowDockingSystem = new ChildWindowDockingSystem(ESystemID::eGamePlayDockingSystem);
//
//
//     wcontroller->Initialize(mDevice, mGraphicCommandObject->GetGraphicsCommandList(), MeshManager::GetInstance(),
//         MaterialManager::GetInstance(), TextureManager::GetInstance(), MapManager::GetInstance(), mRenderWindowTest,
//         gamePlaySystem, windowLayoutSystem, windowDockingSystem, mDescriptorHeapManagerMaster, false);
//
//     gamePlaySystem->SetController(wcontroller);
//   //  gamePlayUiSystem->SetController(wcontroller);
//     windowLayoutSystem->SetController(wcontroller);
//     windowDockingSystem->SetController(wcontroller);
//
//     wcontroller->SetWindowActiveFlag(false);
//
//
//
//
//     InitGameProject(mRenderWindowTest, wcontroller, gamePlaySystem, windowLayoutSystem,
//         windowDockingSystem);
//
//
//
//     //유일하게 gameWindow만 false로 시작한다.
//     //다른 window들은 항상 play mode이기때문에(true) //그리고 이미 ui,main system은 false로 설정되었다
//     (내부초기화때문에)
//     //일단 컨트롤러에서 직접 상태를 가져오는 일이 없는거같지만 그래도 controller와 동기화해주자
//     wcontroller->SetPlayMode(false);
//
//
//     wcontroller->SetTitleBarSize(true, 3000, 40);
//     //gamePlayUiSystem->SetDefaultController("gameWindowPlayController");
//
//     mRenderWindowTest->Initialize(wcontroller);
//
//
//     auto eventDispatcher = EventDispatcher::GetInstance();
//     eventDispatcher->RegisterSystem(gamePlaySystem);
//     //eventDispatcher->RegisterSystem(gamePlayUiSystem);
//     eventDispatcher->RegisterSystem(windowLayoutSystem);
//     eventDispatcher->RegisterSystem(windowDockingSystem);
//
//
//
//
//
//     KeyBoard::SetHwnd(mRenderWindowTest->GetWindowHandle());
// }
//
// void Quad::EditorDirector::InitFileUiWindow()
//{
//     mFileUiWindow = new FileUiWindow(mHinstance);
//
//     //window controller;
//     //  Controller* controller = new MapController;// new Controller;
//
//     //rendersystem
//
//     //3d,ui,chrome system
//
//     UINT clientWidth = mFileUiWindow->GetClientWidth();
//     UINT clientHeight = mFileUiWindow->GetClientHeight();
//
//     //renderSystem;
//     RenderSystem* renderSystem = new RenderSystem;
//     renderSystem->Initialize(mDevice,
//         mFactory, mGraphicCommandObject, mFileUiWindow->GetWindowHandle(),
//         clientWidth, clientHeight, mDescriptorHeapManagerMaster, FILEUIWINDOW);
//
//
//     FileUiWindowContoller* wController = new FileUiWindowContoller(renderSystem);// new Controller;
//     mFileUiWindowController = wController;
//
//     AddEffect(renderSystem, EffectManager::GetEffect("Default.effect"),ESystemType::eMainSystem);
//     AddEffect(renderSystem, EffectManager::GetEffect("Default.effect"),ESystemType::eWindowLayoutSystem);
//     AddEffect(renderSystem, EffectManager::GetEffect("Default.effect"),ESystemType::eDockingSystem);
//
//     AddEffect(renderSystem, EffectManager::GetEffect("DefaultUi.effect"), ESystemType::eMainSystem);
//     AddEffect(renderSystem, EffectManager::GetEffect("DefaultUi.effect"), ESystemType::eWindowLayoutSystem);
//     AddEffect(renderSystem, EffectManager::GetEffect("DefaultUi.effect"), ESystemType::eDockingSystem);
//
//
//     AddEffect(renderSystem, EffectManager::GetEffect("WindowLayout.effect"), ESystemType::eWindowLayoutSystem);
//
//     AddEffect(renderSystem, EffectManager::GetEffect("TextBox.effect"),ESystemType::eMainSystem);
//     AddEffect(renderSystem, EffectManager::GetEffect("TextCharacter.effect"), ESystemType::eMainSystem);
//     AddEffect(renderSystem, EffectManager::GetEffect("TextCharacter.effect"), ESystemType::eWindowLayoutSystem);
//
//     AddEffect(renderSystem, EffectManager::GetEffect("DefaultCollider.effect"), ESystemType::eMainSystem);
//   //  AddEffect(renderSystem, EffectManager::GetEffect("Docking.effect"), ESystemType::eMainSystem);
//   //  AddEffect(renderSystem, EffectManager::GetEffect("Docking.effect"), ESystemType::eWindowLayoutSystem);
//     AddEffect(renderSystem, EffectManager::GetEffect("Docking.effect"), ESystemType::eDockingSystem);
//
//     //  SetCurrentDirectory(beforeDirectory);
//
//
//
//
//
//
//
//     wController->SetName("FileUiWindowController");
//     Controller::AddController("FileUiWindowController", wController);
//
//     //ui System;
//    // FileUiSystem* mainSystem = new FileUiSystem;
//     FileUiUiSystem* mainSystem = new FileUiUiSystem;
//     WindowChromeSystem* windowLayoutSystem = new WindowChromeSystem(ESystemID::eFileUiLayoutSystem);
//     ChildWindowDockingSystem* windowDockingSystem = new ChildWindowDockingSystem(ESystemID::eFileUiDockingSystem);
//     //uiSystem->SetController(wController);
//     mainSystem->SetController(wController);
//     windowLayoutSystem->SetController(wController);
//     windowDockingSystem->SetController(wController);
//     windowDockingSystem->SetController(wController);
//
//     wController->Initialize(mDevice, mGraphicCommandObject->GetGraphicsCommandList(), MeshManager::GetInstance(),
//     MaterialManager::GetInstance(), TextureManager::GetInstance(),
//         MapManager::GetInstance(), mFileUiWindow,mainSystem, windowLayoutSystem,
//         windowDockingSystem, mDescriptorHeapManagerMaster);
//
//     mFileUiWindow->Initialize(wController);
//
//     InitFileUiUiSystem(mFileUiWindow, mainSystem);
//    // InitFileUiSystem2(mFileUiWindow , mainSystem);
//     InitFileUiWindowLayoutSystem(mFileUiWindow, windowLayoutSystem);
//     InitChildWindowDockingSystem(mFileUiWindow, windowDockingSystem);
//
//
//     wController->SetTitleBarSize(true, 3000, 40);
//     wController->SetWindowActiveFlag(false);
//
//     auto eventDispatcher = EventDispatcher::GetInstance();
//     eventDispatcher->RegisterSystem(mainSystem);
//   //  eventDispatcher->RegisterSystem(uiSystem);
//     eventDispatcher->RegisterSystem(windowLayoutSystem);
//     eventDispatcher->RegisterSystem(windowDockingSystem);
//
//
//     //윈도우생성
//     //윈도우컨트롤러 생성
//     //시스템들을 생성
//
//     //윈도우 컨트롤러 초기화 ( 시스템 패싱)
//     //컨트롤러 초기화에서 시스템 초기화수행
//     //맵은 없는상태
//
//
//
//
//     //그이후 맵추가
//     //(맵추가할때 카메라등등처리)
//
//
//
//     KeyBoard::SetHwnd(mFileUiWindow->GetWindowHandle());
//
// }
//
// void Quad::EditorDirector::InitAttributeWindow()
//{
//
//     mAttributeWindow = new AttributeWindow(mHinstance);
//     //  Controller* controller = new MapController;// new Controller;
//
//
//     UINT clientWidth = mAttributeWindow->GetClientWidth();
//     UINT clientHeight = mAttributeWindow->GetClientHeight();
//
//     //renderSystem;
//     RenderSystem* renderSystem = new RenderSystem;
//     renderSystem->Initialize(mDevice,
//         mFactory, mGraphicCommandObject, mAttributeWindow->GetWindowHandle(),
//         clientWidth, clientHeight, mDescriptorHeapManagerMaster, ATTRIBUTEWINDOW);
//
//     DockingWindowController* wController = new DockingWindowController(renderSystem);// new Controller;
//     mAttributeWindowController = wController;
//
//     AddEffect(renderSystem, EffectManager::GetEffect("Default.effect"),ESystemType::eMainSystem);
//
//
//
//     AddEffect(renderSystem, EffectManager::GetEffect("DefaultUi.effect"), ESystemType::eMainSystem);
//     AddEffect(renderSystem, EffectManager::GetEffect("DefaultUi.effect"), ESystemType::eDockingSystem);
//     AddEffect(renderSystem, EffectManager::GetEffect("DefaultUi.effect"), ESystemType::eWindowLayoutSystem);
//
//     AddEffect(renderSystem, EffectManager::GetEffect("WindowLayout.effect"), ESystemType::eWindowLayoutSystem);
//
//     AddEffect(renderSystem, EffectManager::GetEffect("TextBox.effect"),ESystemType::eMainSystem);
//
//     AddEffect(renderSystem, EffectManager::GetEffect("TextCharacter.effect"), ESystemType::eMainSystem);
//
//     AddEffect(renderSystem, EffectManager::GetEffect("DefaultCollider.effect"), ESystemType::eMainSystem);
//
//     AddEffect(renderSystem, EffectManager::GetEffect("Line.effect"), ESystemType::eMainSystem);
//
//     AddEffect(renderSystem, EffectManager::GetEffect("ScrollListPanelUi.effect"), ESystemType::eMainSystem);
//
//     AddEffect(renderSystem, EffectManager::GetEffect("ScrollListChildPanelUi.effect"), ESystemType::eMainSystem);
//
//     AddEffect(renderSystem, EffectManager::GetEffect("Docking.effect"), ESystemType::eDockingSystem);
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
//     wController->SetName("AttributeWindowController");
//     Controller::AddController("AttributeWindowController", wController);
//     //ui System;
//     AttributeUiSystem* uiSystem = new AttributeUiSystem;
//     //AttributeSystem* worldSystem = new AttributeSystem;
//     WindowChromeSystem* windowLayoutSystem = new WindowChromeSystem(ESystemID::eAttributeLayoutSystem);
//     ChildWindowDockingSystem* windowDockingSystem = new ChildWindowDockingSystem(ESystemID::eAttributeDockingSystem);
//
//
//     mAttributeWindow->Initialize(wController);
//     uiSystem->SetController(wController);
//   //  worldSystem->SetController(wController);
//     windowLayoutSystem->SetController(wController);
//     windowDockingSystem->SetController(wController);
//     windowDockingSystem->SetController(wController);
//
//
//
//     wController->Initialize(mDevice, mGraphicCommandObject->GetGraphicsCommandList(), MeshManager::GetInstance(),
//     MaterialManager::GetInstance(), TextureManager::GetInstance(),
//         MapManager::GetInstance(), mAttributeWindow, uiSystem, windowLayoutSystem, windowDockingSystem,
//         mDescriptorHeapManagerMaster);
//
//  //   InitAttributeSystem(mAttributeWindow, worldSystem);
//     InitAttributeUiSystem(mAttributeWindow, uiSystem);
//     InitAttributeWindowLayoutSystem(mAttributeWindow, windowLayoutSystem);
//     InitChildWindowDockingSystem(mAttributeWindow, windowDockingSystem);
//
//
//
//     wController->SetWindowActiveFlag(false);
//
//
//
//
//     wController->SetTitleBarSize(true, 3000, 40);
//
//     auto eventDispatcher = EventDispatcher::GetInstance();
//     //eventDispatcher->RegisterSystem(worldSystem);
//     eventDispatcher->RegisterSystem(uiSystem);
//     eventDispatcher->RegisterSystem(windowLayoutSystem);
//     eventDispatcher->RegisterSystem(windowDockingSystem);
//
//
//     KeyBoard::SetHwnd(mAttributeWindow->GetWindowHandle());
//
// }

// void Quad::EditorDirector::InitFrameWindow()
//{
//
//
//
//     mFrameWindow = new FrameWindow(mHinstance);
//
//         //  Controller* controller = new MapController;// new Controller;
//
//
//         UINT clientWidth = mFrameWindow->GetClientWidth();
//         UINT clientHeight = mFrameWindow->GetClientHeight();
//
//         //renderSystem;
//         RenderSystem* renderSystem = new RenderSystem;
//         renderSystem->Initialize(mDevice,
//             mFactory, mGraphicCommandObject, mFrameWindow->GetWindowHandle(),
//             clientWidth, clientHeight, mDescriptorHeapManagerMaster, FRAMEWINDOW);
//
//
//         FrameWindowController* wController = new FrameWindowController(renderSystem);// new Controller;
//         mFrameWindowController = wController;
//
//         AddEffect(renderSystem, EffectManager::GetEffect("Default.effect"), ESystemType::eMainSystem);
//         AddEffect(renderSystem, EffectManager::GetEffect("Default.effect"), ESystemType::eDockingSystem);
//         AddEffect(renderSystem, EffectManager::GetEffect("Default.effect"), ESystemType::eWindowLayoutSystem);
//
//
//         AddEffect(renderSystem, EffectManager::GetEffect("DefaultUi.effect"), ESystemType::eMainSystem);
//         AddEffect(renderSystem, EffectManager::GetEffect("DefaultUi.effect"), ESystemType::eDockingSystem);
//         AddEffect(renderSystem, EffectManager::GetEffect("DefaultUi.effect"), ESystemType::eWindowLayoutSystem);
//
//
//         AddEffect(renderSystem, EffectManager::GetEffect("WindowLayout.effect"), ESystemType::eWindowLayoutSystem);
//
//         AddEffect(renderSystem, EffectManager::GetEffect("TextBox.effect"), ESystemType::eMainSystem);
//         AddEffect(renderSystem, EffectManager::GetEffect("TextCharacter.effect"), ESystemType::eMainSystem);
//         AddEffect(renderSystem, EffectManager::GetEffect("TextCharacter.effect"), ESystemType::eWindowLayoutSystem);
//         AddEffect(renderSystem, EffectManager::GetEffect("DefaultCollider.effect"), ESystemType::eMainSystem);
//         AddEffect(renderSystem, EffectManager::GetEffect("ScrollListPanelUi.effect"), ESystemType::eMainSystem);
//         AddEffect(renderSystem, EffectManager::GetEffect("ScrollListChildPanelUi.effect"), ESystemType::eMainSystem);
//         AddEffect(renderSystem, EffectManager::GetEffect("Line.effect"), ESystemType::eMainSystem);
//         AddEffect(renderSystem, EffectManager::GetEffect("Spline.effect"), ESystemType::eMainSystem);
//         AddEffect(renderSystem, EffectManager::GetEffect("Thick2DSpline.effect"), ESystemType::eMainSystem);
//        // EffectManager::GetEffect("Default.effect");
//
//
//
//         wController->SetName("FrameWindowController");
//
//         Controller::AddController("FrameWindowController", wController);
//         //ui System;
//         FrameWindowUiSystem* uiSystem = new FrameWindowUiSystem;
//         mFrameWindowUiSystem = uiSystem;    //ProjectDirector초기화를위해
//         //FrameWindowSystem* worldSystem = new FrameWindowSystem;
//
//         WindowChromeSystem* windowLayoutSystem = nullptr; //new WindowChromeSystem(ESystemID::eFrameLayoutSystem);
//         FrameWindowDockingSystem* windowDockingSystem = nullptr; // new FrameWindowDockingSystem;
//
//
//
//         wController->Initialize(mDevice, mGraphicCommandObject->GetGraphicsCommandList(), MeshManager::GetInstance(),
//         MaterialManager::GetInstance(), TextureManager::GetInstance(),
//             MapManager::GetInstance(), mFrameWindow, uiSystem, windowLayoutSystem,
//             windowDockingSystem, mDescriptorHeapManagerMaster);
//
//         wController->SetWindowActiveFlag(true);
//
//         uiSystem->SetController(wController);
//         //worldSystem->SetController(wController);
//      //   windowLayoutSystem->SetController(wController);
//      //   windowDockingSystem->SetController(wController);
//
//         //InitFrameWindowSystem(mFrameWindow, worldSystem);
//         InitFrameWindowUiSystem(mFrameWindow, uiSystem);
//      //   InitFrameWindowLayoutSystem(mFrameWindow, windowLayoutSystem);
//      //   InitFrameWindowDockingSystem(mFrameWindow, windowDockingSystem);
//
//
//         mFrameWindow->Initialize(wController);
//
//         mFrameWindow->SetProjectSelectSceneFlag(true);
//         uiSystem->SetActiveState(true);
//       //  worldSystem->SetActiveState(true);
//        windowLayoutSystem->SetActiveState(true);
//      //   windowDockingSystem->SetActiveState(false);
//
//
//
//
//
//        // wController->SetTitleBarSize(true, 3000, 60);
//
//
//      //   wController->InitChildWindowSetting(mRenderWindowTest, mAttributeWindow,mFileUiWindow);
//         mFrameWindow->OnResize(clientWidth, clientHeight, 0);
//
//         auto eventDisaptcher = EventDispatcher::GetInstance();
//         //eventDisaptcher->RegisterSystem(worldSystem);
//         eventDisaptcher->RegisterSystem(uiSystem);
//       //  eventDisaptcher->RegisterSystem(windowLayoutSystem);
//      //  eventDisaptcher->RegisterSystem(windowDockingSystem);
//
//
//
//
//
//         mFrameWindowMenuDirector.Initialize(windowLayoutSystem);
//
//         KeyBoard::SetHwnd(mFrameWindow->GetWindowHandle());
//
//
// }

// void Quad::EditorDirector::InitDragAndDropWindow()
//{
//   //  //mDragAndDropWindow = new DragAndDropWindow(mHinstance,;
//
//
//
//   //  DragAndDropWindowController* controller = new DragAndDropWindowController(&mDragAndDropRenderSystem);
//
//
//   //
//   //  DragAndDropSystem* uiSystem = new DragAndDropSystem;
//   //  uiSystem->SetController(controller);
//
//   //  Map* map = EditorMapManager::CreateEngineSystemMap(uiSystem, "DragAndDropWindowMainMap");
//   //  uiSystem->Initialize(300, 100, map);
//   //  //map->SetName("DragAndDropWindowMainMap");
//
//   //  //
//   ////  map->Initialize(uiSystem);
//
//   // // VectorSpace* vectorSpace = new VectorSpace;
//   // // vectorSpace->Initialize(600);
//
//
//
//   //  mDragAndDropWindow = new DragAndDropWindow(mHinstance);
//   //  mDragAndDropWindow->Initialize(controller, 300, 100);
//
//   //  mDragAndDropRenderSystem.Initialize(mDevice, mFactory,
//   //      mGraphicCommandObject, mDragAndDropWindow->GetWindowHandle(), 300, 100, mDescriptorHeapManagerMaster,
//   DRAGANDDROPWINDOW);
//
//
//   //  AddEffect(&mDragAndDropRenderSystem, EffectManager::GetEffect("Default.effect"), ESystemType::eMainSystem);
//   //  AddEffect(&mDragAndDropRenderSystem, EffectManager::GetEffect("DefaultUi.effect"), ESystemType::eMainSystem);
//   //  AddEffect(&mDragAndDropRenderSystem, EffectManager::GetEffect("WindowLayout.effect"),
//   ESystemType::eWindowLayoutSystem);
//   //  AddEffect(&mDragAndDropRenderSystem, EffectManager::GetEffect("TextBox.effect"), ESystemType::eMainSystem);
//   //  AddEffect(&mDragAndDropRenderSystem, EffectManager::GetEffect("TextCharacter.effect"),
//   ESystemType::eMainSystem);
//   //  AddEffect(&mDragAndDropRenderSystem, EffectManager::GetEffect("DefaultCollider.effect"),
//   ESystemType::eMainSystem);
//
//
//
//   //  //드래그앤드랍윈도우에대한 고유의 시스템이 필요하다.
//
//
//   //  controller->Initialize(mDragAndDropWindow, uiSystem);
//
//
//
//
//
//   // // CollisionWorld* collisionWorld = new CollisionWorld(vectorSpace);
//
//   ////  map->CreateMapLayer(0, 0, nullptr, collisionWorld, { 0,0,(float)300, (float)100 ,0.0f,1.0f });
//
//   //
//   //
//   //
//   //  map->CreateMapLayer(0, 0, nullptr, { 0,0,(float)300,(float)400,0.0f,1.0f });
//   //  //default 3D - mapLayer 1
//   //  map->CreateMapLayer(0, 0, nullptr, { 0,0,(float)300,(float)400,0.0f,1.0f });
//
//
//
//   //  //ui collisionWorld
//   //  auto spacePartitioningStructureFactoryUi = SpacePartitioningStructureFactory<UiCollider>::GetInstance();
//   //  SpacePartitioningStructure<UiCollider>* spacePartitioningStructureUi =
//   spacePartitioningStructureFactoryUi->CreateSpacePartitioningStructure("VectorSpace");
//
//   //  auto collisionWorldFactory = CollisionWorldFactory::GetInstance();
//   //  UiCollisionWorld* CollisionWorldUi =
//   collisionWorldFactory->CreateUiCollisionWorld(spacePartitioningStructureUi);
//
//   //  map->SetDefaultUiCollisionWorld(CollisionWorldUi);
//
//
//
//   //  //3d collisionWorld
//   //  auto spacePartitioningStructureFactory3D = SpacePartitioningStructureFactory<Collider>::GetInstance();
//   //  SpacePartitioningStructure<Collider>* spacePartitioningStructure3D =
//   spacePartitioningStructureFactory3D->CreateSpacePartitioningStructure("VectorSpace");
//
//   //  CollisionWorld* CollisionWorld3D = collisionWorldFactory->CreateCollisionWorld(spacePartitioningStructure3D);
//
//   //  map->SetDefault3DCollisionWorld(CollisionWorld3D);
//
//
//
//   //  uiSystem->Start();
//
//
//
//
//
//   //  Map* currMap = uiSystem->GetMap();
//
//   //  //   OrthogoanlCamera* camera = static_cast<OrthogoanlCamera*>(currMap->CreateObject("OrthogoanlCamera"));
//   //  OrthogoanlCamera* camera = OrthogoanlCamera::Create(map, 0, 300, 100);
//
//   //  //  camera->Initialize(300, 100);
//   //  currMap->SetMainCamera(camera);
//
//
//
//
//   //  auto eventDispatcher = EventDispatcher::GetInstance();
//
//   //  eventDispatcher->RegisterSystem(uiSystem);
//
//
//
// }
//
// void Quad::EditorDirector::InitPopupWindow()
//{
//   //  PopupWindowUiSystem* uiSystem = new PopupWindowUiSystem;
//   //  Map* map = EditorMapManager::CreateEngineSystemMap(uiSystem, "PopupUiSystem");
//   //  // map->Initialize(uiSystem);
//
//
//
//   ////  map->CreateMapLayer(0, 0, nullptr, collisionWorld, { 0,0,(float)300, (float)400 ,0.0f,1.0f });
//
//
//   //
//
//   //  //   Camera* camera = new PopupSystemCamera(L"PopupSystemCamera");
//
//
//
//   //  PopupWindowController* controller = new PopupWindowController(&mPopupRenderSystem);
//   //  mPopupWindowController = controller;
//
//
//   //  mPopupWindow = new PopupWindow(mHinstance, 300, 400, false);
//   //  mPopupWindow->Initialize(controller);
//
//   //  uiSystem->Initialize(300, 400, map);
//   //  uiSystem->SetController(controller);
//
//   //  //default Ui - mapLayer 0
//   //  map->CreateMapLayer(0, 0, nullptr, { 0,0,(float)300,(float)400,0.0f,1.0f });
//   //  //default 3D - mapLayer 1
//   //  map->CreateMapLayer(0, 0, nullptr, { 0,0,(float)300,(float)400,0.0f,1.0f });
//
//
//
//   //  //ui collisionWorld
//   //  auto spacePartitioningStructureFactoryUi = SpacePartitioningStructureFactory<UiCollider>::GetInstance();
//   //  SpacePartitioningStructure<UiCollider>* spacePartitioningStructureUi =
//   spacePartitioningStructureFactoryUi->CreateSpacePartitioningStructure("VectorSpace");
//
//   //  auto collisionWorldFactory = CollisionWorldFactory::GetInstance();
//   //  UiCollisionWorld* CollisionWorldUi =
//   collisionWorldFactory->CreateUiCollisionWorld(spacePartitioningStructureUi);
//
//   //  map->SetDefaultUiCollisionWorld(CollisionWorldUi);
//
//
//
//   //  //3d collisionWorld
//   //  auto spacePartitioningStructureFactory3D = SpacePartitioningStructureFactory<Collider>::GetInstance();
//   //  SpacePartitioningStructure<Collider>* spacePartitioningStructure3D =
//   spacePartitioningStructureFactory3D->CreateSpacePartitioningStructure("VectorSpace");
//
//   //  // auto collisionWorldFactory = CollisionWorldFactory::GetInstance();
//   //  CollisionWorld* CollisionWorld3D = collisionWorldFactory->CreateCollisionWorld(spacePartitioningStructure3D);
//
//   //  map->SetDefault3DCollisionWorld(CollisionWorld3D);
//
//
//
//   //  uiSystem->Start();
//
//
//
//
//
//   //
//
//   //  Map* currMap = uiSystem->GetMap();
//
//
//   //  // PopupSystemCamera* camera = static_cast<PopupSystemCamera*>(currMap->CreateObject("PopupSystemCamera"));
//   //  PopupSystemCamera* camera = PopupSystemCamera::Create(currMap, 0, 300, 400);
//   //  //  camera->Initialize(300, 400);
//   //  camera->SetPosition(0, 0, 0);
//   //  currMap->SetMainCamera(camera);
//
//   //  mPopupRenderSystem.Initialize(mDevice, mFactory, mGraphicCommandObject, mPopupWindow->GetWindowHandle(), 300,
//   400, mDescriptorHeapManagerMaster, POPUPWINDOW);
//
//   //  mPopupRenderSystem.SetBackgroundColor(0.2f, 0.2f, 0.2f, 1.0f);
//   //  controller->Initialize(mPopupWindow, uiSystem);
//
//   // // AddEffect(&mPopupRenderSystem, EffectManager::GetEffect("Default.effect"));
//   //  AddEffect(&mPopupRenderSystem, EffectManager::GetEffect("DefaultUi.effect"), ESystemType::eMainSystem);
//   //  // AddEffect(&mPopupRenderSystem, EffectTable::GetEffect(L"WindowLayout.effect"));
//   //  AddEffect(&mPopupRenderSystem, EffectManager::GetEffect("TextBox.effect"), ESystemType::eMainSystem);
//   //  AddEffect(&mPopupRenderSystem, EffectManager::GetEffect("TextCharacter.effect"), ESystemType::eMainSystem);
//   //  // AddEffect(mPopupRenderSystem, EffectTable::GetEffect(L"DefaultCollider.effect"));
//
//
//
//
//   //  KeyBoard::SetHwnd(mPopupWindow->GetWindowHandle());
//
// }
//
// void Quad::EditorDirector::InitGameProject(TaskWindow* window, DockingWindowController* controller, GamePlaySystem*
// gamePlaySystem,  WindowChromeSystem* windowLayoutSystem, ChildWindowDockingSystem* windowDockingSystem)
//{
//     //InitGamePlayUiSystem(window, gamePlayUiSystem);
//     InitGamePlaySystem(window, gamePlaySystem);
//     InitGamePlayWindowLayoutSystem(window, windowLayoutSystem);
//     InitChildWindowDockingSystem(window, windowDockingSystem);
//
//     //gamePlayUiSystem->SetController(controller);
//     gamePlaySystem->SetController(controller);
//     windowLayoutSystem->SetController(controller);
//     windowDockingSystem->SetController(controller);
//
// }
//
// void Quad::EditorDirector::InitGamePlaySystem(TaskWindow* window, GamePlaySystem* system)
//{
////    system->Initialize(window->GetClientWidth(), window->GetClientHeight(), nullptr);
//
//
//
//}
//
// void Quad::EditorDirector::InitGamePlayUiSystem(TaskWindow* window, GamePlayUiSystem* system)
//{
//   // system->Initialize(window->GetClientWidth(), window->GetClientHeight(), nullptr);
//}
//
////void Quad::EditorDirector::InitAttributeSystem(TaskWindow* window, AttributeSystem* system)
////{
////
////
////    Map* map = EditorSystem::CreateMap(system, "AttrMainMap", false, false);
////    //  map->SetName("AttrMainMap");
////    //  map->Initialize(system);
////   /* map->CreateMapLayer(0, 0, nullptr, collisionWorld, { 0,0,(float)window->GetClientWidth(),
///(float)window->GetClientHeight() ,0.0f,1.0f });*/
////
////    initDefaultMapSetting(window, map);
////    // Camera* gameCamera = new Camera(L"GameCamera");
////
////
////
////    system->Initialize(window->GetClientWidth(), window->GetClientHeight(), map);
////    Map* currMap = system->GetMap();
////    // FrustumCamera* gameCamera = static_cast<FrustumCamera*>(currMap->CreateObject("FrustumCamera"));
////    FrustumCamera* gameCamera = FrustumCamera::Create(currMap, 0, DirectX::XM_PI / 2,
///(float)window->GetClientWidth() / window->GetClientHeight()); /    //  gameCamera->Initialize(DirectX::XM_PI / 2,
///(float)window->GetClientWidth() / window->GetClientHeight()); /    currMap->SetMainCamera(gameCamera); /    //
/// map->SetCameraType(ECameraType::ePerspectiveCamera); / currMap->GetMainCamera()->GetTransform().SetPositionLocal({
/// 0,0,-10.0f });
////
////}
//
// void Quad::EditorDirector::InitAttributeUiSystem(TaskWindow* window, AttributeUiSystem* system)
//{
//
//
//    Map* map = EditorMapManager::CreateEngineSystemMap(system, "AttrUiMainMap");
//    // map->SetName("AttrUiMainMap");
//    // map->Initialize(system);
//
//
//
//   // system->Initialize(window->GetClientWidth(), window->GetClientHeight(), map);
//
//    initDefaultMapSetting(window, map);
//
//
//
//    map->SetViewportAutoFlag(false, 1);
//
//
//    RenderTargetTexture* renderTargetTexture =
//    TextureManager::CreateRenderTargetTexture("AnimationEdit3DPanelTexture", 300, 300);
//    //TextureManager::AddTexture(renderTargetTexture, L"AnimationEdit3DPanelTexture");
//    renderTargetTexture->SetEngineContentItemFlag(true);
//
//    Texture* depthStencilBuffer = TextureManager::CreateDepthStencilBuffer("AnimationEdit3DPanelDepthStencilBuffer",
//    300, 300);
//    //map layer 1
//    depthStencilBuffer->SetEngineContentItemFlag(true);
//    //  vectorSpace = new VectorSpace;
//    //  vectorSpace->Initialize(100);
//   //   CollisionWorld* animationEdit3DCollisionWorld = new CollisionWorld(vectorSpace);
//
//
//    //  map->CreateMapLayer(1, 1, nullptr, animationEdit3DCollisionWorld, { 0,0,300,300,0.0f,1.0f },
//    renderTargetTexture, //depthStencilBuffer);
//
//    MapLayer& default3DMapLayer = map->GetMapLayer(1);
//    default3DMapLayer.mID = 1;
//    default3DMapLayer.mDepthPriority = 1;
//    default3DMapLayer.mViewPort = { 0,0,300,300,0.0f,1.0f };
//    default3DMapLayer.mRenderTarget = renderTargetTexture;
//    default3DMapLayer.mDepthStencilBuffer = depthStencilBuffer;
//
//    //  Camera* animationEdit3DCamera = new Camera(L"AnimationEdit3DCamera");
//
//
//    system->Start();
//
//
//
//
//    Map* currMap = system->GetMap();
//
//
//    // OrthogoanlCamera* gameCamera = static_cast<OrthogoanlCamera*>(currMap->CreateObject("OrthogoanlCamera"));
//    OrthogoanlCamera* gameCamera = OrthogoanlCamera::Create(currMap, 0, window->GetClientWidth(),
//    window->GetClientHeight());
//    // gameCamera->Initialize(window->GetClientWidth(), window->GetClientHeight());
//
//    currMap->SetMainCamera(gameCamera);
//
//
//
//    //  FrustumCamera* animationEdit3DCamera = static_cast<FrustumCamera*>(currMap->CreateObject("FrustumCamera", 1));
//    FrustumCamera* animationEdit3DCamera = FrustumCamera::Create(currMap, 0, 1000, 1000);
//    //    animationEdit3DCamera->Initialize(1000, 1000);
//    currMap->SetMainCamera(animationEdit3DCamera, 1);
//
//
//    //map->SetCameraType(ECameraType::ePerspectiveCamera);
//    currMap->GetMainCamera()->GetTransform().SetPositionLocal({ 0,0,0.0f });
//
//
//    currMap->GetMainCamera()->GetTransform().SetPositionLocal({ (float)window->GetClientWidth() / 2,-1.0f *
//    (float)window->GetClientHeight() / 2 , 0 });
//
//
//
//}
//
// void Quad::EditorDirector::InitFileUiUiSystem(TaskWindow* window, FileUiUiSystem* system)
//{
//   //// VectorSpace* vectorSpace = new VectorSpace;
//   //// vectorSpace->Initialize(100000000);
//   //// CollisionWorld* collisionWorld = new CollisionWorld(vectorSpace);
//
//
//   // Map* map = EditorMapManager::CreateEngineSystemMap(system, "FileUiUiMainMap");
//   // //  map->SetName("FileUiUiMainMap");
//   // //  map->Initialize(system);
//
//
//
//
//   // //gameCamera를 설정은했는데 맵으로 들어가진않은거지
//
//   // system->Initialize(window->GetClientWidth(), window->GetClientHeight(), map);
//   // //Map* currMap = system->GetMap();
//   // initDefaultMapSetting(window, map);
//
//   // system->Start();
//
//
//   // //  OrthogoanlCamera* gameCamera = static_cast<OrthogoanlCamera*>(currMap->CreateObject("OrthogoanlCamera"));
//   // OrthogoanlCamera* gameCamera = OrthogoanlCamera::Create(map, 0, window->GetClientWidth(),
//   window->GetClientHeight());
//   // //  gameCamera->Initialize(window->GetClientWidth(), window->GetClientHeight());
//
//   // map->SetMainCamera(gameCamera);
//   // //map->SetCameraType(ECameraType::ePerspectiveCamera);
//   // map->GetMainCamera()->GetTransform().SetPositionLocal({ 0,0,-10.0f });
//
//}
//
// void Quad::EditorDirector::InitFileUiSystem2(TaskWindow* window, FileUiSystem* system)
//{
//
//
//    Map* map = EditorMapManager::CreateEngineSystemMap(system, "FileUiMainMap");
//    //   map->SetName("FileUiMainMap");
//     //  map->Initialize(system);
//    initDefaultMapSetting(window, map);
//
//
//    system->Initialize(window->GetClientWidth(), window->GetClientHeight(), map);
//    Map* currMap = system->GetMap();
//    //  OrthogoanlCamera* gameCamera = static_cast<OrthogoanlCamera*>(currMap->CreateObject("OrthogoanlCamera"));
//    OrthogoanlCamera* gameCamera = OrthogoanlCamera::Create(currMap, 0, window->GetClientWidth(),
//    window->GetClientHeight());
//    //  gameCamera->Initialize(window->GetClientWidth(), window->GetClientHeight());
//
//    currMap->SetMainCamera(gameCamera);
//    //map->SetCameraType(ECameraType::ePerspectiveCamera);
//    currMap->GetMainCamera()->GetTransform().SetPositionLocal({ 0,0,0.0f });
//
//
//}
//

// void Quad::EditorDirector::InitFrameWindowSystem(TaskWindow* window, FrameWindowSystem* system)
//{
//
//
//
//     Map* map = EditorSystem::CreateMap(system, "FrameWindowMainMap", false, false);
//
//     initDefaultMapSetting(window, map);
//
//
//     system->Initialize(window->GetClientWidth(), window->GetClientHeight(), map);
//
//     Map* currMap = system->GetMap();
//     //OrthogoanlCamera* gameCamera = static_cast<OrthogoanlCamera*>(currMap->CreateObject("OrthogoanlCamera"));
//     OrthogoanlCamera* gameCamera = OrthogoanlCamera::Create(currMap, 0, window->GetClientWidth(),
//     window->GetClientHeight());
//     //  gameCamera->Initialize(window->GetClientWidth(), window->GetClientHeight());
//
//
//     currMap->SetMainCamera(gameCamera);
//
//     //map->SetCameraType(ECameraType::ePerspectiveCamera);
//
//     currMap->GetMainCamera()->GetTransform().SetPositionLocal({ 0,0,0.0f });
// }

// void Quad::EditorDirector::InitFrameWindowUiSystem(TaskWindow* window, FrameWindowUiSystem* system)
//{
//
//
//    // Map* map = EditorMapManager::CreateEngineSystemMap(system, "FrameWindowUiMainMap");
//    // // map->SetName("FrameWindowUiMainMap");
//    ////  map->Initialize(system);
//
//
//    //
//
//
//    // system->Initialize(window->GetClientWidth(), window->GetClientHeight(), map);
//
//    // initDefaultMapSetting(window, map);
//
//    // system->Start();
//    // Map* currMap = system->GetMap();
//
//    // // OrthogoanlCamera* gameCamera = static_cast<OrthogoanlCamera*>(currMap->CreateObject("OrthogoanlCamera"));
//    // OrthogoanlCamera* gameCamera = OrthogoanlCamera::Create(currMap, 0, window->GetClientWidth(),
//    window->GetClientHeight());
//
//    // //  gameCamera->Initialize(window->GetClientWidth(), window->GetClientHeight());
//
//    // currMap->SetMainCamera(gameCamera);
//    // // map->SetCameraType(ECameraType::ePerspectiveCamera);
//    // currMap->GetMainCamera()->GetTransform().SetPositionLocal({ (float)window->GetClientWidth() / 2,
//    (float)-window->GetClientHeight() / 2,0.0f });
//    // //system->SetViewPort(0, 60, (float)window->GetClientWidth(), (float)window->GetClientHeight(), 0.0f, 1.0f);
//
//
//    // auto cameraEventComponentFactory = CameraEventComponentFactory::GetInstance();
//
//    // CameraFixWindowResizeEventComponent * cameraFixWindowResizeEventComponent =
//    cameraEventComponentFactory->CreateComponent< CameraFixWindowResizeEventComponent>();
//
//    // cameraFixWindowResizeEventComponent->Initialize(gameCamera);
//
//    // RegisterCameraEventComponentHelperMethod(gameCamera, cameraFixWindowResizeEventComponent);
//
//
// }

// void Quad::EditorDirector::InitGamePlayWindowLayoutSystem(TaskWindow* window, WindowChromeSystem* system)
//{
//
//   ////  VectorSpace* vectorSpace = new VectorSpace;
//   ////  vectorSpace->Initialize(100);
//   ////  CollisionWorld* collisionWorld = new CollisionWorld(vectorSpace);
//
//
//   //  Map* map = EditorMapManager::CreateEngineSystemMap(system, "GamePlayLayoutMainMap");
//   //  //  map->SetName("GamePlayLayoutMainMap");
//   //  //  map->Initialize(system);
//   ////  map->CreateMapLayer(0, 0, nullptr, collisionWorld, { 0,0,(float)window->GetClientWidth(),
//   (float)window->GetClientHeight() ,0.0f,1.0f });
//
//
//   //
//
//
//   //  system->Initialize(window->GetClientWidth(), window->GetClientHeight(), map);
//   //  initDefaultMapSetting(window, map);
//   //  system->Start();
//
//   //  Map* currMap = system->GetMap();
//
//   //  //ChromeSystemCamera* gameCamera =
//   static_cast<ChromeSystemCamera*>(currMap->CreateObject("ChromeSystemCamera"));
//   //  ChromeSystemCamera* gameCamera = ChromeSystemCamera::Create(currMap, 0, window->GetClientWidth(),
//   window->GetClientHeight());
//
//   //  // gameCamera->SetSystem(system);
//   //  // gameCamera->Initialize(window->GetClientWidth(), window->GetClientHeight());
//
//   //  currMap->SetMainCamera(gameCamera);
//
//
//
//   //  // system->SetTitleBarSize(3000, 40);
//
//   // //  WindowCloseButton* windowCloseButton = new WindowCloseButton("WindoeCloseButton");
//   // //  WindowCloseButton* windowCloseButton =
//   static_cast<WindowCloseButton*>(currMap->CreateObject("WindowCloseButton"));
//   //  WindowCloseButton* windowCloseButton = WindowCloseButton::Create(currMap, 0);
//   //  //windowCloseButton->SetSystem(system);
//   ////  windowCloseButton->Initialize();
//   //  windowCloseButton->SetKeepVisibleBaseHorizontalLineOffset(20);
//   //  windowCloseButton->SetKeepVisibleBaseVerticalLineOffset(20);
//
//
// }
//
// void Quad::EditorDirector::InitAttributeWindowLayoutSystem(TaskWindow* window, WindowChromeSystem* system)
//{
//
//   //  Map* map = EditorMapManager::CreateEngineSystemMap(system, "AttrLayoutMainMap");
//
//
//   //
//
//
//   //  system->Initialize(window->GetClientWidth(), window->GetClientHeight(), map);
//   //  initDefaultMapSetting(window, map);
//   //  system->Start();
//
//   //  Map* currMap = system->GetMap();
//   //  // system->SetTitleBarSize(3000, 40);
//   //  // ChromeSystemCamera* gameCamera =
//   static_cast<ChromeSystemCamera*>(currMap->CreateObject("ChromeSystemCamera"));
//   //  ChromeSystemCamera* gameCamera = ChromeSystemCamera::Create(currMap, 0, window->GetClientWidth(),
//   window->GetClientHeight());
//
//   //  //  gameCamera->SetSystem(system);
//   //   // gameCamera->Initialize(window->GetClientWidth(), window->GetClientHeight());
//
//   //  currMap->SetMainCamera(gameCamera);
//
//
//   //  //  WindowCloseButton* windowCloseButton =
//   static_cast<WindowCloseButton*>(currMap->CreateObject("WindowCloseButton"));
//   //  WindowCloseButton* windowCloseButton = WindowCloseButton::Create(currMap, 0);
//   //  // windowCloseButton->SetSystem(system);
//   ////   windowCloseButton->Initialize();
//   //  windowCloseButton->SetKeepVisibleBaseHorizontalLineOffset(20);
//   //  windowCloseButton->SetKeepVisibleBaseVerticalLineOffset(20);
//   //  windowCloseButton->SetDepth(2);
// }
//
// void Quad::EditorDirector::InitFileUiWindowLayoutSystem(TaskWindow* window, WindowChromeSystem* system)
//{
//
//
//   //  Map* map = EditorMapManager::CreateEngineSystemMap(system, "FileUiLayoutMainMap");
//
//   //  // map->SetName("FileUiLayoutMainMap");
//   // //  map->Initialize(system);
//
//   //
//
//
//   //  // map->AddObject(gameCamera);
//   //  system->Initialize(window->GetClientWidth(), window->GetClientHeight(), map);
//   //  initDefaultMapSetting(window, map);
//   //  system->Start();
//
//   //  // system->SetTitleBarSize(3000, 40);
//
//   //  Map* currMap = system->GetMap();
//   //  // ChromeSystemCamera* gameCamera =
//   static_cast<ChromeSystemCamera*>(currMap->CreateObject("ChromeSystemCamera"));
//   //  ChromeSystemCamera* gameCamera = ChromeSystemCamera::Create(currMap, 0, window->GetClientWidth(),
//   window->GetClientHeight());
//
//   //  // gameCamera->SetSystem(system);
//   //   //gameCamera->Initialize(window->GetClientWidth(), window->GetClientHeight());
//
//   //  currMap->SetMainCamera(gameCamera);
//
//   //  //   WindowCloseButton* windowCloseButton = new WindowCloseButton("WindoeCloseButton");
//   //    // WindowCloseButton* windowCloseButton =
//   static_cast<WindowCloseButton*>(currMap->CreateObject("WindowCloseButton"));
//   //  WindowCloseButton* windowCloseButton = WindowCloseButton::Create(currMap, 0);
//   //  //windowCloseButton->SetSystem(system);
//   ////  windowCloseButton->Initialize();
//   //  windowCloseButton->SetKeepVisibleBaseHorizontalLineOffset(20);
//   //  windowCloseButton->SetKeepVisibleBaseVerticalLineOffset(20);
// }
//
// void Quad::EditorDirector::InitFrameWindowLayoutSystem(TaskWindow* window, WindowChromeSystem* system)
//{
//
//
//
//
//  //   Map* map = EditorMapManager::CreateEngineSystemMap(system, "FrameWIindowLayoutMainMap");
//  //   //  map->SetName("FrameWIindowLayoutMainMap");
//  //    // map->Initialize(system);
//  //
//
//  //   //map->AddObject(gameCamera);
//  //   system->Initialize(window->GetClientWidth(), window->GetClientHeight(), map);
//
//  //   initDefaultMapSetting(window, map);
//  //   system->Start();
//
//  //   Map* currMap = system->GetMap();
//  //   int mapLayerIndex = 0;
//  //   //  ChromeSystemCamera* gameCamera =
//  static_cast<ChromeSystemCamera*>(currMap->CreateObject("ChromeSystemCamera"));
//  //   ChromeSystemCamera* gameCamera = ChromeSystemCamera::Create(currMap, 0, window->GetClientWidth(),
//  window->GetClientHeight());
//
//
//  //   //  gameCamera->SetSystem(system);
//  //   // gameCamera->Initialize(window->GetClientWidth(), window->GetClientHeight());
//
//  //   currMap->SetMainCamera(gameCamera);
//
//
//
//  //   //system->SetTitleBarSize(3000, 60);
//
//  //  // WindowCloseButton* windowCloseButton = new WindowCloseButton("WindoeCloseButton");
//  ////   WindowCloseButton* windowCloseButton =
//  static_cast<WindowCloseButton*>(currMap->CreateObject("WindowCloseButton"));
//  //   WindowCloseButton* windowCloseButton = WindowCloseButton::Create(currMap, mapLayerIndex);
//  //   //   windowCloseButton->SetSystem(system);
//  //    //  windowCloseButton->Initialize();
//  //   windowCloseButton->SetKeepVisibleBaseHorizontalLineOffset(20);
//  //   windowCloseButton->SetKeepVisibleBaseVerticalLineOffset(20);
//  //   windowCloseButton->SetDepth(2);
//
//  //   //   system->AddEntity(windowCloseButton);
//
//
//  //      //프레임윈도우 처음resize가 먼저 전달되서 안보이는것같다
//
//  //   //   WindowMaxRestoreButton* windowMaxRestoreButton = new WindowMaxRestoreButton("WindowMaxRestoreButton");
//  //     // WindowMaxRestoreButton* windowMaxRestoreButton =
//  static_cast<WindowMaxRestoreButton*>(currMap->CreateObject("WindowMaxRestoreButton"));
//  //   WindowMaxRestoreButton* windowMaxRestoreButton = WindowMaxRestoreButton::Create(currMap, mapLayerIndex);
//  //   //  windowMaxRestoreButton->SetSystem(system);
//  //   //  windowMaxRestoreButton->Initialize();
//  //   windowMaxRestoreButton->SetKeepVisibleBaseHorizontalLine(0);
//  //   windowMaxRestoreButton->SetKeepVisibleBaseVerticalLine(1);
//  //   windowMaxRestoreButton->SetKeepVisibleBaseHorizontalLineOffset(20);
//  //   windowMaxRestoreButton->SetKeepVisibleBaseVerticalLineOffset(60);
//
//  //   // system->AddEntity(windowMaxRestoreButton);
//
//  //   // WindowMinButton* windowMinButton = new WindowMinButton("WindowMinButton");
//  //   // WindowMinButton* windowMinButton = static_cast<WindowMinButton*>(currMap->CreateObject("WindowMinButton"));
//  //   WindowMinButton* windowMinButton = WindowMinButton::Create(currMap, mapLayerIndex);
//  //   // windowMinButton->SetSystem(system);
//  //  //  windowMinButton->Initialize();
//  //   windowMinButton->SetKeepVisibleBaseHorizontalLine(0);
//  //   windowMinButton->SetKeepVisibleBaseVerticalLine(1);
//  //   windowMinButton->SetKeepVisibleBaseHorizontalLineOffset(20);
//  //   windowMinButton->SetKeepVisibleBaseVerticalLineOffset(100);
//
//  //   //  system->AddEntity(windowMinButton);
//
//
//
//
// }
//
// void Quad::EditorDirector::InitFrameWindowDockingSystem(TaskWindow* window, FrameWindowDockingSystem* system)
//{
//
//
//    // Map* map = EditorMapManager::CreateEngineSystemMap(system, "FrameWindowDockingMainMap");
//
//    // // map->AddObject(gameCamera);
//    // system->Initialize(window->GetClientWidth(), window->GetClientHeight(), map);
//
//    // initDefaultMapSetting(window, map);
//
//    // system->Start();
//
//    // Map* currMap = system->GetMap();
//    // //  DockingSystemCamera* gameCamera =
//    static_cast<DockingSystemCamera*>(currMap->CreateObject("DockingSystemCamera"));
//    // DockingSystemCamera* gameCamera = DockingSystemCamera::Create(currMap, 0, window->GetClientWidth(),
//    window->GetClientHeight());
//    // // DockingSystemCamera* gameCamera = ObjectFactory::CreateObject()
//    //// gameCamera->Initialize(window->GetClientWidth(), window->GetClientHeight());
//
//    // currMap->SetMainCamera(gameCamera);
// }
//
// void Quad::EditorDirector::InitChildWindowDockingSystem(TaskWindow* window, ChildWindowDockingSystem* system)
//{
//
//
//
//
//     //Map* map = EditorMapManager::CreateEngineSystemMap(system, "ChildWindowDockingMainMap" +
//     std::to_string((int)system->GetSystemID()));
//
//
//
//     //system->Initialize(window->GetClientWidth(), window->GetClientHeight(), map);
//     //initDefaultMapSetting(window, map);
//     //system->Start();
//
//
//     //Map* currMap = system->GetMap();
//     ////  DockingSystemCamera* gameCamera =
//     static_cast<DockingSystemCamera*>(currMap->CreateObject("DockingSystemCamera"));
//     //DockingSystemCamera* gameCamera = DockingSystemCamera::Create(currMap, 0, window->GetClientWidth(),
//     window->GetClientHeight());
//
//     ////gameCamera->Initialize(window->GetClientWidth(), window->GetClientHeight());
//
//     //currMap->SetMainCamera(gameCamera);
//
// }
//
// void Quad::EditorDirector::initDefaultMapSetting(TaskWindow* window, Map* map)
//{
//     //default Ui - mapLayer 0
//     map->CreateMapLayer(0, 0, nullptr, {
//     0,0,(float)window->GetClientWidth(),(float)window->GetClientHeight(),0.0f,1.0f });
//     //default 3D - mapLayer 1
//     map->CreateMapLayer(1, 0, nullptr, {
//     0,0,(float)window->GetClientWidth(),(float)window->GetClientHeight(),0.0f,1.0f });
//
//
//     //new SpacePartitioningStructureFactory<UiCollider>(nullptr);
//     //ui collisionWorld
//     auto spacePartitioningStructureFactoryUi = SpacePartitioningStructureFactory<UiCollider>::GetInstance();
//     SpacePartitioningStructure<UiCollider>* spacePartitioningStructureUi =
//     spacePartitioningStructureFactoryUi->CreateSpacePartitioningStructure("VectorSpace");
//
//     auto collisionWorldFactory = CollisionWorldFactory::GetInstance();
//     UiCollisionWorld* CollisionWorldUi = collisionWorldFactory->CreateUiCollisionWorld(spacePartitioningStructureUi);
//
//     map->SetDefaultUiCollisionWorld(CollisionWorldUi);
//
//
//
//     //3d collisionWorld
//     auto spacePartitioningStructureFactory3D = SpacePartitioningStructureFactory<Collider>::GetInstance();
//     SpacePartitioningStructure<Collider>* spacePartitioningStructure3D =
//     spacePartitioningStructureFactory3D->CreateSpacePartitioningStructure("VectorSpace");
//
//     //auto collisionWorldFactory = CollisionWorldFactory::GetInstance();
//     CollisionWorld* CollisionWorld3D = collisionWorldFactory->CreateCollisionWorld(spacePartitioningStructure3D);
//
//     map->SetDefault3DCollisionWorld(CollisionWorld3D);
//
//
//
//
//
//
// }
//
// void Quad::EditorDirector::AddVisibleEditorAssetToFileWindow(const std::string& configFilePath)
//{
//
//
//     JsonParser::ReadFile(configFilePath);
//     JsonParser::ReadStart();
//     JsonParser::DescendIntoObjectOrArray();
//
//
//     rapidjson::Value::ConstArray  textureArray = JsonParser::ReadArray("Texture");
//     std::vector<Asset*> assetVector;
//
//     for (auto& element : textureArray)
//     {
//
//         std::string textureName= element.GetString();
//         Asset * asset =  TextureManager::GetTexture(textureName);
//         if (asset != nullptr)
//         {
//             assetVector.push_back(asset);
//         }
//      }
//
//     rapidjson::Value::ConstArray  matArray = JsonParser::ReadArray("Material");
//
//     for (auto& element : matArray)
//     {
//
//         std::string matName = element.GetString();
//         Asset* asset = MaterialManager::GetMaterial(matName);
//         if (asset != nullptr)
//         {
//             assetVector.push_back(asset);
//         }
//     }
//
//
//     rapidjson::Value::ConstArray  meshArray = JsonParser::ReadArray("Mesh");
//     for (auto& element : meshArray)
//     {
//
//         std::string meshName = element.GetString();
//         Asset* asset = MeshManager::GetMesh(meshName);
//         if (asset != nullptr)
//         {
//             assetVector.push_back(asset);
//         }
//     }
//
//
//     FileUiUiSystem* fileUiSystem = FileUiUiSystem::GetInstance();
//     fileUiSystem->AddAsset(assetVector);
//
//
// }

void Quad::EditorDirector::CreateDefaultMaterial()
{

    /* auto * materialManager =  MaterialManager::GetInstance();

     const std::string defaultMaterialName = "Default";
     Material * defaultMaterial =  materialManager->CreateMaterial(defaultMaterialName);

     defaultMaterial->SetDiffuseMap(TextureManager::GetTexture("Default.bmp"));*/
}

void Quad::EditorDirector::CreateDefaultMesh()
{
    ////Squre Mesh

    // MeshManager* meshManager = MeshManager::GetInstance();
    // StaticMesh * squareMesh  =static_cast<StaticMesh*>( meshManager->CreateMesh("Rectangle",
    // Quad::EMeshType::eStaticMesh));

    //
    //
    // std::vector<StaticVertex> vertexVector(4);
    // std::vector<MeshIndexType> indexVector(6);

    // vertexVector[0].mPos = { -0.5f,0.5f,0.0f };
    // vertexVector[1].mPos = { 0.5f,0.5f,0.0f };
    // vertexVector[2].mPos = { 0.5f,-0.5f,0.0f };
    // vertexVector[3].mPos = { -0.5f,-0.5f,0.0f };

    // vertexVector[0].mNormal = { 0.0f,0.0f,-1.0f };
    // vertexVector[1].mNormal = { 0.0f,0.0f,-1.0f };
    // vertexVector[2].mNormal = { 0.0f,0.0f,-1.0f };
    // vertexVector[3].mNormal = { 0.0f,0.0f,-1.0f };

    // vertexVector[0].mTex = { 0.0f,0.0f };
    // vertexVector[1].mTex = { 1.0f,0.0f };
    // vertexVector[2].mTex = { 1.0f,1.0f };
    // vertexVector[3].mTex = { 0.0f,1.0f };

    // indexVector[0] = 0;
    // indexVector[1] = 1;
    // indexVector[2] = 2;
    //
    // indexVector[3] = 0;
    // indexVector[4] = 2;
    // indexVector[5] = 3;

    //
    // squareMesh->SetVertexVector(std::move(vertexVector));
    // squareMesh->SetIndexVector(std::move(indexVector));

    // squareMesh->SetIndexNum(6);
    // squareMesh->SetVertexNum(4);

    // std::vector<SubMesh> subMeshVector(1);

    // subMeshVector[0].mID = 0;
    // subMeshVector[0].mIndexRange.first = 0;
    // subMeshVector[0].mIndexRange.second = 6;
    // subMeshVector[0].mVertexNum = 4;
    // subMeshVector[0].mName = "DefaultSubMesh";
    // subMeshVector[0].mVertexOffset = 0;
    // subMeshVector[0].mMaterialPointer = MaterialManager::GetMaterial("Default");

    // squareMesh->SetSubMeshVector(std::move(subMeshVector));
    // squareMesh->SetMinMaxPoint({ -0.5f,-0.5f,-0.5f }, { 0.5f,0.5f,0.5f });
}

void Quad::EditorDirector::SwitchFrameWindow()
{

    //   mProjectDirector.SetOhterWindowSystemOff();
    // mFrameWindow->SetProjectSelectSceneFlag(true);
    // FrameWindowController::GetInstance()->SetSystemActiveState(2, false);
}

void Quad::EditorDirector::SwitchCommonEditWindow()
{
    // mProjectDirector.SetOhterWindowSystemOn();
    //   mFrameWindow->SetProjectSelectSceneFlag(false);
    //  FrameWindowController::GetInstance()->SetSystemActiveState(2, true);
}

void Quad::EditorDirector::SetPlayModeState(bool state)
{
    // auto instance = GetInstance();
    //    mPlayModeState = state;
    //   mRenderWindowTest->SetPlayMode(state);
}

bool Quad::EditorDirector::GetPlayModeState()
{
    auto instance = GetInstance();
    return true;
    // return instance->mPlayModeState;
}

const std::string &Quad::EditorDirector::GetEditorPathA() const
{

    return ""; // mEditorPathA;
    // TODO: 여기에 return 문을 삽입합니다.
}

const std::wstring &Quad::EditorDirector::GetEditorPathW() const
{
    return L""; // mEditorPathW;
    // TODO: 여기에 return 문을 삽입합니다.
}

// void Quad::EditorDirector::AddEffect(RenderSystem* renderSystem, Effect* effect ,ESystemType systemType)
//{
//  //   renderSystem->AddEffect(*effect, systemType);
//   //  renderSystem->ReigsterDefaultEffect(systemType, effect->GetName());
// }

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR pCmdLine, int nCmdShow)
{

#ifdef _DEBUG
    // bool ret = SetDllDirectory(L"C:\\Users\\dongd\\gitproject\\GameEngine\\Dll\\x64\\Debug\\");

    // if (ret != true)
    //   assert(1);
    // 메모리 누수 체크 활성화
    //  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#else

    SetDllDirectoryW(L".\\Dll\\x64\\Release\\");

#endif

    // Editor 설정
    Quad::EditorConfig *editorConfig = Quad::EditorConfig::GetInstance();

    wchar_t editorRootPath[255];
    GetCurrentDirectoryW(255, editorRootPath);
    editorConfig->SetEditorRootPath(CoreUtility::Utility::ConvertToString(editorRootPath, true));

    int cmdNum;
    LPWSTR *cmdLists = CommandLineToArgvW(pCmdLine, &cmdNum);

    MessageBoxW(nullptr, cmdLists[0], L"ProjectPath", 0);

    QuadLog::Logger *logger = QuadLog::Logger::GetInstance();

    Quad::ProjectConfig *projectConfig = Quad::ProjectConfig::GetInstance();

    std::string projectFolderPath;
#ifdef _DEBUG

    projectFolderPath = editorConfig->GetEditorRootPath() + "\\TestProject";
    ;
#else

    projectFolderPath = CoreUtility::Utility::ConvertToString(cmdLists[0], true);
#endif
    projectConfig->SetProjectPath(projectFolderPath);

    logger->SetLoggerFile((projectConfig->GetProjectPath() + "\\LogFile.txt").c_str());

    Quad::Application *app = Quad::Application::GetInstance();
    Quad::EditorDirector *editorDirector = Quad::EditorDirector::GetInstance();

    Quad::AppInitData appInitData;
    appInitData.hInstance = hInstance;
    appInitData.nShowCmd = nCmdShow;
    appInitData.programDirector = editorDirector;

    if (!app->Initialize(appInitData))
        return 0;

    return app->Run();
}
