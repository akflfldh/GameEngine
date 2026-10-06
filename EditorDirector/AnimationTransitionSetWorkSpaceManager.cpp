#include "AnimationTransitionSetWorkSpaceManager.h"

#include <Core/LogicalWindow.h>
#include <Core/WorkSpace.h>
#include <CoreAsset/AnimationTransitionSet.h>
#include <CoreAsset/AssetManager.h>
#include <EditorDirector/EditorProjectManager.h>
#include <EditorDirector/GlobalOverlayManager.h>
#include <UiSystem/UICanvas.h>
#include <UiSystem/UIManager.h>

AnimationTransitionSetWorkSpaceManager *AnimationTransitionSetWorkSpaceManager::GetInstance()
{
    static AnimationTransitionSetWorkSpaceManager instance;
    return &instance;
}

AnimationTransitionSetWorkSpaceManager::~AnimationTransitionSetWorkSpaceManager() = default;

void AnimationTransitionSetWorkSpaceManager::Initialize(Core::LogicalWindow *globalLogicalWindow,
                                                       const UI::UITheme &uiTheme)
{
    auto *uiManager = UI::UIManager::GetInstance();
    const auto canvasID = uiManager->CreateCanvas("AnimationTransitionSetCanvas", UI::ECanvasSizeMode::eFixSize);
    mCanvas = uiManager->GetCanvas(canvasID);
    // 초기 크기 통지는 WorkSpace 진입 시 이루어지므로 그 전에는 명시적인 빈 크기를 사용한다.
    mCanvas->SetSize({0.0f, 0.0f});
    mCanvas->SetTheme(uiTheme);
    mWorkSpace = std::make_unique<Core::WorkSpace>();
    InitLogicalWindow(globalLogicalWindow);

    mEditUIController.SetSaveButtonCallback([this]() { SaveAsset(); });
    mEditUIController.Initialize(mCanvas);
    OnWorkSpaceInActive();
}

void AnimationTransitionSetWorkSpaceManager::InitLogicalWindow(Core::LogicalWindow *globalLogicalWindow)
{
    mLogicalWindow = std::make_unique<Core::LogicalWindow>();
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

    // UI 전용 화면에서도 렌더 경로가 읽는 두 viewport는 유효하게 구성한다.
    // 하단 에셋 브라우저는 기존 전역 오버레이를 공유하고 별도 World는 연결하지 않는다.
    configureFullViewport(mLogicalWindow->mViewportController);
    configureFullViewport(mLogicalWindow->m3DWorldViewportController);
    mLogicalWindow->SetActiveCanvas(mCanvas);
    mLogicalWindow->SetBackBufferClearColor(0.25f, 0.25f, 0.28f, 1.0f);
    mWorkSpace->AddLogicalWindow(mLogicalWindow.get());
    mWorkSpace->AddLogicalWindow(globalLogicalWindow);
    mWorkSpace->SetGlobalOverlayWindow(globalLogicalWindow);
}

bool AnimationTransitionSetWorkSpaceManager::SetTransitionSet(CoreAsset::AnimationTransitionSet *transitionSet)
{
    if (!transitionSet || transitionSet->GetID() == NoneAssetID)
        return false;

    // 편집 화면에서 다른 에셋을 열거나 삭제할 수 있으므로 장기 보관하는 대상은 포인터가 아닌 ID다.
    mTargetTransitionSetAssetID = transitionSet->GetID();
    mEditUIController.SetTargetTransitionSet(mTargetTransitionSetAssetID);
    return true;
}

Core::WorkSpace *AnimationTransitionSetWorkSpaceManager::GetWorkSpace() const
{
    return mWorkSpace.get();
}

void AnimationTransitionSetWorkSpaceManager::OnWorkSpaceActive()
{
    mActive = true;
    mCanvas->SetActiveFlag(true);
    mEditUIController.UpdateLayout();
}

void AnimationTransitionSetWorkSpaceManager::OnWorkSpaceInActive()
{
    mActive = false;
    mCanvas->SetActiveFlag(false);
}

void AnimationTransitionSetWorkSpaceManager::Update()
{
    if (mActive)
        mEditUIController.UpdateLayout();
}

void AnimationTransitionSetWorkSpaceManager::SaveAsset()
{
    auto asset = CoreAsset::AssetManager::GetInstance()->GetAsset<CoreAsset::AnimationTransitionSet>(
        mTargetTransitionSetAssetID);
    auto *transitionSet = asset.As<CoreAsset::AnimationTransitionSet>();
    if (!transitionSet)
    {
        GlobalOverlayManager::GetInstance()->ShowMessageBox("저장할 애니메이션 전이 에셋을 찾을 수 없습니다.");
        return;
    }

    // 직렬화와 프로젝트 상대 저장 경로는 기존 에셋 저장 시스템의 책임을 유지한다.
    // 기존 저장 API는 결과를 반환하지 않으므로 여기서는 저장 성공을 별도로 표시하지 않는다.
    Quad::EditorProjectManager::GetInstance()->SaveAsset(transitionSet);
}
