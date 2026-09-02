#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "EditorDirector/EditorDirector.h"
#include <Core/Application.h>
#include <Core/ProjectConfig.h>
#include <Utility/Utility.h>

#include "EditorAssetImporterModule.h"
#include "EditorDirector/EditorProjectManager.h"
#include "EditorDirector/EditorShaderImporter.h"
#include "EditorDirector/EditorTextureImporter.h"
#include "EditorDirector/SerializedAssetContainer.h"
#include "EditorDirector/SuperAssetBrowerController.h"
#include "EditorDirector/SuperFrameController.h"
#include <BinaryReaderWriter/BinaryReader.h>
#include <ClassGenerationManager.h>
#include <Core/MapFactory.h>
#include <Core/MapLoader.h>
#include <Core/MapStorer.h>
#include <Core/PrefabFactory.h>
#include <Core/PrefabLoader.h>
#include <Core/RuntimeServices.h>
#include <Core/SceneManager.h>
#include <CoreAsset/AssetCommon.h>
#include <CoreAsset/AssetFactoryManager.h>
#include <CoreAsset/AssetIOManager.h>
#include <CoreAsset/AssetImporterManager.h>
#include <CoreAsset/AssetLoader.h>
#include <CoreAsset/AssetManager.h>
#include <CoreAsset/AssetMetaDataManager.h>
#include <CoreAsset/AssetType.h>
#include <CoreAsset/FontFactory.h>
#include <CoreAsset/Material.h>
#include <CoreAsset/MaterialFactory.h>
#include <CoreAsset/MaterialLoader.h>
#include <CoreAsset/MaterialStorer.h>
#include <CoreAsset/MeshFactory.h>
#include <CoreAsset/MeshLoader.h>
#include <CoreAsset/MeshStorer.h>
#include <CoreAsset/StaticMesh.h>
#include <CoreAsset/Texture.h>
#include <CoreAsset/TextureFactory.h>
#include <CoreAsset/TextureLoader.h>
#include <CoreAsset/TextureManager.h>
#include <CoreAsset/TextureStorer.h>
#include <CoreAsset/UIMaterialManager.h>
#include <CoreBase/AsyncThreadPool.h>
#include <CoreBase/BinaryArch.h>
#include <CoreBase/CoreAssert.h>
#include <CoreBase/FNameTable.h>
#include <CoreBase/TextArch.h>
#include <D3DGpuResourceManager/GpuBufferContextSystem.h>
#include <D3DGpuResourceManager/GpuSamplerSystem.h>
#include <D3DGpuResourceManager/IGpuResourceManager.h>
#include <DefaultEditorInspectorManager.h>
#include <DefaultPropertyInspector.h>
#include <EditorAssetImporterManager.h>
#include <EditorDirector/EditorAssetImporterManager.h>
#include <EditorDirector/EditorAssetManager.h>
#include <EditorDirector/EditorConfig.h>
#include <EditorDirector/EditorProjectBrowserManager.h>
#include <EditorDirector/EditorSceneManager.h>
#include <EditorDirector/EditorSelectionManager.h>
#include <EditorDirector/GlobalOverlayManager.h>
#include <EditorDirector/ObjectHierarchyPanel.h>
#include <EditorDirector/ProjectGenerator.h>
#include <EditorDirector/UIAssetBrowser.h>
#include <IEditorTaskManager.h>
#include <ImportModule/TextureImporter.h>
#include <InputSystem/InputSystem.h>
#include <Logger/Logger.h>
#include <LogicalFileSystem/LogicalFile.h>
#include <LogicalFileSystem/LogicalFileSystem.h>
#include <LogicalFileSystem/LogicalFolder.h>
#include <MapSettingUIController.h>
#include <MaterialWorkSpaceManager.h>
#include <PhysicalFileSystem/PhysicalFileSystem.h>
#include <PrefabWorkSpaceManager.h>
#include <PropertyPanel.H>
#include <RenderFrontend/AssetResolver.h>
#include <RenderFrontend/ObjectRenderItemBuilder.h>
#include <RenderFrontend/RenderPipelineManager.h>
#include <RenderFrontend/UIRenderItemBuilder.h>
#include <RenderSystem/IMaterialManager.h>
#include <SystemInitializer/ISystemInitializer.h>
#include <UIScrollBox.h>
#include <UISystem/UIManager.h>
#include <UiSystem/UIElementPtr.h>
#include <UiSystem/UIImage.h>
#include <Window/BaseWindow.h>
#include <core/PrefabStorer.h>
#include <memory>

#pragma comment(lib, "ImportModule.lib")

#ifdef D3DX

#include <SystemInitializer/D3DSystemInitializer.h>

#endif

#include <fstream>

#undef EngineMode

// std::string testProjectPath = "C:/Users/dongd/gitproject/GameEngine";

Quad::EditorDirector *Quad::EditorDirector::GetInstance()
{
    static EditorDirector instance;
    return &instance;
}

Quad::EditorDirector::EditorDirector()
    : mRenderPipelineManager(Render::RenderPipelineManager::GetInstance()), mApp(nullptr), mAssetManager(nullptr)
/* :mFrameWindowSceneSwitch(0), mWindowSwitchRenderFlag(false)*/
{
}
Quad::EditorDirector::~EditorDirector() {}
// #define EngineMode
void Quad::EditorDirector::Initialize()
{

    auto app = Application::GetInstance();
    mApp = app;

    InitSystems();
    LoadEditorAssets();

    mSuperFrameController = SuperFrameController::GetInstance();
    mSuperFrameController->Initialize(*mRenderPipelineManager);
    mSuperControllerVector.push_back(mSuperFrameController);

    // project init
    EEditorMode editorMode = EditorConfig::GetInstance()->GetEditorMode();

    if (editorMode == EEditorMode::eEditProject)
    {

        mProjectInitializer = EditorProjectManager::GetInstance();
        mProjectInitializer->Initialize();
        CreateEditWorkSpace();
    }
    else if (editorMode == EEditorMode::eProjectBrowser)
    {
        InitProjectBrowserWindow();
        EditorProjectBrowserManager *editorProjectBrowserManager = EditorProjectBrowserManager::GetInstance();
        editorProjectBrowserManager->Initialize(mProjectBrowserLogicalWindow->GetActiveCanvas());
    }

    FNameTable *nameTable = FNameTable::GetInstance();
}

void Quad::EditorDirector::Initialize(const RuntimeServices &services)
{
    mRuntimeServices = &services;
    mAssetManager = mRuntimeServices->mAssetManager;
    mUIManager = mRuntimeServices->mUIManager;
    mRenderSystem = mRuntimeServices->mRenderSystem;
    mGpuResourceManager = mRuntimeServices->mGpuResourceManager;

    Initialize();
}

void Quad::EditorDirector::Begin()
{
    if (Quad::EditorConfig::GetInstance()->GetEditorMode() == EEditorMode::eEditProject)
    {

        mProjectInitializer->InitProject();

        DefaultEditorInspectorManager::GetInstance()->BeginInspectorUI();
    }
}

void Quad::EditorDirector::PreUpdate(float deltaTime)
{

    InputSystem *inputSystem = InputSystem::GetInstance();

    mSuperFrameController->PreUpdate();
}

void Quad::EditorDirector::Update(float deltaTime)
{

    mRenderPipelineManager->Update(mApp->GetTotalFrameCount(), mApp->GetLastCompletedFenceValue());

    UpdateEditorTaskManagers();

    //    UI::UIManager *uiManager = UI::UIManager::GetInstance();
    mUIManager->Update(deltaTime);

    // 지금이렇게 순서대로하는데 멀티스레드 고려를해봐야한다.
    mSuperFrameController->Update(deltaTime);
    // mSuperAssetBrowerController->Update(deltaTime);

    PrefabWorkSpaceManager::GetInstance()->Update(deltaTime);

    Quad::EditorSceneManager::GetInstance()->Update(deltaTime);

    GlobalOverlayManager::GetInstance()->Update(deltaTime);
}

void Quad::EditorDirector::EndUpdate(float deltaTime)
{

    // 마지막에는 각 가지고있는 채널들에대해서 렌더아이템구축후 렌더시스템으로 전달
    //  mSuperFrameController
    // mSuperAssetBrowerController

    mSuperFrameController->EndUpdate();
    // mSuperAssetBrowerController->EndUpdate();
    Quad::EditorSceneManager::GetInstance()->EndUpdate(deltaTime);
}

void Quad::EditorDirector::CleanUp()
{

    EditorSceneManager::GetInstance()->CleanUp();
    //    UI::UIManager::GetInstance()->CleanUp();
    mUIManager->CleanUp();
}

void Quad::EditorDirector::Draw()
{

    // 렌더시스템이 렌더 혹은

    // 각 컨트롤러들이 draw  (이미 렌더아이템들은 다 제출된상황)
    mSuperFrameController->Draw(*mRenderPipelineManager);
    // mSuperAssetBrowerController->Draw();
}

void Quad::EditorDirector::EndFrame()
{
    Render::UIRenderItemBuilder::GetInstance()->EndFrame();
    Render::RenderPipelineManager::GetInstance()->EndFrame();
}

void Quad::EditorDirector::ShutDownWindow()
{
    mSuperFrameController->ShutDownWindow();
}

void Quad::EditorDirector::EndSystem()
{
    Render::RenderPipelineManager::GetInstance()->EndRenderThread();
    Render::AssetResolver::GetInstance()->EndResourceResolveThread();
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

Core::LogicalWindow *Quad::EditorDirector::GetMainSceneWindow() const
{
    return mMainSceneLogicalWindow.get();
}

void Quad::EditorDirector::ChangeToPrefabEditWorkSpace()
{

    GlobalOverlayManager::GetInstance()->ChangeToPrefabEdit();
    ChangeWorkSpace(PrefabWorkSpaceManager::GetInstance()->GetWorkSpace());
    PrefabWorkSpaceManager::GetInstance()->OnPrefabEditActive();
}

void Quad::EditorDirector::ChangeToMaterialEditWorkSpace(CoreAsset::Material *targetMaterial)
{

    GlobalOverlayManager::GetInstance()->ChangeToMaterialEdit();

    MaterialWorkSpaceManager *materialWorkSpaceManager = MaterialWorkSpaceManager ::GetInstance();
    ChangeWorkSpace(materialWorkSpaceManager->GetWorkSpace());
    materialWorkSpaceManager->OnWorkSpaceActive();
    materialWorkSpaceManager->SetMaterial(targetMaterial);
}

void Quad::EditorDirector::ChangeToDefaultEditWorkSpace()
{
    GlobalOverlayManager::GetInstance()->ChangeToDefaultEdit();
    ChangeWorkSpace(mDefaultEditWorkSpace.get());
}

void Quad::EditorDirector::ChangeWorkSpace(Core::WorkSpace *workspace)
{

    Core::WorkSpace *currentWorkSpace = mSuperFrameController->GetWorkSpace();

    if (currentWorkSpace == PrefabWorkSpaceManager::GetInstance()->GetWorkSpace())
    {
        PrefabWorkSpaceManager::GetInstance()->OnPrefabEditInActive();
    }
    else if (currentWorkSpace == MaterialWorkSpaceManager::GetInstance()->GetWorkSpace())
    {
        MaterialWorkSpaceManager::GetInstance()->OnWorkSpaceInActive();
    }

    mSuperFrameController->SetWorkSpace(workspace);
}

void Quad::EditorDirector::LoadEditorAssets()
{

    bool engineImport = true;
    if (engineImport)
    {
        std::filesystem::path engineAssetPath = EditorConfig::GetInstance()->GetEditorAssetPath();

        std::vector<std::filesystem::path> assetFileList;
        mPhysicalFileSystem->GetFileList(engineAssetPath, assetFileList);

        // std::ifstream fin("C:\\Users\\dongd\\gitproject\\GameEngine\\TestAssetPathFile.txt");
        for (const auto &path : assetFileList)
        {
            EditorAssetImporterManager::GetInstance()->RequestImportSync(path, true);
        }

        // std::string assetFilePath;

        // while (fin >> assetFilePath)
        //{
        //     // 현재 논리적폴더에서 해당 파일과 동일한 이름을 가진 논리적파일이존재하면실패하도록한다.
        //     // Check Logical File Name

        //    // 없으면 임포트수행

        //    // 동기버전도 필용할듯
        //    // 그리고 잡context가 필요없는 버전도 필요할듯
        //    EditorAssetImporterManager::GetInstance()->RequestImportSync(assetFilePath.c_str(), true);
        //    // EditorAssetImporterModule::GetInstance()->Import(assetFilePath.c_str(), true);
        //}
    }

    InitEngineAssetLogicalFile();
}

void Quad::EditorDirector::InitProjectBrowserWindow()
{

    mProjectBrowserWorkSpace = std::make_unique<Core::WorkSpace>();

    mProjectBrowserLogicalWindow = std::make_unique<Core::LogicalWindow>();

    //    UI::UIManager *uiManager = UI::UIManager::GetInstance();
    UI::UICanvasID canvasID = mUIManager->CreateCanvas("ProjectBrowser", UI::ECanvasSizeMode::eFixSize);

    UI::UICanvas *canvas = mUIManager->GetCanvas(canvasID);

    auto projectBrowserPanel = canvas->CreateUIElement<UI::UIImage>("ProjectBrowserBackground");
    projectBrowserPanel->SetSize(1700, 1000);
    projectBrowserPanel->SetPositionLocal(0, 0);
    mProjectBrowserLogicalWindow->SetActiveCanvas(canvas);

    mProjectBrowserLogicalWindow->mViewportController.SetViewportMode(Core::EViewportMode::eAnchored);

    mProjectBrowserLogicalWindow->mViewportController.SetAnchorLeftState(true);
    mProjectBrowserLogicalWindow->mViewportController.SetAnchorLeftMode(Core::EViewportAnchoredMode::eRelative);
    mProjectBrowserLogicalWindow->mViewportController.SetAnchorLeftRelValue(0.0f);

    mProjectBrowserLogicalWindow->mViewportController.SetAnchorRightState(true);
    mProjectBrowserLogicalWindow->mViewportController.SetAnchorRightMode(Core::EViewportAnchoredMode::eRelative);
    mProjectBrowserLogicalWindow->mViewportController.SetAnchorRightRelValue(0.0f);

    mProjectBrowserLogicalWindow->mViewportController.SetAnchorTopState(true);
    mProjectBrowserLogicalWindow->mViewportController.SetAnchorTopMode(Core::EViewportAnchoredMode::ePixel);
    mProjectBrowserLogicalWindow->mViewportController.SetAnchorTopRelValue(0.0f);

    mProjectBrowserLogicalWindow->mViewportController.SetAnchorBottomState(true);
    mProjectBrowserLogicalWindow->mViewportController.SetAnchorBottomMode(Core::EViewportAnchoredMode::eRelative);
    mProjectBrowserLogicalWindow->mViewportController.SetAnchorBottomRelValue(0.0f);

    mProjectBrowserWorkSpace->AddLogicalWindow(mProjectBrowserLogicalWindow.get());

    mSuperFrameController->SetWorkSpace(mProjectBrowserWorkSpace.get());
    // mSuperFrameController->AddLogicalWindow(mProjectBrowserLogicalWindow.get());
}

void Quad::EditorDirector::InitEngineAssetLogicalFile()
{
    // auto assetManager = CoreAsset::AssetManager::GetInstance();
    // 기본에셋
    // cube
    QuadLF::LogicalFileAssetInfo cubeInfo;
    cubeInfo.mName = "Cube";
    cubeInfo.mAssetType = CoreAsset::EAssetType::eStaticMesh;
    cubeInfo.mAssetID = mAssetManager->GetAsset<CoreAsset::StaticMesh>("Engine/Cube").GetAssetID();

    mLogicalFileSystem->MakeFile(cubeInfo, "Cube", mLogicalFileSystem->GetEngineFolder());

    // Cylinder
    QuadLF::LogicalFileAssetInfo cylinderInfo;
    cylinderInfo.mName = "Cylinder";
    cylinderInfo.mAssetType = CoreAsset::EAssetType::eStaticMesh;
    cylinderInfo.mAssetID = mAssetManager->GetAsset<CoreAsset::StaticMesh>("Engine/Cylinder").GetAssetID();

    mLogicalFileSystem->MakeFile(cylinderInfo, "Cylinder", mLogicalFileSystem->GetEngineFolder());
}

void Quad::EditorDirector::CreateEditWorkSpace()
{

    CreateDefaultEditWorkSpace();

    CreatePrefabEditWorkSpace();

    CreateMaterialEditWorkSpace();

    InitEditorTaskManagerList();
}

void Quad::EditorDirector::CreateDefaultEditWorkSpace()
{
    mDefaultEditWorkSpace = std::make_unique<Core::WorkSpace>();
    InitEditorWindows();

    mSuperFrameController->SetWorkSpace(mDefaultEditWorkSpace.get());
}

void Quad::EditorDirector::CreatePrefabEditWorkSpace()
{

    auto prefabWorkSpaceManager = PrefabWorkSpaceManager::GetInstance();

    //   UI::UIManager *uiManager = UI::UIManager::GetInstance();
    UI::UICanvasID canvasID = mUIManager->CreateCanvas("PrefabEditCanvas", UI::ECanvasSizeMode::eFixSize);

    UI::UICanvas *canvas = mUIManager->GetCanvas(canvasID);
    prefabWorkSpaceManager->Initialize(canvas, mGlobalOverlayLogicalWindow.get(),
                                       EditorPrefabSelectionManager::GetInstance());
}

void Quad::EditorDirector::CreateMaterialEditWorkSpace()
{

    auto materialEditWorkSpaceManager = MaterialWorkSpaceManager::GetInstance();

    materialEditWorkSpaceManager->Initialize(mGlobalOverlayLogicalWindow.get(),
                                             EditorMaterialSelectionManager::GetInstance());
}

void Quad::EditorDirector::InitEditorWindows()
{

    // 논리적윈도우생성+ 초기화 + 실제 물리적윈도우에 등록

    InitMainSceneWindow();
    InitPropertyWindow();
    InitAssetBrowerWindow();
    InitGlobalOverlayWindow();
}

void Quad::EditorDirector::InitMainSceneWindow()
{
    mMainSceneLogicalWindow = std::make_unique<Core::LogicalWindow>();

    mMainSceneLogicalWindow->mViewportController.SetViewportMode(Core::EViewportMode::eAnchored);

    mMainSceneLogicalWindow->mViewportController.SetAnchorLeftState(true);
    mMainSceneLogicalWindow->mViewportController.SetAnchorLeftMode(Core::EViewportAnchoredMode::eRelative);
    mMainSceneLogicalWindow->mViewportController.SetAnchorLeftRelValue(0.0f);

    mMainSceneLogicalWindow->mViewportController.SetAnchorRightState(true);
    mMainSceneLogicalWindow->mViewportController.SetAnchorRightMode(Core::EViewportAnchoredMode::eRelative);
    mMainSceneLogicalWindow->mViewportController.SetAnchorRightRelValue(0.3f);

    mMainSceneLogicalWindow->mViewportController.SetAnchorTopState(true);
    mMainSceneLogicalWindow->mViewportController.SetAnchorTopMode(Core::EViewportAnchoredMode::eRelative);
    mMainSceneLogicalWindow->mViewportController.SetAnchorTopRelValue(0.1f);
    // mMainSceneLogicalWindow->mViewportController.SetAnchorTopPixelValue(100.0f);

    mMainSceneLogicalWindow->mViewportController.SetAnchorBottomState(true);
    mMainSceneLogicalWindow->mViewportController.SetAnchorBottomMode(Core::EViewportAnchoredMode::eRelative);
    mMainSceneLogicalWindow->mViewportController.SetAnchorBottomRelValue(0.25f);

    mMainSceneLogicalWindow->m3DWorldViewportController.SetViewportMode(Core::EViewportMode::eAnchored);
    mMainSceneLogicalWindow->m3DWorldViewportController.SetAnchorLeftState(true);
    mMainSceneLogicalWindow->m3DWorldViewportController.SetAnchorLeftMode(Core::EViewportAnchoredMode::eRelative);
    mMainSceneLogicalWindow->m3DWorldViewportController.SetAnchorLeftRelValue(0.0f);

    mMainSceneLogicalWindow->m3DWorldViewportController.SetAnchorRightState(true);
    mMainSceneLogicalWindow->m3DWorldViewportController.SetAnchorRightMode(Core::EViewportAnchoredMode::eRelative);
    mMainSceneLogicalWindow->m3DWorldViewportController.SetAnchorRightRelValue(0.0f);

    mMainSceneLogicalWindow->m3DWorldViewportController.SetAnchorTopState(true);
    mMainSceneLogicalWindow->m3DWorldViewportController.SetAnchorTopMode(Core::EViewportAnchoredMode::eRelative);
    mMainSceneLogicalWindow->m3DWorldViewportController.SetAnchorTopRelValue(0.0f);
    // mMainSceneLogicalWindow->mViewportController.SetAnchorTopPixelValue(0.0f);

    mMainSceneLogicalWindow->m3DWorldViewportController.SetAnchorBottomState(true);
    mMainSceneLogicalWindow->m3DWorldViewportController.SetAnchorBottomMode(Core::EViewportAnchoredMode::eRelative);
    mMainSceneLogicalWindow->m3DWorldViewportController.SetAnchorBottomRelValue(0.0f);

    mMainSceneLogicalWindow->SetWorld(EditorSceneManager::GetInstance()->GetUserWorld());
    mMainSceneLogicalWindow->SetBackBufferClearColor(0.2f, 0.3f, 0.5f, 1.0f);

    mDefaultEditWorkSpace->AddLogicalWindow(mMainSceneLogicalWindow.get());

    mMainSceneLogicalWindow->SetDebugGridRender(true);

    EditorSceneManager::GetInstance()->GetUserWorld()->mOnMapObjectRemovedCallbackSystem.Register(
        [](Object *object) { EditorSelectionManager::GetInstance()->OnMapObjectRemoved(object); });

    // mSuperFrameController->AddLogicalWindow(mMainSceneLogicalWindow.get());
}

void Quad::EditorDirector::InitAssetBrowerWindow()
{
    mAssetBrowserLogicalWindow = std::make_unique<Core::LogicalWindow>();

    // UI::UIManager *uiManager = UI::UIManager::GetInstance();
    UI::UICanvasID canvasID = mUIManager->CreateCanvas("AssetBrowserCanvas", UI::ECanvasSizeMode::eFixSize);

    UI::UICanvas *canvas = mUIManager->GetCanvas(canvasID);

    // auto uiAssetBrowser = canvas->CreateUIElement<UIAssetBrowser>("UIAssetBrowser");
    // uiAssetBrowser->SetSize(1700, 500);
    // uiAssetBrowser->SetPositionLocal(0, 0);

    mAssetBrowserLogicalWindow->SetActiveCanvas(canvas);

    mAssetBrowserLogicalWindow->mViewportController.SetViewportMode(Core::EViewportMode::eAnchored);

    mAssetBrowserLogicalWindow->mViewportController.SetAnchorLeftState(true);
    mAssetBrowserLogicalWindow->mViewportController.SetAnchorLeftMode(Core::EViewportAnchoredMode::eRelative);
    mAssetBrowserLogicalWindow->mViewportController.SetAnchorLeftRelValue(0.0f);

    mAssetBrowserLogicalWindow->mViewportController.SetAnchorRightState(true);
    mAssetBrowserLogicalWindow->mViewportController.SetAnchorRightMode(Core::EViewportAnchoredMode::eRelative);
    mAssetBrowserLogicalWindow->mViewportController.SetAnchorRightRelValue(0.3);

    mAssetBrowserLogicalWindow->mViewportController.SetAnchorTopState(true);
    mAssetBrowserLogicalWindow->mViewportController.SetAnchorTopMode(Core::EViewportAnchoredMode::eRelative);
    mAssetBrowserLogicalWindow->mViewportController.SetAnchorTopRelValue(0.75f);

    mAssetBrowserLogicalWindow->mViewportController.SetAnchorBottomState(true);
    mAssetBrowserLogicalWindow->mViewportController.SetAnchorBottomMode(Core::EViewportAnchoredMode::eRelative);
    mAssetBrowserLogicalWindow->mViewportController.SetAnchorBottomRelValue(0.0f);
    // mAssetBrowserLogicalWindow->mViewportController.SetAnchorBottomPixelValue(300.0f);

    mDefaultEditWorkSpace->AddLogicalWindow(mAssetBrowserLogicalWindow.get());
    //    mSuperFrameController->AddLogicalWindow(mAssetBrowserLogicalWindow.get());
}

void Quad::EditorDirector::InitGlobalOverlayWindow()
{
    mGlobalOverlayLogicalWindow = std::make_unique<Core::LogicalWindow>();

    //  UI::UIManager *uiManager = UI::UIManager::GetInstance();
    UI::UICanvasID canvasID = mUIManager->CreateCanvas("GlobalOverlayCanvas", UI::ECanvasSizeMode::eFixSize);

    UI::UICanvas *canvas = mUIManager->GetCanvas(canvasID);

    mGlobalOverlayLogicalWindow->SetActiveCanvas(canvas);

    GlobalOverlayManager::GetInstance()->Initialize(canvas, mGlobalOverlayLogicalWindow.get());

    Core::WindowRenderConfig windowRenderConfig;
    windowRenderConfig.bIsOverlay = true;
    windowRenderConfig.bClearRenderTarget = false;
    mGlobalOverlayLogicalWindow->SetRenderConfig(windowRenderConfig);
    mGlobalOverlayLogicalWindow->mViewportController.SetViewportMode(Core::EViewportMode::eAnchored);

    mGlobalOverlayLogicalWindow->mViewportController.SetAnchorLeftState(true);
    mGlobalOverlayLogicalWindow->mViewportController.SetAnchorLeftMode(Core::EViewportAnchoredMode::eRelative);
    mGlobalOverlayLogicalWindow->mViewportController.SetAnchorLeftRelValue(0.0f);

    mGlobalOverlayLogicalWindow->mViewportController.SetAnchorRightState(true);
    mGlobalOverlayLogicalWindow->mViewportController.SetAnchorRightMode(Core::EViewportAnchoredMode::eRelative);
    mGlobalOverlayLogicalWindow->mViewportController.SetAnchorRightRelValue(0.0);

    mGlobalOverlayLogicalWindow->mViewportController.SetAnchorTopState(true);
    mGlobalOverlayLogicalWindow->mViewportController.SetAnchorTopMode(Core::EViewportAnchoredMode::eRelative);
    mGlobalOverlayLogicalWindow->mViewportController.SetAnchorTopRelValue(0.0f);

    mGlobalOverlayLogicalWindow->mViewportController.SetAnchorBottomState(true);
    mGlobalOverlayLogicalWindow->mViewportController.SetAnchorBottomMode(Core::EViewportAnchoredMode::eRelative);
    mGlobalOverlayLogicalWindow->mViewportController.SetAnchorBottomRelValue(0.0f);
    //    mSuperFrameController->AddLogicalWindow(mGlobalOverlayLogicalWindow.get());

    mDefaultEditWorkSpace->AddLogicalWindow(mGlobalOverlayLogicalWindow.get());
    mDefaultEditWorkSpace->SetGlobalOverlayWindow(mGlobalOverlayLogicalWindow.get());
    // mSuperFrameController->SetGlobalOverlayWindow(mGlobalOverlayLogicalWindow.get());
}

void Quad::EditorDirector::InitPropertyWindow()
{
    mPropertyLogicalWindow = std::make_unique<Core::LogicalWindow>();

    //  UI::UIManager *uiManager = UI::UIManager::GetInstance();

    //// Canvas1 (default)
    UI::UICanvasID canvasID = mUIManager->CreateCanvas("DefaultCanvas", UI::ECanvasSizeMode::eFixSize);

    UI::UICanvas *canvas = mUIManager->GetCanvas(canvasID);
    ObjectHierarchyPanel *ohpanel = canvas->CreateUIElement<ObjectHierarchyPanel>("asds");
    ohpanel->Initialize(EditorSelectionManager::GetInstance());

    ohpanel->SetPositionLocal(0, 0);
    ohpanel->SetSize(700, 500);

    ohpanel->SetScrollPanelSize(200, 300);

    /* PropertyPanel *propertyPanel = canvas->CreateUIElement<PropertyPanel>("PropertyPanel");
     propertyPanel->Initialize(EditorSelectionManager::GetInstance());
     propertyPanel->SetSize(700, 800);
     propertyPanel->SetPositionLocal(0, 500);
     propertyPanel->SetBackgrounColor(0.6f, 0.6f, 0.6f);
     propertyPanel->SetLayout(EUIScrollLayout::eVertical);*/

    auto editorSelectionManager = EditorSelectionManager::GetInstance();
    editorSelectionManager->mOnSelectedObjectCallbackSystem.Register(ohpanel, &ObjectHierarchyPanel::OnSelectedObject);
    // editorSelectionManager->mOnSelectedObjectCallbackSystem.Register(propertyPanel,
    // &PropertyPanel::OnSelectedObject);
    // editorSelectionManager->mOnSelectedComponentCallbackSystem.Register(propertyPanel,
    //                                                                    &PropertyPanel::OnSelectedComponent);

    DefaultPropertyInspector *defaultPropertyInspector = DefaultPropertyInspector::GetInstance();
    defaultPropertyInspector->Initialize(canvas);
    // defaultPropertyInspector->BeginUI();

    // MapSettingPanel

    MapSettingUIController *mapSettingUIController = MapSettingUIController::GetInstance();
    mapSettingUIController->Initialize(canvas, 700, 800, {0, 500});
    // mapSettingUIController->BeginUI();

    DefaultEditorInspectorManager *defaultEditorInspectorManager = DefaultEditorInspectorManager::GetInstance();

    defaultEditorInspectorManager->RegisterInspector(EDefaultEditorInspectorType::eProprety, defaultPropertyInspector);
    defaultEditorInspectorManager->RegisterInspector(EDefaultEditorInspectorType::eMapSetting, mapSettingUIController);

    // defaultEditorInspectorManager->ActivateInsepctor(EDefaultEditorInspectorType::eProprety);

    mPropertyLogicalWindow->SetActiveCanvas(canvas);

    mPropertyLogicalWindow->mViewportController.SetViewportMode(Core::EViewportMode::eAnchored);

    mPropertyLogicalWindow->mViewportController.SetAnchorLeftState(true);
    mPropertyLogicalWindow->mViewportController.SetAnchorLeftMode(Core::EViewportAnchoredMode::eRelative);
    mPropertyLogicalWindow->mViewportController.SetAnchorLeftRelValue(0.7f);

    mPropertyLogicalWindow->mViewportController.SetAnchorRightState(true);
    mPropertyLogicalWindow->mViewportController.SetAnchorRightMode(Core::EViewportAnchoredMode::eRelative);
    mPropertyLogicalWindow->mViewportController.SetAnchorRightRelValue(0.0f);

    mPropertyLogicalWindow->mViewportController.SetAnchorTopState(true);
    mPropertyLogicalWindow->mViewportController.SetAnchorTopMode(Core::EViewportAnchoredMode::ePixel);
    //  mSubLogicalWindow.mViewportController.SetAnchorTopRelValue(200.0f);
    mPropertyLogicalWindow->mViewportController.SetAnchorTopPixelValue(200.0f);

    mPropertyLogicalWindow->mViewportController.SetAnchorBottomState(true);
    mPropertyLogicalWindow->mViewportController.SetAnchorBottomMode(Core::EViewportAnchoredMode::eRelative);
    mPropertyLogicalWindow->mViewportController.SetAnchorBottomRelValue(0.0f);

    mPropertyLogicalWindow->SetBackBufferClearColor(0.3, 0.3f, 0.3f, 1.0f);

    mDefaultEditWorkSpace->AddLogicalWindow(mPropertyLogicalWindow.get());
}

void Quad::EditorDirector::InitEditorTaskManagerList()
{

    ClassGenerationManager *classGenerationManager = ClassGenerationManager::GetInstance();
    classGenerationManager->Initialize(mGlobalOverlayLogicalWindow->GetActiveCanvas());
    mEditorTaskManagerList.push_back(classGenerationManager);

    EditorAssetImporterManager *editorAssetImporterManager = EditorAssetImporterManager::GetInstance();
    editorAssetImporterManager->Initialize(mGlobalOverlayLogicalWindow->GetActiveCanvas());
    mEditorTaskManagerList.push_back(editorAssetImporterManager);
}

Core::LogicalWindow *Quad::EditorDirector::GetMainWindow() const
{
    return mMainSceneLogicalWindow.get();
}

void Quad::EditorDirector::RegisterAssetFactory()
{
    CoreAsset::AssetFactoryManager *assetFactoryManager = CoreAsset::AssetFactoryManager::GetInstance();
    assetFactoryManager->RegisterAssetFactory(CoreAsset::EAssetType::eTexture,
                                              CoreAsset::TextureFactory::GetInstance());
    assetFactoryManager->RegisterAssetFactory(CoreAsset::EAssetType::eMaterial,
                                              CoreAsset::MaterialFactory::GetInstance());
    assetFactoryManager->RegisterAssetFactory(CoreAsset::EAssetType::eStaticMesh,
                                              CoreAsset::MeshFactory::GetInstance());

    assetFactoryManager->RegisterAssetFactory(CoreAsset::EAssetType::eSkinningMesh,
                                              CoreAsset::MeshFactory::GetInstance());

    assetFactoryManager->RegisterAssetFactory(CoreAsset::EAssetType::eMap, Core::MapFactory::GetInstance());

    assetFactoryManager->RegisterAssetFactory(CoreAsset::EAssetType::eFont, CoreAsset::FontFactory::GetInstance());

    assetFactoryManager->RegisterAssetFactory(CoreAsset::EAssetType::ePrefab, PrefabFactory::GetInstance());
}

void Quad::EditorDirector::RegisterAssetLoader()
{ // asset loader  register
    CoreAsset::AssetIOManager *assetIOManager = CoreAsset::AssetIOManager::GetInstance();

    assetIOManager->RegisterAssetLoader(CoreAsset::EAssetType::eTexture, CoreAsset::TextureLoader::GetInstance());
    assetIOManager->RegisterAssetLoader(CoreAsset::EAssetType::eMaterial, CoreAsset::MaterialLoader::GetInstance());
    assetIOManager->RegisterAssetLoader(CoreAsset::EAssetType::eStaticMesh, CoreAsset::MeshLoader::GetInstance());
    assetIOManager->RegisterAssetLoader(CoreAsset::EAssetType::eSkinningMesh, CoreAsset::MeshLoader::GetInstance());
    assetIOManager->RegisterAssetLoader(CoreAsset::EAssetType::eMap, Core::MapLoader::GetInstance());
    assetIOManager->RegisterAssetLoader(CoreAsset::EAssetType::ePrefab, PrefabLoader::GetInstance());
}

void Quad::EditorDirector::RegisterAssetStorer()
{

    CoreAsset::AssetIOManager *assetIOManager = CoreAsset::AssetIOManager::GetInstance();

    // asset  storer register
    assetIOManager->RegisterAssetStorer(CoreAsset::EAssetType::eTexture, CoreAsset::TextureStorer::GetInstance());
    assetIOManager->RegisterAssetStorer(CoreAsset::EAssetType::eMaterial, CoreAsset::MaterialStorer::GetInstance());
    assetIOManager->RegisterAssetStorer(CoreAsset::EAssetType::eStaticMesh, CoreAsset::MeshStorer::GetInstance());
    assetIOManager->RegisterAssetStorer(CoreAsset::EAssetType::eSkinningMesh, CoreAsset::MeshStorer::GetInstance());
    assetIOManager->RegisterAssetStorer(CoreAsset::EAssetType::eMap, Core::MapStorer::GetInstance());
    assetIOManager->RegisterAssetStorer(CoreAsset::EAssetType::ePrefab, Core::PrefabStorer::GetInstance());
}

void Quad::EditorDirector::SwitchFrameWindow() {}

void Quad::EditorDirector::SwitchCommonEditWindow() {}

void Quad::EditorDirector::InitSystems()
{
    CHECK(mAssetManager != nullptr);
    CHECK(mUIManager != nullptr);
    CHECK(mRenderSystem != nullptr);
    CHECK(mGpuResourceManager != nullptr);

    // asset manager init
    // CoreAsset::AssetManager *assetManager = CoreAsset::AssetManager::GetInstance();

    // asset factory  register
    CoreAsset::AssetFactoryManager *assetFactoryManager = CoreAsset::AssetFactoryManager::GetInstance();
    CoreAsset::AssetImporterManager *assetImporterManager = CoreAsset::AssetImporterManager::GetInstance();
    CoreAsset::AssetIOManager *assetIOManager = CoreAsset::AssetIOManager::GetInstance();

    RegisterAssetFactory();
    RegisterAssetLoader();
    RegisterAssetStorer();

    // asset manager init

    mAssetManager->Initialize(assetFactoryManager, assetIOManager, assetImporterManager,
                              EditorConfig::GetInstance()->GetEditorAssetPath());

    ProjectConfig *projectConfig = ProjectConfig::GetInstance();
    mAssetManager->SetAssetRawDataPath(projectConfig->GetProjectRawAssetPath());

    // import module test
    // editor asset importer module
    EditorAssetImporterModule *editorAssetImporterModule = EditorAssetImporterModule::GetInstance();
    editorAssetImporterModule->Initialize();

    //
    //
    //

    EditorConfig *editorConfig = EditorConfig::GetInstance();
    const std::filesystem::path &editorRootPath = editorConfig->GetEditorRootPath();

    QuadLog::Logger *log = QuadLog::Logger::GetInstance();

    // FileSystem
    mPhysicalFileSystem = QuadPF::PhysicalFileSystem::GetInstance();
    mLogicalFileSystem = std::make_unique<QuadLF::LogicalFileSystem>(mPhysicalFileSystem);
    mLogicalFileSystem->Initialize(projectConfig->GetProjectPath(), editorConfig->GetEditorAssetPath());

    // AssetMeta
    mAssetMetaDataManager = CoreAsset::AssetMetaDataManager::GetInstance();

    // UI
    mUIMaterialManager = std::make_unique<CoreAsset::UIMaterialManager>(Render::IMaterialManager::GetInstance());

    mUIRenderItemBuilder = std::make_unique<Render::UIRenderItemBuilder>(mRenderSystem, mUIManager, mGpuResourceManager,
                                                                         Render::AssetResolver::GetInstance());

    //.shader.buffer 파일을 읽어서 gpuBuffer를 gpuBufferContextSystem에 등록한다.
    GRM::GpuBufferContextSystem *gpuBufferContextSystem = GRM::GpuBufferContextSystem::GetInstance();

    gpuBufferContextSystem->LoadShaderBufferFile(editorRootPath / "Shader/shaderbuffer.shader.buffer");

    mGpuSamplerSystem = std::make_unique<GRM::GpuSamplerSystem>(mGpuResourceManager);
    //   mGpuSamplerSystem->LoadShaderSamplerFile(editorRootPath + "/Shader/Sampler.sampler");

    // ShaderImporter
    // 현재 필요없는 상황
    mEditorShaderImporter = std::make_unique<EditorShaderImporter>(Render::IMaterialManager::GetInstance());
    EditorShaderImporter *editorShaderImporter = EditorShaderImporter::GetInstance();

    // Asset Resolver
    Render::AssetResolver *assetResolver = Render::AssetResolver::GetInstance();
    assetResolver->Initialize(mAssetManager, mGpuResourceManager);

    // TextureImporter
    mTextureImporter = std::make_unique<EditorTextureImporter>(Import::TextureImporter::GetInstance(),
                                                               CoreAsset::TextureManager::GetInstance(),
                                                               CoreAsset::AssetMetaDataManager::GetInstance());

    // ObjectRenderItemBuilder(IRenderProxyManager)
    mObjectRenderItemBuilder = std::make_unique<Render::ObjectRenderItemBuilder>(mRenderSystem, mGpuResourceManager,
                                                                                 Render::AssetResolver::GetInstance());

    Core::IRenderProxyManager::SetRenderProxyManager(mObjectRenderItemBuilder.get());

    EditorAssetManager *editorAssetManager = EditorAssetManager::GetInstance();
    editorAssetManager->Initialize(mAssetManager, QuadLF::LogicalFileSystem::GetInstance(),
                                   CoreAsset::AssetMetaDataManager::GetInstance());

    AsyncThreadPool::GetInstance();
}

void Quad::EditorDirector::UpdateEditorTaskManagers()
{

    for (auto taskManager : mEditorTaskManagerList)
    {

        if (taskManager->GetActiveState())
        {
            taskManager->Update();
        }
    }
}

void Quad::EditorDirector::SetPlayModeState(bool state) {}

bool Quad::EditorDirector::GetPlayModeState()
{
    auto instance = GetInstance();
    return true;
    // return instance->mPlayModeState;
}
