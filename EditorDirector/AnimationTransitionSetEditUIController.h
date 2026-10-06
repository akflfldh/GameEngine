#pragma once

#include <CoreAsset/AnimationTypes.h>
#include <CoreAsset/AssetType.h>
#include <functional>
#include <vector>

namespace UI
{
class UICanvas;
class UIImage;
class UIText;
class UIElement;
} // namespace UI

class UIScrollBox;
class UIDropdown;
class UIReflectFloatPanel;

// 전이 에셋 편집 화면의 UI를 구성한다. Canvas와 UI 요소는 UIManager가 소유하며,
// 편집 대상은 ID로 추적하고 에셋과 저장 경로는 소유하지 않는다. 저장 요청은 WorkSpaceManager로 전달한다.
class AnimationTransitionSetEditUIController
{
  public:
  public:
    void Initialize(UI::UICanvas *canvas);
    void UpdateLayout();
    void SetSaveButtonCallback(const std::function<void()> &callback);
    void SetTargetTransitionSet(CoreAsset::AssetID id);

  private:
    void CreateAnimationTransitionSetPanel();

    void CreateAnimationTransitionSettingPanel();
    void SetTransitionSettingData(size_t index);
    void RefreshTransitionSettingUI();
    void ApplyEditingTransitionToList();
    void ApplyTransitionListToAsset();
    void SetSkeleton(CoreAsset::AssetID id);
    void RefreshSkeleton();
    void RebuildTransitionList();

    UI::UIElement *GetTransitionItem(size_t index);

    void AddTransitionData();

  private:
    UI::UICanvas *mCanvas = nullptr;
    UI::UIImage *mToolbar = nullptr;
    UI::UIText *mSkeletonText = nullptr;
    CoreAsset::AssetID mTargetTransitionSetAssetID = NoneAssetID;
    std::function<void()> mSaveButtonCallback;
    float mLayoutWidth = -1.0f;
    float mToolbarHeight = 50.0f;

    UIScrollBox *mTransitionListPanel = nullptr;
    std::vector<UI::UIElement *> mTransitionItemPool;
    std::vector<CoreAsset::AnimationTransitionData> mTransitionDataList;

    // 선택한 항목의 편집용 복사본이다. UI는 vector 원소를 참조하지 않으며 원본 에셋 반영은 별도 단계다.
    CoreAsset::AnimationTransitionData mEditingTransitionData;
    bool mHasSelectedTransition = false;
    // 현재 리스트에서의 위치다. 삭제/재정렬 기능을 추가할 때는 이 인덱스도 재설정해야 한다.
    size_t mSelectedTransitionIndex = 0;

    UI::UIElement *mTransitionSettingPanel = nullptr;
    // 선택한 전이 항목의 표시값을 갱신하기 위한 비소유 참조다. UI의 수명은 Canvas/UIManager가 관리한다.
    UI::UIImage *mPreAnimClipIcon = nullptr;
    UI::UIText *mPreAnimClipText = nullptr;
    UI::UIImage *mNextAnimClipIcon = nullptr;
    UI::UIText *mNextAnimClipText = nullptr;
    UIReflectFloatPanel *mDurationPanel = nullptr;
    UIDropdown *mBlendingTypeDropdown = nullptr;
    UIReflectFloatPanel *mStartNextAnimClipTimePanel = nullptr;
};
