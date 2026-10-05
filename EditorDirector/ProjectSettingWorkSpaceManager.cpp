#include "ProjectSettingWorkSpaceManager.h"

#include <Core/LogicalWindow.h>
#include <Core/WorkSpace.h>
#include <UiSystem/UICanvas.h>
#include <UiSystem/UIManager.h>

ProjectSettingWorkSpaceManager *ProjectSettingWorkSpaceManager::GetInstance()
{
    static ProjectSettingWorkSpaceManager instance;
    return &instance;
}

ProjectSettingWorkSpaceManager::~ProjectSettingWorkSpaceManager() = default;

void ProjectSettingWorkSpaceManager::Initialize(Core::LogicalWindow *globalLogicalWindow, const UI::UITheme &uiTheme)
{
    auto *uiManager = UI::UIManager::GetInstance();
    const auto canvasID = uiManager->CreateCanvas("ProjectSettingCanvas", UI::ECanvasSizeMode::eFixSize);
    mCanvas = uiManager->GetCanvas(canvasID);
    // 최초 워크스페이스 진입 전에는 창 크기 통지가 없으므로 초기 레이아웃의 입력을 명시한다.
    mCanvas->SetSize({0.0f, 0.0f});
    mCanvas->SetTheme(uiTheme);
    mWorkSpace = std::make_unique<Core::WorkSpace>();
    mLogicalWindow = std::make_unique<Core::LogicalWindow>();

    // 기존 작업 공간과 같은 전역 오버레이를 공유하되, 설정 창에는 3D World를 연결하지 않는다.
    auto configureFullViewport = [](Core::ViewportController &viewport)
    {
        viewport.SetViewportMode(Core::EViewportMode::eAnchored);
        viewport.SetAnchorLeftState(true);
        viewport.SetAnchorLeftMode(Core::EViewportAnchoredMode::eRelative);
        viewport.SetAnchorLeftRelValue(0.0f);
        viewport.SetAnchorRightState(true);
        viewport.SetAnchorRightMode(Core::EViewportAnchoredMode::eRelative);
        viewport.SetAnchorRightRelValue(0.0f);
        viewport.SetAnchorTopState(true);
        viewport.SetAnchorTopMode(Core::EViewportAnchoredMode::eRelative);
        viewport.SetAnchorTopRelValue(0.0f);
        viewport.SetAnchorBottomState(true);
        viewport.SetAnchorBottomMode(Core::EViewportAnchoredMode::eRelative);
        viewport.SetAnchorBottomRelValue(0.0f);
    };
    configureFullViewport(mLogicalWindow->mViewportController);
    configureFullViewport(mLogicalWindow->m3DWorldViewportController);
    mLogicalWindow->SetActiveCanvas(mCanvas);
    mLogicalWindow->SetBackBufferClearColor(0.22f, 0.22f, 0.24f, 1.0f);
    mWorkSpace->AddLogicalWindow(mLogicalWindow.get());
    mWorkSpace->AddLogicalWindow(globalLogicalWindow);
    mWorkSpace->SetGlobalOverlayWindow(globalLogicalWindow);
    mUIController.Initialize(mCanvas);
    mCanvas->SetActiveFlag(false);
}

Core::WorkSpace *ProjectSettingWorkSpaceManager::GetWorkSpace() const
{
    return mWorkSpace.get();
}

UI::UICanvas *ProjectSettingWorkSpaceManager::GetCanvas() const
{
    return mCanvas;
}

void ProjectSettingWorkSpaceManager::OnWorkSpaceActive()
{
    mActive = true;
    mCanvas->SetActiveFlag(true);
    mUIController.UpdateLayout();
}

void ProjectSettingWorkSpaceManager::OnWorkSpaceInActive()
{
    mActive = false;
    mCanvas->SetActiveFlag(false);
}

void ProjectSettingWorkSpaceManager::Update()
{
    if (mActive)
        mUIController.UpdateLayout();
}

bool ProjectSettingWorkSpaceManager::AddSettingPanel(const std::string &name, UI::UIElement *panel)
{
    return mUIController.AddSettingPanel(name, panel);
}
