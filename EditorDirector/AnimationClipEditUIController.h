#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace CoreAsset
{
class Skeleton;
}

namespace UI
{
class UICanvas;
class UIButton;
class UITextButton;
class UIElement;
class UITextButton;
} // namespace UI

class UIScrollBox;
class UIBoolPanel;

/// 클립 작업 공간의 Skeleton 계층, 재생 UI와 오른쪽 클립 설정을 표시한다.
/// Canvas 소유 UI를 참조하며 에셋과 재생 상태는 소유하지 않는다. 설정 변경은 작업 공간에 콜백으로 전달한다.
class AnimationClipEditUIController
{
  public:
    void Initialize(UI::UICanvas *canvas, float toolbarHeight);
    void SetSkeleton(const CoreAsset::Skeleton &skeleton);
    void SetPlayButtonCallback(std::function<void()> callback);

    void ResetAnimTrackUI();
    // ( start normalized pos , end normalized pos)
    void SetClipSplitButtonCallback(std::function<void(float, float)> callback);
    void SetPlayButtonState(bool started, bool playing);
    void SetClipDuration(float duration);
    void SetClipLoop(bool loop);
    void SetClipLoopChangedCallback(std::function<void(bool)> callback);

    // float-deltaMove
    void SetAnimTrackStickMoveCallback(std::function<void(float)> callback);
    void MoveTrackStick(float shiftX);

    void OnStartAnimClip();
    void OnPauseAnimClip();
    void OnEndAnimClip();
    void OnPlayingAnimClip(float value);

    float GetNormalizedStartStickPos() const;
    float GetNormalizedEndStickPos() const;

  private:
    void CreateClipSettingsPanel(float toolbarHeight);
    void CreateClipSettingsItems();

    // mouse drag  verison
    void MoveTrackStick(UI::UIElement *stick, float deltaX, bool bCallback);

    float GetNormalizedStickPos(UI::UIElement *stick) const;

    void OnClickedClipSplitButton();

  private:
    struct JointRow
    {
        UI::UITextButton *mButton = nullptr;
        std::string mDisplayName;
        uint32_t mParentIndex = 0;
        bool mHasChildren = false;
        bool mExpanded = true;
    };

    void ToggleJoint(uint32_t jointIndex);
    void RefreshVisibility();

    UI::UICanvas *mCanvas = nullptr;
    UIScrollBox *mSkeletonTree = nullptr;
    UIScrollBox *mClipSettingsPanel = nullptr;
    UIBoolPanel *mClipLoopPanel = nullptr;
    UI::UIButton *mPlayButton = nullptr;
    UI::UITextButton *mClipSplitButton = nullptr;
    UI::UIElement *mAnimTrackStick = nullptr;

    // 잘라내기위한 영역
    UI::UIElement *mAnimTrackStartStick = nullptr;
    UI::UIElement *mAnimTrackEndStick = nullptr;

    std::function<void()> mOnPlayButtonClicked;
    std::vector<JointRow> mRows;

    std::function<void(float)> mOnAnimTrackStickMoveCallback;
    std::function<void(float, float)> mOnAnimClipSplitCallback;
    std::function<void(bool)> mOnClipLoopChanged;

    float mAnimClipTrackLength;
    float mAnimStickMoveUnitPerSecond;
};
