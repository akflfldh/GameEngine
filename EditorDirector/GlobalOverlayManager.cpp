#include "GlobalOverlayManager.h"
#include <ClassListUIScrollPanel.h>
#include <Core/CameraComponent.h>
#include <Core/Entity.h>
#include <Core/GlobalAppHelper.h>
#include <Core/LightObject.h>
#include <Core/LogicalWindow.h>
#include <Core/Map.h>
#include <Core/PlayerStart.h>
#include <Core/ProjectConfig.h>
#include <Core/StaticMeshComponent.h>
#include <Core/SuperController.h>
#include <Core/World.h>
#include <CoreAsset/GlobalAssetRegistrySystem.h>
#include <CoreAsset/assetManager.h>
#include <CoreBase/AsyncThreadPool.h>
#include <EditorDirector/ClassGenerationManager.h>
#include <EditorDirector/EditorAssetImporterManager.h>
#include <EditorDirector/EditorAssetImporterModule.h>
#include <EditorDirector/EditorDirector.h>
#include <EditorDirector/EditorProjectManager.h>
#include <EditorDirector/EditorSceneManager.h>
#include <EditorDirector/EditorUtility.h>
#include <EditorDirector/MapPlaySettingPanel.h>
#include <EditorDirector/PrefabGenerationManager.h>

#include <DefaultEditorInspectorManager.h>
#include <EditorDirector/TaskUIController.h>
#include <EditorDirector/UIDropTargetComponent.h>
#include <EditorDirector/UIEditorDebugHUD.h>
#include <EditorDirector/UIScrollBox.h>
#include <EditorSceneManager.h>
#include <IInspector.h>
#include <InputSystem/InputSystem.h>
#include <LogicalFileSystem/LogicalFileSystem.h>
#include <Prefab.h>
#include <ProjectGenerator.h>
#include <ReflectSystem/ReflectionClassInfo.h>
#include <UIAssetBrowser.h>
#include <UiSystem/UIButton.h>
#include <UiSystem/UIButtonComponent.h>
#include <UiSystem/UICanvas.h>
#include <UiSystem/UIEditBox.h>
#include <UiSystem/UIImage.h>
#include <UiSystem/UIImageComponent.h>
#include <UiSystem/UIMovableComponent.h>
#include <UiSystem/UIText.h>
#include <UiSystem/UITextButton.h>
#include <UiSystem/UITextComponent.h>
#include <UiSystem/UIVerticalLayoutComponent.h>
#include <chrono>
#include <thread>

GlobalOverlayManager *GlobalOverlayManager::GetInstance()
{
    static GlobalOverlayManager instance;
    return &instance;
}

GlobalOverlayManager::GlobalOverlayManager()
    : mMessageBox(nullptr), mMessageBoxTextCom(nullptr), mIsOpeningMenuPanel(false), mCurrentMenuContextPanel(nullptr),
      mSaveMapPanel(nullptr), mToSaveMap(nullptr), mDragDropImage(nullptr), mGenerationObjectClassPanel(nullptr),
      mIsDragDrop(false), mScenePlayState(EScenePlayState::eNone), mWorkingTaskContext(std::make_shared<TaskContext>())
{
}

GlobalOverlayManager::~GlobalOverlayManager() {}

void GlobalOverlayManager::Initialize(UI::UICanvas *overlayCanvas, Core::LogicalWindow *overlayWindow)
{

    mOverlayCanvas = overlayCanvas;
    mOverlayWindow = overlayWindow;

    // 미리 panel들을 생성.
    CreateToobar();
    // assetBrowser
    auto uiAssetBrowser = overlayCanvas->CreateUIElement<UIAssetBrowser>("UIAssetBrowser");
    uiAssetBrowser->SetWidth(3000.0);

    uiAssetBrowser->SetHorizontalPivotSide(UI::EUIPosPivotHorizontal::eLeft);
    uiAssetBrowser->SetHorizontalPivotOffset(0.0f);

    uiAssetBrowser->SetVerticalPivotSide(UI::EUIPosPivotVertical::eBottom);
    uiAssetBrowser->SetVerticalPivotOffset(0.0f);

    uiAssetBrowser->SetInitFolder(QuadLF::LogicalFileSystem::GetInstance()->GetAssetFolder());

    CreateMessageBox();
    CreateSaveMapPanel();
    CreateGenerationObjectClassPanel();
    CreateGenerationPrefabPanel();
    // Debug Panel

    mDebugHUD = overlayCanvas->CreateUIElement<UIEditorDebugHUD>("DebugHUD");
    mDebugHUD->SetPositionLocal(0, 0);
    mDebugHUD->SetDepthValue(1); // 가장위에

    mMainWindow = Quad::EditorDirector::GetInstance()->GetMainSceneWindow();

    //  mainWindow->mOnMouseEnterCallbackSystem.Register([this]() { OnDragDropMouseEnterMainWindow(); });
    //    mainWindow->mOnMouseLeaveCallbackSystem.Register([this]() { OnDragDropMouseLeaveMainWindow(); });
}

void GlobalOverlayManager::Update(float deltaTime)
{

    if (mIsDragDrop)
    {
        const auto &mouseContext = Quad::InputSystem::GetInstance()->GetMouseContext();

        mouseContext.mClientPosX;
        mouseContext.mClientPosY;

        // 현재 마우스가 위치한 논리적윈도우를 얻는다.해당 논리적윈도우의 UICanvas에서의 Hover UI를 찾는다.

        auto ownerController = mOverlayWindow->GetOwnerController();
        Core::WorkSpace *workSpace = ownerController->GetWorkSpace();
        Core::LogicalWindow *currMouseActiveLogicalWindow =
            workSpace->FindLogicalWindowAtClientPos(mouseContext.mClientPosX, mouseContext.mClientPosY);

        if (currMouseActiveLogicalWindow)
        {
            UI::UICanvas *currMouseActiveUICanvas = currMouseActiveLogicalWindow->GetActiveCanvas();

            if (currMouseActiveUICanvas)
            {

                glm::vec2 worldPos =
                    currMouseActiveLogicalWindow->ConverToWorldPos(mouseContext.mClientPosX, mouseContext.mClientPosY);

                UI::UIElement *element = currMouseActiveUICanvas->GetHittedElement(worldPos.x, worldPos.y);

                if (element)
                {

                    UIDropTargetComponent *dropTargetCom = nullptr;
                    if (element->GetComponents<UIDropTargetComponent>(&dropTargetCom, 1) == 1)
                    {

                        if (mCurrentHoverDropTarget != element)
                        {
                            if (mCurrentHoverDropTarget)
                            {
                                UIDropTargetComponent *preDropTargetCom = nullptr;
                                mCurrentHoverDropTarget->GetComponents<UIDropTargetComponent>(&preDropTargetCom, 1);

                                if (preDropTargetCom)
                                {
                                    preDropTargetCom->OnReleaseHover();
                                }
                            }
                        }

                        mCurrentHoverDropTarget = element;
                        dropTargetCom->OnHover(worldPos.x, worldPos.y);
                    }
                }
            }
        }

        glm::vec2 worldPos = mOverlayWindow->ConverToWorldPos(mouseContext.mClientPosX, mouseContext.mClientPosY);
        mDragDropImage->SetPositionLocal(worldPos);

        auto inputSystem = Quad::InputSystem::GetInstance();
        if (inputSystem->GetMouseContext().bLButtonUpThisFrame)
        {
            ClearDragDrop();
        }

        UpdateDragDropMainWindowState(currMouseActiveLogicalWindow);
        UpdatePrefabPosIfDragDrop(mouseContext.mClientPosX, mouseContext.mClientPosY);

        /*
         마우스가 main window에 있는가?
         처음 진입인가?  image -> false   ,  해당 월드에 default object add
         마우스 레이캐스팅  , 해당위치에 배치

         //아니다
         //main window에서 빠진것인가?
         // image- > true ; 해당 월드에서 default object remove




        */
    }
    // else if (mIsWorking)
    //{
    //     // 작업중이다.
    //     // 풀링방식으로 메세지 업데이트
    //     if (mCurrentTaskUIController)
    //     {
    //         mCurrentTaskUIController->Update();
    //     }
    // }
}

void GlobalOverlayManager::ChangeToDefaultEdit()
{

    mToolbar->SetActiveFlag(true);
}

void GlobalOverlayManager::ChangeToPrefabEdit()
{

    mToolbar->SetActiveFlag(false);
}

void GlobalOverlayManager::ChangeToMaterialEdit()
{

    mToolbar->SetActiveFlag(false);
}

void GlobalOverlayManager::ShowMessageBox(const std::string &str)
{

    mMessageBox->SetActiveFlag(true);
    mMessageBoxTextCom->SetText(str);
}

void GlobalOverlayManager::CloseMessageBox()
{
    mMessageBox->SetActiveFlag(false);
}

void GlobalOverlayManager::CreateMessageBox()
{
    mMessageBox = mOverlayCanvas->CreateUIElement<UI::UIImage>("MessageBox");
    mMessageBox->SetActiveFlag(false);
    mMessageBox->SetColor(0.4f, 0.4f, 0.4f);
    mMessageBox->SetSize(300, 300);
    mMessageBox->SetPositionLocal(400, 900);

    mMessageBoxTextCom = mMessageBox->CreateUIComponent<UI::UITextComponent>("TextCom");
    mMessageBoxTextCom->SetOverflowMode(UI::EUITextOverflowMode::eWordWrap);
    mMessageBoxTextCom->SetFontSize(30.0f);
    mMessageBoxTextCom->SetPaddingLeft(30.0f);
    mMessageBoxTextCom->SetPaddingTop(30.0f);

    auto exitButton = mMessageBox->CreateChildUIElement<UI::UIButton>("ExitButton");

    exitButton->SetSize(40, 40);
    exitButton->SetPositionLocal(mMessageBox->mTransform.GetSize().x - exitButton->mTransform.GetSize().x, 0);
    exitButton->mUIImageComponent->SetColor(1.0f, 0.0f, 0.0f);
    exitButton->mUIButtonComponent->mButtonClickCallbackSystem.Register([this](float, float) { CloseMessageBox(); });
    exitButton->mUIImageComponent->UseTexture();
    exitButton->mUIImageComponent->SetTexture("Engine/Exit");

    exitButton->mHoverCallbackSystem.Register([button = exitButton](float, float)
                                              { button->mUIImageComponent->SetTexture("Engine/ExitHover"); });

    exitButton->mReleaseHoverCallbackSystem.Register([button = exitButton]()
                                                     { button->mUIImageComponent->SetTexture("Engine/Exit"); });
}

void GlobalOverlayManager::CreateToobar()
{
    // y= 0에서 높이 200을 가진다.

    mToolbar = mOverlayCanvas->CreateUIElement<UI::UIImage>("Toolbar");
    mToolbar->SetSize(3000, 200);
    mToolbar->SetColor(0.3f, 0.3f, 0.3f);

    CreateProjectBar();
    CreateSceneBar();
    CreatePlayBar();

    CreateDragDropImage();
}

void GlobalOverlayManager::CreateProjectBar()
{

    auto projectBar = mToolbar->CreateChildUIElement<UI::UIElement>("projectBar");
    projectBar->SetSize(mToolbar->mTransform.GetSize().r, 40);

    struct MenuData
    {
        std::string name;
        std::string text;
        float width;
    };

    std::vector<MenuData> menus = {{"File", "파일", 100.0f}, {"Edit", "편집", 100.0f},
                                   {"Asset", "에셋", 100.0}, {"Object", "오브젝트", 150.0f},
                                   {"Window", "창", 80.0f},  {"Help", "도움말", 100.0f}};

    std::unordered_map<std::string, UI::UITextButton *> menuMap;

    float currentPosX = 0.0f;
    for (const auto &menu : menus)
    {
        auto textButton =
            AddMenuButton(projectBar, menu.text, menu.name, menu.width, UI::UIColor{0.5f, 0.5f, 0.5f, 1.0f});
        textButton->SetPositionLocal(currentPosX, 0);
        currentPosX += menu.width;
        menuMap[menu.name] = textButton;
    }
    CreateFileContextPanel(menuMap["File"]);
    CreateEditContextPanel(menuMap["Edit"]);
    CreateAssetContextPanel(menuMap["Asset"]);
    CreateObjectContextPanel(menuMap["Object"]);
}

void GlobalOverlayManager::CreateSceneBar()
{

    auto sceneBar = mToolbar->CreateChildUIElement<UI::UIElement>("SceneBar");

    sceneBar->SetSize(mToolbar->mTransform.GetSize().r, 40);

    sceneBar->SetPositionLocal(0, 40);

    std::vector<MenuData> menus = {{"GameObject", "게임오브젝트", 200.0f}, {"SceneSetting", "씬 설정", 200.0f}};

    std::unordered_map<std::string, UI::UITextButton *> menuMap;

    float currentPosX = 0.0f;
    for (const auto &menu : menus)
    {
        auto textButton =
            AddMenuButton(sceneBar, menu.text, menu.name, menu.width, UI::UIColor{0.5f, 0.5f, 0.5f, 1.0f});
        textButton->SetPositionLocal(currentPosX, 0);
        currentPosX += menu.width;
        menuMap[menu.name] = textButton;
    }

    CreateGameObjectContextPanel(menuMap["GameObject"]);
    CreateSceneSettingContextPanel(menuMap["SceneSetting"]);
}

void GlobalOverlayManager::CreatePlayBar()
{

    auto playBar = mToolbar->CreateChildUIElement<UI::UIImage>("PlayBar");

    playBar->SetColor({0.4F, 0.4F, 0.4F});
    playBar->SetSize(mToolbar->mTransform.GetSize().r, 120);

    playBar->SetPositionLocal(0, 80);

    // PlayButton
    auto playButton = playBar->CreateChildUIElement<UI::UIButton>("PlayButton");
    playButton->SetUseHoverImageColor(false);
    playButton->mUIImageComponent->UseTexture();
    playButton->mUIImageComponent->SetTexture("Engine/PlayStartState");
    playButton->SetSize(50, 50);

    playButton->mUIButtonComponent->mButtonClickCallbackSystem.Register(
        [this, playButton](float, float)
        {
            OnScenePlayButtonDown();

            EScenePlayState currState = GetScenePlayState();
            switch (currState)
            {
            case EScenePlayState::eNone:
            {
                playButton->mUIImageComponent->SetTexture("Engine/PlayStartState");
            }
            break;
            case EScenePlayState::ePlay:
            {
                playButton->mUIImageComponent->SetTexture("Engine/PlayingState");
            }
            break;
            case EScenePlayState::ePause:
            {
                playButton->mUIImageComponent->SetTexture("Engine/PlayPause");
            }
            break;
            }
        });

    // Pause
    auto pauseButton = playBar->CreateChildUIElement<UI::UIButton>("PlayButton");
    pauseButton->SetPositionLocal(50, 0);
    pauseButton->SetUseHoverImageColor(false);
    pauseButton->mUIImageComponent->UseTexture();
    pauseButton->mUIImageComponent->SetTexture("Engine/PlayEnd");
    pauseButton->SetSize(50, 50);
    pauseButton->mUIButtonComponent->mButtonClickCallbackSystem.Register(
        [this, playButton, pauseButton](float, float)
        {
            OnScenePlayEndButtonDown();
            playButton->mUIImageComponent->SetTexture("Engine/PlayStartState");
        });
}

void GlobalOverlayManager::CreatePrefabEditPanel() {}

UI::UITextButton *GlobalOverlayManager::AddMenuButton(UI::UIElement *parent, const std::string &name,
                                                      const std::string &objectName, float width,
                                                      const UI::UIColor &baseColor)
{

    UI::UIColor hoverColor = {0.7f, 0.7f, 0.7f, 1.0f};
    UI::UIColor hoverReleaseColor = baseColor;

    auto menu = parent->CreateChildUIElement<UI::UITextButton>(objectName.c_str());
    menu->SetSize(width, 40);
    menu->mUIImageComponent->SetColor(baseColor);
    menu->mTextComponent->SetText(name);
    menu->mTextComponent->SetFontSize(25.0f);
    menu->SetUseHoverImageColor(true);
    menu->mHoverImageColor = hoverColor;
    menu->mReleaseHoverImageColor = hoverReleaseColor;

    return menu;
}

UI::UIImage *GlobalOverlayManager::CreateBaseContextPanel(UI::UITextButton *button, const std::string &objectName,
                                                          float width, int dir)
{
    auto panel = mOverlayCanvas->CreateUIElement<UI::UIImage>(objectName.c_str());
    panel->SetColor(0.4f, 0.4f, 0.4f);
    panel->SetSize(width, 40);
    glm::vec2 buttonWorldPos = button->mTransform.GetWorldPosition();

    if (dir == 0)
    {
        panel->SetPositionLocal(buttonWorldPos.x, buttonWorldPos.g + button->mTransform.GetSize().g);
    }
    else
    {
        panel->SetPositionLocal(buttonWorldPos.x + button->mTransform.GetSize().r, buttonWorldPos.g);
    }

    button->mUIButtonComponent->mButtonClickCallbackSystem.Register(
        [panel, this, button](float, float)
        {
            if (CheckCurrentOpenedMenu(panel))
            {
                CloseCurrentContextMenu(panel, button);
                return;
            }

            ShowContextMenu(panel, button);
        });

    panel->SetDepthValue(1);

    return panel;
}

void GlobalOverlayManager::CreateFileContextPanel(UI::UITextButton *fileButton)
{
    auto panel = CreateBaseContextPanel(fileButton, "FilePanel", 300);
    auto verticalLayoutCom = panel->CreateUIComponent<UI::UIVerticalLayoutComponent>("VerticalLayoutCom");
    panel->SetActiveFlag(false);

    std::vector<MenuData> menus = {{"NewMapButton", "새 맵 생성하기", 300.0f},
                                   {"OpenMapButton", "맵 불러오기", 300.0f},
                                   {"SaveMapButton", "맵 저장", 300.0},
                                   {"SaveMapAs...", "다른이름으로 맵 저장", 300.0f},
                                   {"OpenProjct", "새 프로젝트 열기", 300.0f},
                                   {"SaveProject", "프로젝트 저장", 300.0f},
                                   {"Exit", "에디터 종료", 300.0f}};

    std::unordered_map<std::string, UI::UITextButton *> menuMap;

    for (const auto &menu : menus)
    {
        auto button = AddMenuButton(panel, menu.text, menu.name, menu.width, UI::UIColor{0.4f, 0.4f, 0.4f, 1.0f});
        menuMap[menu.name] = button;
    }

    verticalLayoutCom->CalculateLayout();

    menuMap["SaveProject"]->mUIButtonComponent->mButtonClickCallbackSystem.Register(
        [this](float, float)
        {
            CloseCurrentContextMenuAll();

            // 프로젝트 저장 호출
            Quad::EditorProjectManager::GetInstance()->SaveProject();
        });
}

void GlobalOverlayManager::CreateEditContextPanel(UI::UITextButton *editButton)
{

    auto panel = CreateBaseContextPanel(editButton, "EditPanel", 300);
    auto verticalLayoutCom = panel->CreateUIComponent<UI::UIVerticalLayoutComponent>("VerticalLayoutCom");
    panel->SetActiveFlag(false);

    std::vector<MenuData> menus = {{"ProjectSettingButton", "프로젝트 설정", 300.0f},
                                   {"EditorSettingButton", "에디터 설정", 300.0f}};

    std::unordered_map<std::string, UI::UITextButton *> menuMap;

    for (const auto &menu : menus)
    {
        auto button = AddMenuButton(panel, menu.text, menu.name, menu.width, {0.4f, 0.4f, 0.4f, 1.0f});
        menuMap[menu.name] = button;
    }
    verticalLayoutCom->CalculateLayout();
}

void GlobalOverlayManager::CreateAssetContextPanel(UI::UITextButton *ownerButton)
{

    auto panel = CreateBaseContextPanel(ownerButton, "AssetPanel", 300);
    auto verticalLayoutCom = panel->CreateUIComponent<UI::UIVerticalLayoutComponent>("VerticalLayoutCom");
    panel->SetActiveFlag(false);

    std::vector<MenuData> menus = {{"ImportAsset", "에셋 임포트", 300.0f}, {"CreateMaterial", "머터리얼 생성", 300.0f}};

    std::unordered_map<std::string, UI::UITextButton *> menuMap;

    for (const auto &menu : menus)
    {
        auto button = AddMenuButton(panel, menu.text, menu.name, menu.width, UI::UIColor{0.4f, 0.4f, 0.4f, 1.0f});
        menuMap[menu.name] = button;
    }
    verticalLayoutCom->CalculateLayout();

    menuMap["ImportAsset"]->mUIButtonComponent->mButtonClickCallbackSystem.Register(
        [this](float, float)
        {
            CloseCurrentContextMenuAll();
            StartImport();
        });
}

void GlobalOverlayManager::CreateObjectContextPanel(UI::UITextButton *editButton)
{

    auto panel = CreateBaseContextPanel(editButton, "ObjectPanel", 300);
    auto verticalLayoutCom = panel->CreateUIComponent<UI::UIVerticalLayoutComponent>("VerticalLayoutCom");
    panel->SetActiveFlag(false);

    std::vector<MenuData> menus = {
        {"GenerationClass", "클래스 생성", 300.0f}, {"Prefab", "프리팹 생성", 300.0f}, {"TEST", "TEST", 300.0f}};

    std::unordered_map<std::string, UI::UITextButton *> menuMap;

    for (const auto &menu : menus)
    {
        auto button = AddMenuButton(panel, menu.text, menu.name, menu.width, UI::UIColor{0.4f, 0.4f, 0.4f, 1.0f});
        menuMap[menu.name] = button;
    }
    verticalLayoutCom->CalculateLayout();

    menuMap["GenerationClass"]->mUIButtonComponent->mButtonClickCallbackSystem.Register(
        [this](float, float)
        {
            mGenerationObjectClassPanel->SetActiveFlag(true);
            CloseCurrentContextMenuAll();
        });

    menuMap["TEST"]->mUIButtonComponent->mButtonClickCallbackSystem.Register(
        [this](float, float)
        {
            SetCurrentTaskUIController(mClassGenerationTaskUIController.get());

            auto threadPool = AsyncThreadPool::GetInstance();
            threadPool->Submit(
                [this]()
                {
                    PrograssContext prograssContext(mWorkingTaskContext);

                    prograssContext.Report("진행중 1. ");

                    std::this_thread::sleep_for(std::chrono::seconds(1));

                    prograssContext.Report("진행중 1. . ");

                    std::this_thread::sleep_for(std::chrono::seconds(1));

                    prograssContext.Report("진행중 1. . . ");

                    std::this_thread::sleep_for(std::chrono::seconds(1));

                    prograssContext.Report("진행중 1. ");

                    std::this_thread::sleep_for(std::chrono::seconds(1));

                    prograssContext.Report("진행중 1. . ");

                    std::this_thread::sleep_for(std::chrono::seconds(1));

                    SetCurrentTaskUIController(nullptr);
                });

            CloseCurrentContextMenuAll();
        });

    menuMap["Prefab"]->mUIButtonComponent->mButtonClickCallbackSystem.Register(
        [this](float, float)
        {
            // GenerationPrefabPanel->SetActiveFlag(true);
            mGenerationPrefabPanel->SetActiveFlag(true);

            CloseCurrentContextMenuAll();
        });
}

void GlobalOverlayManager::CreateGameObjectContextPanel(UI::UITextButton *ownerButton)
{

    auto panel = CreateBaseContextPanel(ownerButton, "GameObjectPanel", 300);
    auto verticalLayoutCom = panel->CreateUIComponent<UI::UIVerticalLayoutComponent>("VerticalLayoutCom");
    panel->SetActiveFlag(false);

    std::vector<MenuData> menus = {
        {"3DObject", "3D 오브젝트 ", 300.0f}, {"Light", "라이트", 300.0f}, {"PlayerStart", "플레이어 스타트", 300.0f}};

    std::unordered_map<std::string, UI::UITextButton *> menuMap;

    for (const auto &menu : menus)
    {
        auto button = AddMenuButton(panel, menu.text, menu.name, menu.width, UI::UIColor{0.4f, 0.4f, 0.4f, 1.0f});
        menuMap[menu.name] = button;
    }
    verticalLayoutCom->CalculateLayout();

    Create3DObjectContextPanel(menuMap["3DObject"]);
    CreateLightObjectContextPanel(menuMap["Light"]);
    menuMap["PlayerStart"]->mUIButtonComponent->mButtonClickCallbackSystem.Register(
        [this](float, float)
        {
            CloseCurrentContextMenuAll();

            CreatePlayerStartAtEditorView();
        });
}

void GlobalOverlayManager::Create3DObjectContextPanel(UI::UITextButton *ownerButton)
{

    auto panel = CreateBaseContextPanel(ownerButton, "GameObjectPanel", 300, 1);
    auto verticalLayoutCom = panel->CreateUIComponent<UI::UIVerticalLayoutComponent>("VerticalLayoutCom");
    panel->SetActiveFlag(false);

    std::vector<MenuData> menus = {{"Cube", "큐브", 300.0f},
                                   {"Cylinder", "원기둥", 300.0f},
                                   {"Cone", "원뿔", 300.0f},
                                   {"Arrow", "화살표", 300.0f},
                                   {"Sphere", "구", 300.0f}};

    std::unordered_map<std::string, UI::UITextButton *> menuMap;

    for (const auto &menu : menus)
    {
        auto button = AddMenuButton(panel, menu.text, menu.name, menu.width, UI::UIColor{0.4f, 0.4f, 0.4f, 1.0f});
        menuMap[menu.name] = button;
    }
    verticalLayoutCom->CalculateLayout();

    menuMap["Cube"]->mUIButtonComponent->mButtonClickCallbackSystem.Register(
        [this](float, float)
        {
            CloseCurrentContextMenuAll();
            Create3DObjectToWorld(EDefault3DObjectType::eCube);
        });

    menuMap["Cylinder"]->mUIButtonComponent->mButtonClickCallbackSystem.Register(
        [this](float, float)
        {
            CloseCurrentContextMenuAll();
            Create3DObjectToWorld(EDefault3DObjectType::eCylinder);
        });

    menuMap["Cone"]->mUIButtonComponent->mButtonClickCallbackSystem.Register(
        [this](float, float)
        {
            CloseCurrentContextMenuAll();
            Create3DObjectToWorld(EDefault3DObjectType::eCone);
        });

    menuMap["Arrow"]->mUIButtonComponent->mButtonClickCallbackSystem.Register(
        [this](float, float)
        {
            CloseCurrentContextMenuAll();
            Create3DObjectToWorld(EDefault3DObjectType::eArrow);
        });

    menuMap["Sphere"]->mUIButtonComponent->mButtonClickCallbackSystem.Register(
        [this](float, float)
        {
            CloseCurrentContextMenuAll();
            Create3DObjectToWorld(EDefault3DObjectType::eSphere);
        });
}

void GlobalOverlayManager::CreateLightObjectContextPanel(UI::UITextButton *button)
{

    auto panel = CreateBaseContextPanel(button, "LightObjectPanel", 300, 1);
    auto verticalLayoutCom = panel->CreateUIComponent<UI::UIVerticalLayoutComponent>("VerticalLayoutCom");
    panel->SetActiveFlag(false);

    std::vector<MenuData> menus = {
        {"Directional Light", "평행광", 300.0f}, {"Point Light", "점광", 300.0f}, {"Spot Light", "점적광", 300.0f}};

    std::unordered_map<std::string, UI::UITextButton *> menuMap;

    for (const auto &menu : menus)
    {
        auto button = AddMenuButton(panel, menu.text, menu.name, menu.width, UI::UIColor{0.4f, 0.4f, 0.4f, 1.0f});
        menuMap[menu.name] = button;
    }
    verticalLayoutCom->CalculateLayout();

    menuMap["Directional Light"]->mUIButtonComponent->mButtonClickCallbackSystem.Register(
        [this](float, float)
        {
            CloseCurrentContextMenuAll();

            Quad::EditorSceneManager *sceneManager = Quad::EditorSceneManager::GetInstance();
            if (sceneManager)
            {
                auto world = sceneManager->GetUserWorld();
                if (world)
                {
                    Map *map = world->GetCurrentMap();
                    if (map)
                    {
                        Core::LightObject *object = map->CreateEntity<Core::LightObject>("Light");
                    }
                }
            }
            // Cube 생성

            // 엔티티
            // 정적 메시 컴포넌트 소유,
        });

    menuMap["Point Light"]->mUIButtonComponent->mButtonClickCallbackSystem.Register(
        [this](float, float)
        {
            CloseCurrentContextMenuAll();

            Quad::EditorSceneManager *sceneManager = Quad::EditorSceneManager::GetInstance();
            if (sceneManager)
            {
                auto world = sceneManager->GetUserWorld();
                if (world)
                {
                    Map *map = world->GetCurrentMap();
                    if (map)
                    {
                        Core::LightObject *object = map->CreateEntity<Core::LightObject>("Light");
                        object->SetLightType(Core::ELightType::ePoint);
                    }
                }
            }
            // Cube 생성

            // 엔티티
            // 정적 메시 컴포넌트 소유,
        });
}

void GlobalOverlayManager::CreateSceneSettingContextPanel(UI::UITextButton *settingButton)
{

    auto panel = CreateBaseContextPanel(settingButton, "SceneSettingPanel", 300);
    auto verticalLayoutCom = panel->CreateUIComponent<UI::UIVerticalLayoutComponent>("VerticalLayoutCom");
    panel->SetActiveFlag(false);

    std::vector<MenuData> menus = {{"DebugCollider", "디버그 콜라이더  ", 300.0f},
                                   {"MapSetting", "게임모드설정", 300.0f},
                                   {"MapRenderSetting", "맵 렌더 설정", 300.0f}};

    std::unordered_map<std::string, UI::UITextButton *> menuMap;

    for (const auto &menu : menus)
    {
        auto button = AddMenuButton(panel, menu.text, menu.name, menu.width, UI::UIColor{0.4f, 0.4f, 0.4f, 1.0f});
        menuMap[menu.name] = button;
    }
    verticalLayoutCom->CalculateLayout();

    menuMap["DebugCollider"]->mUIButtonComponent->mButtonClickCallbackSystem.Register(
        [this](float, float)
        {
            CloseCurrentContextMenuAll();
            auto projectManager = Quad::EditorProjectManager::GetInstance();

            if (projectManager)
            {
                projectManager->SetDrawDebugColliderFlag(!projectManager->GetDrawDebugColliderFlag());
            }
        });

    menuMap["MapSetting"]->mUIButtonComponent->mButtonClickCallbackSystem.Register(
        [this](float, float)
        {
            CloseCurrentContextMenuAll();
            OpenGameModeSettingPanel();
        });

    menuMap["MapRenderSetting"]->mUIButtonComponent->mButtonClickCallbackSystem.Register(
        [this](float, float)
        {
            CloseCurrentContextMenuAll();
            OpenMapRenderInspector();
        });
}

void GlobalOverlayManager::CreatePlayerStartAtEditorView()
{

    World *world = Quad::EditorSceneManager::GetInstance()->GetUserWorld();
    if (world == nullptr)
        return;

    Map *map = world->GetCurrentMap();
    if (map == nullptr)
        return;
    CameraComponent *camera = world->GetCurrentCameraCom();

    CoreMath::Vector3 spawnPos = {0.0f, 0.0f, 0.0f};

    if (camera)
    {
        spawnPos = camera->GetPositionWorld() + camera->GetForwardWorld() * 10.0f;
    }
    PlayerStart *playerStart = map->CreateEntity<PlayerStart>("PlayerStart");
    if (playerStart == nullptr)
        return;

    playerStart->SetPositionLocal(spawnPos);
}

void GlobalOverlayManager::CreateSaveMapPanel()
{

    auto saveMapPanel = mOverlayCanvas->CreateUIElement<UI::UIText>("SaveMapPanel");
    auto imageCom = saveMapPanel->CreateUIComponent<UI::UIImageComponent>("ImageCom");
    imageCom->SetColor(0.2f, 0.2f, 0.2f);

    // Yes, no  자식 ui버튼 두개 생성
    auto saveButton = saveMapPanel->CreateChildUIElement<UI::UITextButton>("SaveButton");
    auto saveCancelButton = saveMapPanel->CreateChildUIElement<UI::UITextButton>("SaveCancelButton");

    saveButton->mTextComponent->SetFontSize(20.0f);
    saveCancelButton->mTextComponent->SetFontSize(20.0f);

    saveMapPanel->SetSize(600, 400);
    saveButton->SetSize(100, 50);
    saveCancelButton->SetSize(100, 50);

    saveButton->SetPositionLocal(200, 300);
    saveCancelButton->SetPositionLocal(400, 300);

    saveButton->mTextComponent->SetText("저장");
    saveCancelButton->mTextComponent->SetText("취소");

    saveButton->mUIButtonComponent->mButtonClickCallbackSystem.Register(
        [this](float, float)
        {
            mSaveMapPanel->SetActiveFlag(false); // 버튼을 눌렀으니 판넬은 안보이게
            Quad::EditorProjectManager::GetInstance()->SaveMap(mToSaveMap);
            mToSaveMap = nullptr;

            if (mOnSaveMapDicisionCallback)
            {
                mOnSaveMapDicisionCallback();
                mOnSaveMapDicisionCallback = nullptr;
            }
        });

    saveCancelButton->mUIButtonComponent->mButtonClickCallbackSystem.Register(
        [this](float, float)
        {
            mSaveMapPanel->SetActiveFlag(false);
            mToSaveMap = nullptr;

            if (mOnSaveMapDicisionCallback)
            {
                mOnSaveMapDicisionCallback();
                mOnSaveMapDicisionCallback = nullptr;
            }
        });

    mSaveMapPanel = saveMapPanel;

    mSaveMapPanel->SetActiveFlag(false);
}

void GlobalOverlayManager::CreateDragDropImage()
{

    mDragDropImage = mOverlayCanvas->CreateUIElement<UI::UIImage>("DragDropImage");
    mDragDropImage->SetSize(50, 50);
    mDragDropImage->SetActiveFlag(false);
    mDragDropImage->SetOnlyVisible(true);
    mDragDropImage->SetColor(0.4f, 0.2f, 0.7f);
    mDragDropImage->SetDepthValue(0);

    //    mDragDropImage->CreateUIComponent<UI::UIMovableComponent>("MovableCom");
}

void GlobalOverlayManager::CreateGenerationObjectClassPanel()
{

    mGenerationObjectClassPanel = mOverlayCanvas->CreateUIElement<UI::UIImage>("GenerataionObjectClassPanel");
    mGenerationObjectClassPanel->SetSize(800, 700);
    mGenerationObjectClassPanel->SetActiveFlag(false);
    mGenerationObjectClassPanel->SetColor({0.3f, 0.3f, 0.3f});
    mGenerationObjectClassPanel->SetPositionLocal(200.0f, 200.0f);

    auto exitButton = mGenerationObjectClassPanel->CreateChildUIElement<UI::UIButton>("ExitButton");
    exitButton->SetSize(40, 40);
    exitButton->SetPositionLocal(
        mGenerationObjectClassPanel->mTransform.GetSize().x - exitButton->mTransform.GetSize().x, 0);
    exitButton->mUIImageComponent->UseTexture();
    exitButton->mUIImageComponent->SetTexture("Engine/Exit");
    exitButton->mUIButtonComponent->mButtonClickCallbackSystem.Register(
        [this](float, float) { mGenerationObjectClassPanel->SetActiveFlag(false); });

    float fontSize = 20.0f;

    auto classInputPanel = mGenerationObjectClassPanel->CreateChildUIElement<UI::UIImage>("ClassInputPanel");
    classInputPanel->SetSize(mGenerationObjectClassPanel->mTransform.GetSize().r, 200);
    classInputPanel->SetColor(1.0f, 0.3f, 0.3f);
    classInputPanel->SetPositionLocal(0, 500);

    auto parentClassNameTag = classInputPanel->CreateChildUIElement<UI::UIText>("ParentClassNameTag");
    parentClassNameTag->SetSize(200, 30);
    parentClassNameTag->SetPositionLocal(20, 10);
    parentClassNameTag->SetFontSize(fontSize);
    parentClassNameTag->SetText("부모 클래스");

    auto parentClassNameEditBox = classInputPanel->CreateChildUIElement<UI::UIEditBox>("ParentClassEditBox");
    parentClassNameEditBox->SetSize(500, 30);
    parentClassNameEditBox->SetFontSize(fontSize);
    parentClassNameEditBox->SetBackgroundColor(1, 1, 1);
    parentClassNameEditBox->SetTextColor(0, 0, 0);
    parentClassNameEditBox->SetPositionLocal(parentClassNameTag->mTransform.GetLocalPosition().x +
                                                 parentClassNameTag->mTransform.GetSize().x + 40.0f,
                                             parentClassNameTag->mTransform.GetLocalPosition().y);

    auto classNameTag = classInputPanel->CreateChildUIElement<UI::UIText>("classNameTag");
    classNameTag->SetSize(200, 30);
    classNameTag->SetPositionLocal(20, parentClassNameTag->mTransform.GetLocalPosition().y +
                                           parentClassNameTag->mTransform.GetSize().y + 10);
    classNameTag->SetFontSize(fontSize);
    classNameTag->SetText("클래스 이름");

    auto classNameEditBox = classInputPanel->CreateChildUIElement<UI::UIEditBox>("classEditBox");
    classNameEditBox->SetSize(500, 30);
    classNameEditBox->SetFontSize(fontSize);
    classNameEditBox->SetBackgroundColor(1, 1, 1);
    classNameEditBox->SetTextColor(0, 0, 0);
    classNameEditBox->SetPositionLocal(classNameTag->mTransform.GetLocalPosition().x +
                                           classNameTag->mTransform.GetSize().x + 40.0f,
                                       classNameTag->mTransform.GetLocalPosition().y);

    auto classGeneartionButton = classInputPanel->CreateChildUIElement<UI::UITextButton>("ClassGenerationButton");
    classGeneartionButton->SetSize(100, 40);
    classGeneartionButton->mUIImageComponent->SetColor(0.3f, 0.3f, 0.3f);
    classGeneartionButton->mTextComponent->SetText(" 생성 ");
    classGeneartionButton->mTextComponent->SetFontSize(fontSize);
    classGeneartionButton->SetPositionLocal(
        classInputPanel->mTransform.GetSize().x - classGeneartionButton->mTransform.GetSize().x - 20.0f,
        classInputPanel->mTransform.GetSize().y - 10.0f - classGeneartionButton->mTransform.GetSize().y);

    classGeneartionButton->mUIButtonComponent->mButtonClickCallbackSystem.Register(
        [this, parentClassNameEditBox, classNameEditBox](float, float)
        {
            CloseCurrentContextMenuAll();
            mGenerationObjectClassPanel->SetActiveFlag(false);

            std::string parentClassName = parentClassNameEditBox->GetText();
            std::string targetClassName = classNameEditBox->GetText();

            if (parentClassName.empty() || targetClassName.empty())
                return;

            auto reflectSystem = Quad::ReflectionSystem::GetInstance();
            auto classInfo = reflectSystem->FindClassInfo(parentClassName.c_str());

            bool coreClass = false;
            if (classInfo)
            {
                coreClass = classInfo->IsEngineClass();
            }

            Quad::ClassGenerationManager::GetInstance()->RequestClassGeneration(targetClassName.c_str(),
                                                                                parentClassName.c_str(), coreClass);

            // SetCurrentTaskUIController(mClassGenerationTaskUIController.get());
            //   SetCurrentTaskUIController(nullptr);
            //  TODO
            //  실제 parentClass가 있는지

            // targetClassName이 이미존재하는지 검사

            // auto projectConfig = Quad::ProjectConfig::GetInstance();
            //// projectConfig->GetProjectPath();

            // std::string targetPath = projectConfig->GetProjectPath() + "/Source";

            //// 여기서 부터는 WorkerThread 가 수행한다.

            // auto threadPool = AsyncThreadPool::GetInstance();
            // threadPool->Submit(
            //     [this, parentClassName, targetClassName, targetPath, coreClass, projectConfig]()
            //     {
            //         // mProgressContext

            //        PrograssContext prograssContext(mWorkingTaskContext);

            //        ProjectGenerator::GenerateObjectCXXFile(parentClassName, targetClassName, targetPath,
            //                                                prograssContext, coreClass);

            //        ProjectGenerator::ReBuildCMake(projectConfig->GetProjectPath(), projectConfig->GetProjectName(),
            //                                       prograssContext);

            //        SetCurrentTaskUIController(nullptr);
            //    });

            //    ProjectGenerator::GenerateObjectCXXFile(parentClassName, targetClassName, targetPath, coreClass);
            //   ProjectGenerator::ReBuildCMake(projectConfig->GetProjectPath(), projectConfig->GetProjectName());
            // 파일 생성
        });

    // Class list
    auto classListScrollPanel =
        mGenerationObjectClassPanel->CreateChildUIElement<ClassListUIScrollPanel>("ClassListScrollPanel");
    classListScrollPanel->SetScrollPanelColor(0.2f, 0.2f, 0.2f);
    classListScrollPanel->SetSize(700, 500);

    classListScrollPanel->mOnClickedClassItemCallbackSystem.Register(
        [parentClassNameEditBox](const std::string &className) { parentClassNameEditBox->SetText(className); });

    // pragrass ui data

    mClassGenerationTaskUIController = std::make_unique<ClassGenerationTaskUIController>();
    mClassGenerationTaskUIController->Initialize(mOverlayCanvas);
    mClassGenerationTaskUIController->SetPosition(500, 600);
    ClassGenerationTaskUIController *generationTaskUIController =
        static_cast<ClassGenerationTaskUIController *>(mClassGenerationTaskUIController.get());
    generationTaskUIController->mOnClickedExitButtonCallbackSystem.Register([this]()
                                                                            { SetCurrentTaskUIController(nullptr); });
}

void GlobalOverlayManager::CreateGenerationPrefabPanel()
{

    mGenerationPrefabPanel = mOverlayCanvas->CreateUIElement<UI::UIImage>("GenerationPrefabPanel");
    mGenerationPrefabPanel->SetSize(800, 800);
    mGenerationPrefabPanel->SetColor(0.4f, 0.4f, 0.4f);
    mGenerationPrefabPanel->SetPositionLocal(500, 600);

    auto exitButton = mGenerationPrefabPanel->CreateChildUIElement<UI::UIButton>("ExitButton");
    exitButton->SetSize(40, 40);
    exitButton->mUIImageComponent->UseTexture();
    exitButton->mUIImageComponent->SetTexture("Engine/Exit");
    exitButton->SetPositionLocal(mGenerationPrefabPanel->mTransform.GetSize().x - exitButton->mTransform.GetSize().x,
                                 0);
    exitButton->mUIButtonComponent->mButtonClickCallbackSystem.Register(
        [this](float, float) { mGenerationPrefabPanel->SetActiveFlag(false); });

    auto classListScrollPanel =
        mGenerationPrefabPanel->CreateChildUIElement<ClassListUIScrollPanel>("ClassListScrollPanel");
    classListScrollPanel->SetScrollPanelColor(0.2f, 0.2f, 0.2f);
    classListScrollPanel->SetSize(700, 500);

    float fontSize = 20.0f;
    float marginX = 20.0f;
    float marginY = 30.0f;

    auto parentTag = mGenerationPrefabPanel->CreateChildUIElement<UI::UIText>("ParentTag");
    parentTag->SetSize(100, 40);
    parentTag->SetFontSize(fontSize);
    parentTag->SetTextColor({1, 1, 1});
    parentTag->SetText("부모 클래스");
    parentTag->SetPositionLocal(marginX, classListScrollPanel->mTransform.GetLocalPosition().y +
                                             classListScrollPanel->mTransform.GetSize().y + marginY);

    auto parentText = mGenerationPrefabPanel->CreateChildUIElement<UI::UIText>("ParentText");
    parentText->SetSize(500, 40);
    parentText->SetFontSize(fontSize);
    parentText->SetPositionLocal(parentTag->mTransform.GetLocalPosition().x + parentTag->mTransform.GetSize().x +
                                     marginX,
                                 parentTag->mTransform.GetLocalPosition().y);

    classListScrollPanel->mOnClickedClassItemCallbackSystem.Register([parentText](const std::string &className)
                                                                     { parentText->SetText(className); });

    auto prefabNameTag = mGenerationPrefabPanel->CreateChildUIElement<UI::UIText>("PrefabNameTag");
    prefabNameTag->SetSize(100, 40);
    prefabNameTag->SetFontSize(fontSize);
    prefabNameTag->SetTextColor({1, 1, 1});
    prefabNameTag->SetText("프리팹 이름");
    prefabNameTag->SetPositionLocal(marginX, parentTag->mTransform.GetLocalPosition().y +
                                                 parentTag->mTransform.GetSize().y + marginY);

    auto prefabNameEditBox = mGenerationPrefabPanel->CreateChildUIElement<UI::UIEditBox>("PrefabNameEditBox");
    prefabNameEditBox->SetSize(500, 40);
    prefabNameEditBox->SetFontSize(fontSize);
    prefabNameEditBox->SetTextColor(0, 0, 0);
    prefabNameEditBox->SetBackgroundColor(0.3f, 0.3f, 0.3f);
    prefabNameEditBox->SetPositionLocal(prefabNameTag->mTransform.GetLocalPosition().x +
                                            parentTag->mTransform.GetSize().x + marginX,
                                        prefabNameTag->mTransform.GetLocalPosition().y);

    auto generationButton = mGenerationPrefabPanel->CreateChildUIElement<UI::UITextButton>("PrefabGenerationButton");
    generationButton->SetSize(100, 40);
    generationButton->mTextComponent->SetText("생성");
    generationButton->mTextComponent->SetFontSize(fontSize);
    generationButton->mTextComponent->SetColor(1, 1, 1);
    generationButton->mUIImageComponent->SetColor(0.2f, 0.2f, 0.2f);
    generationButton->SetPositionLocal(
        mGenerationPrefabPanel->mTransform.GetSize().x - generationButton->mTransform.GetSize().x - 20.0f,
        prefabNameTag->mTransform.GetSize().y + prefabNameTag->mTransform.GetLocalPosition().y + marginY);

    generationButton->mUIButtonComponent->mButtonClickCallbackSystem.Register(
        [this, prefabNameEditBox, parentText](float, float)
        {
            mGenerationPrefabPanel->SetActiveFlag(false);

            // TODO Prefab생성 (parentClassName, prefabClassName)
            PrefabGenerationManager *prefabGenerationManager = PrefabGenerationManager::GetInstance();
            prefabGenerationManager->CreatePrefab(parentText->GetTextComponent()->GetText(),
                                                  prefabNameEditBox->GetText());
            // current logical folder
        });

    mGenerationPrefabPanel->SetActiveFlag(false);
}

void GlobalOverlayManager::ShowContextMenu(UI::UIImage *panel, UI::UITextButton *parentButton)
{
    CloseCurrentContextMenu(panel, parentButton);

    // mCurrentMenuContextPanel = panel;
    panel->SetActiveFlag(true);
    mCurrentMenuContextPanelList.push_back(panel);
}

bool GlobalOverlayManager::CheckCurrentOpenedMenu(UI::UIImage *panel)
{
    auto it = std::find(mCurrentMenuContextPanelList.begin(), mCurrentMenuContextPanelList.end(), panel);

    if (it != mCurrentMenuContextPanelList.end())
        return true;

    return false;
}

void GlobalOverlayManager::SetCurrentTaskUIController(ITaskUIController *controller)
{
    if (mCurrentTaskUIController)
    {
        mCurrentTaskUIController->SetActive(false);
        mCurrentTaskUIController->ReleaseKeyboardCapture();
        mCurrentTaskUIController->ReleaseMouseCapture();
    }

    mIsWorking = false;

    mCurrentTaskUIController = controller;
    if (mCurrentTaskUIController != nullptr)
    {

        mCurrentTaskUIController->SetActive(true);
        mCurrentTaskUIController->RequestKeyboardCapture();
        mCurrentTaskUIController->RequestMouseCapture();
        mIsWorking = true;
    }
}

void GlobalOverlayManager::UpdatePrefabPosIfDragDrop(int clinetPosX, int clientPosY)
{

    if (mCurrentDragDropPayload.mType == EDragDropType::ePrefab && mIsDragDrop)
    {

        if (mIsMouseOnMainWindow)
        {
            Core::LogicalWindow *mainWindow = Quad::EditorDirector::GetInstance()->GetMainSceneWindow();
            //  const CoreMath::Ray &ray = mainWindow->GetWorldRay();
            const CoreMath::Ray &ray = mainWindow->CaculateWorldRay(clinetPosX, clientPosY);

            CoreMath::Vector3 pos = ray.mOrigin + ray.mDirection * 30.0f;

            Prefab *prefab = static_cast<Prefab *>(mCurrentDragDropPayloadContext.mAsset);
            Entity *entity = static_cast<Entity *>(prefab->mDefaultObject);
            entity->SetPositionLocal(pos);

            std::string text;
            text = std::to_string(pos.X) + " , " + std::to_string(pos.Y) + " , " + std::to_string(pos.Z);

            mDebugHUD->AddText(text);
        }
    }
}

void GlobalOverlayManager::OpenGameModeSettingPanel()
{

    if (mMapPlaySettingPanel == nullptr)
    {
        mMapPlaySettingPanel = mOverlayCanvas->CreateUIElement<MapPlaySettingPanel>("MapPlaySettingPanel");
        mMapPlaySettingPanel->SetSize(500, 300);
        mMapPlaySettingPanel->SetPositionLocal(300, 300);
    }

    if (mMapPlaySettingPanel)
    {

        mMapPlaySettingPanel->SetActiveFlag(true);
        World *world = Quad::EditorSceneManager::GetInstance()->GetUserWorld();
        Map *map = world->GetCurrentMap();
        mMapPlaySettingPanel->BindMap(map);
        mMapPlaySettingPanel->SetObjectSourceList(BuildObjectSourceCandiateList());
        mMapPlaySettingPanel->SetObjectControllerSourceList(BuildObjectControllerSourceCandidateList());
    }
}

void GlobalOverlayManager::OpenMapRenderInspector()
{
    DefaultEditorInspectorManager *defaultEditorInspectorManager = DefaultEditorInspectorManager::GetInstance();

    InspectorContext context;
    World *world = Quad::EditorSceneManager::GetInstance()->GetUserWorld();
    Map *map = world->GetCurrentMap();
    context.mTargetMap = map;

    defaultEditorInspectorManager->ActivateInsepctor(EDefaultEditorInspectorType::eMapSetting, context);
}

std::vector<Core::ObjectSourceCandidate> GlobalOverlayManager::BuildObjectSourceCandiateList()
{
    std::vector<Core::ObjectSourceCandidate> list;

    // reflection class list(user class)
    AppendObjectClassCandidate(list);

    // prefab list
    AppendPrefabCandidate(list);

    return list;
}

void GlobalOverlayManager::AppendObjectClassCandidate(std::vector<Core::ObjectSourceCandidate> &oList)
{

    auto reflectionSystem = Quad::ReflectionSystem::GetInstance();

    for (auto classInfo : reflectionSystem->GetUserClassList())
    {
        Core::ObjectSourceCandidate source;

        source.mClassName = classInfo->GetTypeName();
        source.mDisplayName = source.mClassName;
        source.mType = Core::ESpawnObjectSourceType::eClass;

        oList.push_back(std::move(source));
    }
}

void GlobalOverlayManager::AppendPrefabCandidate(std::vector<Core::ObjectSourceCandidate> &oList)
{

    auto globalAssetRegistrySystem = CoreAsset::GlobalAssetRegistrySystem::GetInstance();

    std::vector<CoreAsset::Asset *> prefabList;
    globalAssetRegistrySystem->GetAssetsByType(CoreAsset::EAssetType::ePrefab, prefabList);

    for (auto asset : prefabList)
    {
        Core::ObjectSourceCandidate source;

        source.mPrefabID = asset->GetID();
        source.mDisplayName = asset->GetName().c_str();
        source.mType = Core::ESpawnObjectSourceType::ePrefab;

        oList.push_back(std::move(source));
    }
}

std::vector<Core::ObjectSourceCandidate> GlobalOverlayManager::BuildObjectControllerSourceCandidateList()
{
    std::vector<Core::ObjectSourceCandidate> list;

    auto reflectionSystem = Quad::ReflectionSystem::GetInstance();

    // engine class인 PlayerController도 넣는다.
    Core::ObjectSourceCandidate source;
    source.mClassName = "PlayerController";
    source.mDisplayName = source.mClassName;
    source.mType = Core::ESpawnObjectSourceType::eClass;
    source.mPrefabID = NoneAssetID;

    list.push_back(source);

    for (auto classInfo : reflectionSystem->GetUserClassList())
    {
        Core::ObjectSourceCandidate source;

        if (classInfo->IsAncestorClass("ObjectController") == false)
            continue;

        source.mClassName = classInfo->GetTypeName();
        source.mDisplayName = source.mClassName;
        source.mType = Core::ESpawnObjectSourceType::eClass;
        source.mPrefabID = NoneAssetID;

        list.push_back(source);
    }

    return list;
}

void GlobalOverlayManager::StartImport()
{

    std::string path = EditorUtility::OpenFileDialog(nullptr);

    Quad::EditorAssetImporterManager::GetInstance()->RequestImport(path.c_str());
}

void GlobalOverlayManager::UpdateDragDropMainWindowState(Core::LogicalWindow *hitWindow)
{

    if (mMainWindow == hitWindow)
    {

        if (mIsMouseOnMainWindow == false)
            OnDragDropMouseEnterMainWindow();
    }
    else
    {
        if (mIsMouseOnMainWindow == true)
            OnDragDropMouseLeaveMainWindow();
    }
}

void GlobalOverlayManager::Create3DObjectToWorld(EDefault3DObjectType objectType)
{

    Quad::EditorSceneManager *sceneManager = Quad::EditorSceneManager::GetInstance();

    if (sceneManager == nullptr)
        return;

    auto world = sceneManager->GetUserWorld();
    if (world == nullptr)
        return;

    Map *map = world->GetCurrentMap();

    if (map == nullptr)
        return;

    std::string entityInstanceName;
    std::string meshName;

    switch (objectType)
    {
    case EDefault3DObjectType::eCube:
        entityInstanceName = "Cube";
        meshName = "Engine/Cube";
        break;
    case EDefault3DObjectType::eCylinder:
        entityInstanceName = "Cylinder";
        meshName = "Engine/Cylinder";

        break;
    case EDefault3DObjectType::eCone:

        entityInstanceName = "Cone";
        meshName = "Engine/Cone";
        break;
    case EDefault3DObjectType::eArrow:
        entityInstanceName = "Arrow";
        meshName = "Engine/Arrow";
        break;
    case EDefault3DObjectType::eSphere:
        entityInstanceName = "Sphere";
        meshName = "Engine/Sphere";
        break;
    }

    Entity *object = map->CreateEntity<Entity>(entityInstanceName.c_str());

    StaticMeshComponent *staticMeshCom =
        static_cast<StaticMeshComponent *>(object->CreateComponent<StaticMeshComponent>("StaticMeshCom"));
    staticMeshCom->SetComponentFlag(Core::EComponentFlag::eEngineAdded);

    if (staticMeshCom)
    {
        staticMeshCom->SetParent(object->GetRootComponent());
        staticMeshCom->SetMesh(meshName.c_str());
        staticMeshCom->FlushPropertyDirty();
    }
}

void GlobalOverlayManager::OnDragDropMouseLeaveMainWindow()
{

    if (mIsDragDrop)
    {
        mIsMouseOnMainWindow = false;
        mDragDropImage->SetActiveFlag(true);
        if (mCurrentDragDropPayload.mType == EDragDropType::ePrefab)
        {

            Prefab *prefab = static_cast<Prefab *>(mCurrentDragDropPayloadContext.mAsset);

            // user의 월드에 prefab의 object를 제거하고
            Quad::EditorSceneManager::GetInstance()->GetUserWorld()->RemovePrefabObject(prefab->mDefaultObject);
            // prefab object의 복사본을 world에 생성
        }
    }
}
void GlobalOverlayManager::OnDragDropMouseEnterMainWindow()
{

    if (mIsDragDrop)
    {
        mIsMouseOnMainWindow = true;
        mDragDropImage->SetActiveFlag(false);

        /*
         마우스가 main window에 있는가?
         처음 진입인가?  image -> false   ,  해당 월드에 default object add
         마우스 레이캐스팅  , 해당위치에 배치

         //아니다
         //main window에서 빠진것인가?
         // image- > true ; 해당 월드에서 default object remove
         */

        if (mCurrentDragDropPayload.mType == EDragDropType::ePrefab)
        {

            Prefab *prefab = static_cast<Prefab *>(mCurrentDragDropPayloadContext.mAsset);

            // user의 월드에 prefab의 object를 넣어준다.
            Quad::EditorSceneManager::GetInstance()->GetUserWorld()->AddPrefabObject(prefab->mDefaultObject);
        }
    }
}

void GlobalOverlayManager::CloseCurrentContextMenu(UI::UIImage *selectedPanel, UI::UITextButton *parentButton)
{
    // 선택된 판넬 또는 선택된 판넬의 부모 판넬이 존재하는가 .
    //  존재한다면 그 부모 + 조상 판넬은 active상태를 유지한다.

    if (parentButton == nullptr)
    {
        CloseCurrentContextMenuAll();
        return;
    }

    auto parentPanel = parentButton->GetParent();
    if (parentPanel == nullptr)
    {
        CloseCurrentContextMenuAll();
        return;
    }

    auto eraseIt = std::find(mCurrentMenuContextPanelList.begin(), mCurrentMenuContextPanelList.end(), selectedPanel);

    if (eraseIt == mCurrentMenuContextPanelList.end())
    {

        eraseIt = std::find(mCurrentMenuContextPanelList.begin(), mCurrentMenuContextPanelList.end(), parentPanel);
        if (eraseIt == mCurrentMenuContextPanelList.end())
        {
            eraseIt = mCurrentMenuContextPanelList.begin();
        }
        else
        {
            eraseIt += 1;
        }
        // eraseIt = mCurrentMenuContextPanelList.begin();
    }

    for (auto it = eraseIt; it != mCurrentMenuContextPanelList.end(); ++it)
    {
        (*it)->SetActiveFlag(false);
    }

    mCurrentMenuContextPanelList.erase(eraseIt, mCurrentMenuContextPanelList.end());
}

void GlobalOverlayManager::CloseCurrentContextMenuAll()
{

    for (auto panel : mCurrentMenuContextPanelList)
    {
        panel->SetActiveFlag(false);
    }

    mCurrentMenuContextPanelList.clear();
}

void GlobalOverlayManager::ShowSaveMapBox(Map *map, const std::function<void()> &onDecisionCallback)
{
    mSaveMapPanel->SetActiveFlag(true);
    mToSaveMap = map;
    mOnSaveMapDicisionCallback = onDecisionCallback;
}

void GlobalOverlayManager::StartDragDrop(const DragPayload &payload)
{

    // TODO
    mCurrentDragDropPayload = payload;
    mCurrentDragDropPayloadContext.mAsset =
        CoreAsset::AssetManager::GetInstance()->GetAssetCommon(payload.mAssetID).Get();
    mCurrentDragDropPayloadContext.mObject = payload.mObject;
    mCurrentDragDropPayloadContext.mType = payload.mType;
    mCurrentDragDropPayloadContext.mUIElement = payload.mUIElement;

    // 이미지 ui띄위기 .
    // 현재 마우스이미지에
    const auto &mouseContext = Quad::InputSystem::GetInstance()->GetMouseContext();

    mouseContext.mClientPosX;
    mouseContext.mClientPosY;

    glm::vec2 worldPos = mOverlayWindow->ConverToWorldPos(mouseContext.mClientPosX, mouseContext.mClientPosY);
    mDragDropImage->SetPositionLocal(worldPos);

    // 그 이미지 ui 마우스 캡처

    mDragDropImage->SetActiveFlag(true);

    mIsDragDrop = true;

    // if (payload.mType == EDragDropType::ePrefab)
    //{

    //    Prefab *prefab =
    //        static_cast<Prefab
    //        *>(CoreAsset::AssetManager::GetInstance()->GetAsset<Prefab>(payload.mAssetID).Get());

    //    // user의 월드에 prefab의 object를 넣어준다.
    //    Quad::EditorSceneManager::GetInstance()->GetUserWorld()->AddPrefabObject(prefab->mDefaultObject);
    //}
}

void GlobalOverlayManager::ClearDragDrop()
{
    if (mCurrentDragDropPayload.mType == EDragDropType::ePrefab)
    {

        if (mIsMouseOnMainWindow)
        {
            Prefab *prefab = static_cast<Prefab *>(
                CoreAsset::AssetManager::GetInstance()->GetAsset<Prefab>(mCurrentDragDropPayload.mAssetID).Get());

            Quad::EditorSceneManager::GetInstance()->GetUserWorld()->RemovePrefabObject(prefab->mDefaultObject);

            prefab->Instantiate(Quad::EditorSceneManager::GetInstance()->GetUserWorld()->GetCurrentMap());
        }
        // user의 월드에 prefab의 object를 넣어준다.
    }

    mDragDropImage->SetActiveFlag(false);
    mIsDragDrop = false;
}

void GlobalOverlayManager::TryDropCurrentPayload()
{

    if (mCurrentHoverDropTarget != nullptr)
    {
        UIDropTargetComponent *targetCom = nullptr;
        mCurrentHoverDropTarget->GetComponents<UIDropTargetComponent>(&targetCom, 1);

        if (targetCom)
        {
            targetCom->OnDroppedPayload(mCurrentDragDropPayload);
        }
    }

    ClearDragDrop();
}

bool GlobalOverlayManager::GetDragDropState() const
{
    return mIsDragDrop;
}

const DragPayload &GlobalOverlayManager::GetCurrentDragPayload() const
{
    return mCurrentDragDropPayload;
    // TODO: 여기에 return 문을 삽입합니다.
}

EScenePlayState GlobalOverlayManager::GetScenePlayState() const
{
    return mScenePlayState;
}

void GlobalOverlayManager::SetScenePlayState(EScenePlayState state)
{

    mScenePlayState = state;
}

void GlobalOverlayManager::OnScenePlayButtonDown()
{

    EScenePlayState currState = GetScenePlayState();
    EScenePlayState targetState;
    switch (currState)
    {
    case EScenePlayState::ePlay:
    {

        // 정지상태로
        Quad::EditorSceneManager::GetInstance()->PauseUserWorld();
        targetState = EScenePlayState::ePause;
    }
    break;
    case EScenePlayState::eNone:
    {

        Quad::EditorSceneManager::GetInstance()->PlayUserWorld();
        // 플레이상태로
        targetState = EScenePlayState::ePlay;
    }
    break;
    case EScenePlayState::ePause:
    {
        // 다시 플레이상태로
        Quad::EditorSceneManager::GetInstance()->ReleaseUserWorldPause();
        targetState = EScenePlayState::ePlay;
    }
    }

    SetScenePlayState(targetState);
}

void GlobalOverlayManager::OnScenePlayEndButtonDown()
{

    EScenePlayState currState = GetScenePlayState();
    EScenePlayState targetState;
    switch (currState)
    {
    case EScenePlayState::ePlay:
    case EScenePlayState::ePause:
    {
        // 정지상태로
        Quad::EditorSceneManager::GetInstance()->EndUserWorld();
        targetState = EScenePlayState::eNone;
    }
    break;
    }

    SetScenePlayState(EScenePlayState::eNone);
}

void GlobalOverlayManager::ShowPrefabEditWindow(Prefab *prefab)
{

    //  mToolbar->SetActiveFlag(false);
    //
    // or 최상위 ui panel 전환
    //  mPrefabEditUIContext.SetActive(true);

    // 다른창
}

PrefabEditUIContext::PrefabEditUIContext() {}

PrefabEditUIContext::~PrefabEditUIContext() {}

void PrefabEditUIContext::SetActive(bool flag)
{

    if (mComPanel)
    {
        mComPanel->SetActiveFlag(flag);
    }
}
