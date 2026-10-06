#pragma once

#include "AnimationTransitionSetEditUIController.h"
#include <CoreAsset/AssetType.h>
#include <memory>

namespace Core
{
class LogicalWindow;
class WorkSpace;
}
namespace CoreAsset
{
class AnimationTransitionSet;
}
namespace UI
{
class UICanvas;
class UITheme;
}

// 전이 에셋 편집용 WorkSpace와 LogicalWindow를 소유하고 편집 대상은 AssetID로 추적한다.
// World/Map과 미리보기 재생 상태는 만들지 않으며, Canvas와 에셋의 소유권은 기존 시스템에 둔다.
// 에셋 저장은 EditorProjectManager의 단일 에셋 저장 경로로 위임한다.
class AnimationTransitionSetWorkSpaceManager
{
  public:
    static AnimationTransitionSetWorkSpaceManager *GetInstance();
    ~AnimationTransitionSetWorkSpaceManager();

    void Initialize(Core::LogicalWindow *globalLogicalWindow, const UI::UITheme &uiTheme);
    bool SetTransitionSet(CoreAsset::AnimationTransitionSet *transitionSet);
    Core::WorkSpace *GetWorkSpace() const;
    void OnWorkSpaceActive();
    void OnWorkSpaceInActive();
    void Update();

  private:
    AnimationTransitionSetWorkSpaceManager() = default;
    void InitLogicalWindow(Core::LogicalWindow *globalLogicalWindow);
    void SaveAsset();

    std::unique_ptr<Core::WorkSpace> mWorkSpace;
    std::unique_ptr<Core::LogicalWindow> mLogicalWindow;
    UI::UICanvas *mCanvas = nullptr;
    CoreAsset::AssetID mTargetTransitionSetAssetID = NoneAssetID;
    AnimationTransitionSetEditUIController mEditUIController;
    bool mActive = false;
};
