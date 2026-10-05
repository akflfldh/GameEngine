#pragma once

#include <CoreAsset/AssetType.h>
#include <EditorDirector/AnimationClipEditUIController.h>
#include <memory>

class World;
class Entity;
namespace Core
{
class LogicalWindow;
class WorkSpace;
} // namespace Core

namespace CoreAsset
{
class AnimationClip;
class Skeleton;
} // namespace CoreAsset

namespace UI
{
class UICanvas;
class UITheme;
} // namespace UI

/// AnimationClip 작업 공간의 World·미리보기 엔티티와 왼쪽 Skeleton UI를 관리한다.
/// 에셋은 AssetManager가 소유하며 이 관리자는 현재 클립 ID와 미리보기 엔티티만 참조한다.
/// 시간축 스크러빙은 아직 담당하지 않으며, 선택한 clip의 미리보기 재생 상태를 관리한다.
class AnimationClipWorkSpaceManager
{
  public:
    static AnimationClipWorkSpaceManager *GetInstance();

    void Initialize(Core::LogicalWindow *globalLogicalWindow, const UI::UITheme &uiTheme);
    Core::WorkSpace *GetWorkSpace() const;
    void SetClip(const CoreAsset::AnimationClip &clip, const CoreAsset::Skeleton &skeleton);
    void Update(float deltaTime);

    void OnWorkSpaceActive();

    void OnWorkSpaceInActive();

  private:
    void InitLogicalWindow(UI::UICanvas *canvas);
    void InitUI(UI::UICanvas *canvas);
    void TogglePlayback();
    void SplitClip(float startTime, float endTime);

    std::unique_ptr<Core::WorkSpace> mWorkSpace;
    std::unique_ptr<Core::LogicalWindow> mLogicalWindow;
    std::unique_ptr<World> mWorld;

    Entity *mAnimClipTargetEntity = nullptr; ///< Map 소유의 미리보기 객체이며 이 포인터는 소유하지 않는다.

    AnimationClipEditUIController mEditUIController;
    CoreAsset::AssetID mTargetClipAssetID = NoneAssetID; ///< 에셋 객체를 소유하지 않는 현재 편집 대상 identity다.
    bool mPreviewStarted = false;
    bool mPreviewPlaying = false;
    float mToolbarHeight = 50.0f;

    // 막대로 조정하여 지정한 시간
    float mTargetTimeSeconds = 0.0f;

    // 막대로 조정하였는지 여부
    bool mSeekPending = false;
};
