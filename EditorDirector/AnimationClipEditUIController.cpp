#include "AnimationClipEditUIController.h"
#include <EditorDirector/EditorUIUtility.h>

#include <CoreAsset/Skeleton.h>
#include <EditorDirector/UIBoolPanel.h>
#include <EditorDirector/UIScrollBox.h>
#include <UiSystem/UIButton.h>
#include <UiSystem/UIButtonComponent.h>
#include <UiSystem/UICanvas.h>
#include <UiSystem/UIImage.h>
#include <UiSystem/UIImageComponent.h>
#include <UiSystem/UIMouseDragComponent.h>
#include <UiSystem/UITextButton.h>
#include <UiSystem/UIText.h>
#include <UiSystem/UITextComponent.h>
#include <algorithm>
#include <string>
#include <utility>

void AnimationClipEditUIController::Initialize(UI::UICanvas *canvas, float toolbarHeight)
{
    mCanvas = canvas;

    mSkeletonTree = EditorUIUtility::Create<UIScrollBox>(canvas, "AnimationSkeletonTree");

    mSkeletonTree->SetBackgrounColor(UI::UIColor::DarkGray);
    mSkeletonTree->SetLayout(EUIScrollLayout::eVertical);
    EditorUIUtility::ApplyPreset(mSkeletonTree, UI::EUIStyleRole::ePanel);
    mSkeletonTree->SetSize(360.0f, 900.0f);
    mSkeletonTree->SetHorizontalAnchor(0.0f);
    mSkeletonTree->SetHorizontalOffset(0.0f);
    mSkeletonTree->SetVerticalAnchor(0.0f);
    mSkeletonTree->SetVerticalOffset(toolbarHeight);

    //
    auto AnimationClipPlayPanel = EditorUIUtility::CreatePanel(canvas, "AnimationClipPlayPanel");
    AnimationClipPlayPanel->SetColor(UI::UIColor::DarkGray);

    AnimationClipPlayPanel->SetSize(1500.0f, 400.0f);
    AnimationClipPlayPanel->SetHorizontalAnchor(0.0f);
    AnimationClipPlayPanel->SetHorizontalOffset(360.0f);
    AnimationClipPlayPanel->SetVerticalAnchor(1.0f);
    AnimationClipPlayPanel->SetVerticalOffset(-(AnimationClipPlayPanel->GetSize().Y + 0.0f));
    // 자체 Pivot이 (0, 0)이므로 기존 우측/하단 정렬은 크기를 포함한 음수 Offset으로 보존한다.
    AnimationClipPlayPanel->mOnChangedSizeCallbackSystem.Register(
        [](UI::UIElement *element)
        {
            element->SetVerticalOffset(-(element->GetSize().Y + 0.0f));
            element->UpdatePosAnchor();
        });

    mPlayButton = EditorUIUtility::CreateLargeButton(AnimationClipPlayPanel, "AnimationClipPlayButton");
    mPlayButton->SetPositionLocal(16.0f, 16.0f);
    mPlayButton->SetUseHoverImageColor(false);
    mPlayButton->mUIImageComponent->UseTexture();
    SetPlayButtonState(false, false);
    mPlayButton->mUIButtonComponent->mButtonClickCallbackSystem.Register(
        [this](float, float)
        {
            if (mOnPlayButtonClicked)
                mOnPlayButtonClicked();
        });

    // 분할 클립 버튼
    mClipSplitButton = EditorUIUtility::CreateMediumTextButton(AnimationClipPlayPanel, "AnimationClipSplitButton");
    mClipSplitButton->SetWidth(48.0f);
    mClipSplitButton->mTextComponent->SetPaddingLeft(5.0f);
    //  mClipSplitButton->mTextComponent->SetColor();

    mClipSplitButton->mTextComponent->SetText("분할");
    mClipSplitButton->SetUseHoverImageColor(false);
    mClipSplitButton->mUIImageComponent->NotUseTexture();
    mClipSplitButton->mUIImageComponent->SetUseBorderFlag(true);
    mClipSplitButton->SetHorizontalAnchor(1.0f);
    mClipSplitButton->SetHorizontalOffset(-(mClipSplitButton->GetSize().X + 16.0f));
    mClipSplitButton->SetVerticalAnchor(0.0f);
    mClipSplitButton->SetVerticalOffset(16.0f);
    // 자체 Pivot이 (0, 0)이므로 기존 우측/하단 정렬은 크기를 포함한 음수 Offset으로 보존한다.
    mClipSplitButton->mOnChangedSizeCallbackSystem.Register(
        [](UI::UIElement *element)
        {
            element->SetHorizontalOffset(-(element->GetSize().X + 16.0f));
            element->UpdatePosAnchor();
        });
    mClipSplitButton->mUIButtonComponent->mButtonClickCallbackSystem.Register([this](float, float)
                                                                              { OnClickedClipSplitButton(); });
    SetPlayButtonState(false, false);

    // clip track
    // 현재 프레임 막대기 , 재생하면 움직이고 , 정지시켜놓고 막대만 움직여서도 재생가능 - 특정자세들 확인가능하도록
    // 재생하면 현재 재생 프레임, 시간을 받아야 막대기의 위치를 조정할수있을거고

    auto animTrack = EditorUIUtility::Create<UI::UIImage>(AnimationClipPlayPanel, "AnimationTrack");
    UI::UIControlStyleOverride animTrackVisual;
    animTrackVisual.mBackgroundColor = UI::UIColor::DimGray;
    mAnimClipTrackLength = 1200.0f;

    animTrack->SetStyleOverride(animTrackVisual);
    animTrack->SetSize(mAnimClipTrackLength, 100.0f);
    animTrack->SetPositionLocal(50, 100.0f);
    animTrack->UseTexture(false);

    auto animTrackStick = EditorUIUtility::Create<UI::UIImage>(animTrack, "AnimTrackStick");
    // 시간/구간 표식은 의미를 가진 색상을 사용하고, 크기는 트랙 좌표계에서 지정한다.
    UI::UIControlStyleOverride stickVisual;
    stickVisual.mBackgroundColor = UI::UIColor::DarkYellow;
    animTrackStick->SetStyleOverride(stickVisual);
    animTrackStick->SetSize(10.0f, 48.0f);

    auto animTrackStickMouseDragCom = animTrackStick->CreateUIComponent<UI::UIMouseDragComponent>("MouseDragCom");

    animTrackStickMouseDragCom->mOnDraggedCallbackSystem.Register(
        [this, animTrackStick](const UI::UIMouseDragContext &context)
        { MoveTrackStick(animTrackStick, context.mDeltaX, true); });

    mAnimTrackStick = animTrackStick;

    mAnimTrackStartStick = EditorUIUtility::Create<UI::UIImage>(animTrack, "AnimTrackStick");
    stickVisual.mBackgroundColor = UI::UIColor::Red;
    mAnimTrackStartStick->SetStyleOverride(stickVisual);
    mAnimTrackStartStick->SetSize(10.0f, 48.0f);

    animTrackStickMouseDragCom = mAnimTrackStartStick->CreateUIComponent<UI::UIMouseDragComponent>("MouseDragCom");

    animTrackStickMouseDragCom->mOnDraggedCallbackSystem.Register(
        [this](const UI::UIMouseDragContext &context)
        { MoveTrackStick(mAnimTrackStartStick, context.mDeltaX, false); });

    mAnimTrackEndStick = EditorUIUtility::Create<UI::UIImage>(animTrack, "AnimTrackStick");
    stickVisual.mBackgroundColor = UI::UIColor::Blue;
    mAnimTrackEndStick->SetStyleOverride(stickVisual);
    mAnimTrackEndStick->SetSize(10.0f, 48.0f);

    animTrackStickMouseDragCom = mAnimTrackEndStick->CreateUIComponent<UI::UIMouseDragComponent>("MouseDragCom");

    animTrackStickMouseDragCom->mOnDraggedCallbackSystem.Register(
        [this](const UI::UIMouseDragContext &context) { MoveTrackStick(mAnimTrackEndStick, context.mDeltaX, false); });

    CreateClipSettingsPanel(toolbarHeight);
}

void AnimationClipEditUIController::CreateClipSettingsPanel(float toolbarHeight)
{
    // Skeleton 트리와 독립된 우측 패널이다. 클립 참조는 저장하지 않아 에셋 전환 후 이전 대상을 수정하지 않는다.
    mClipSettingsPanel = EditorUIUtility::Create<UIScrollBox>(mCanvas, "AnimationClipSettingsPanel");
    EditorUIUtility::ApplyPreset(mClipSettingsPanel, UI::EUIStyleRole::ePanel);
    mClipSettingsPanel->SetBackgrounColor(UI::UIColor::DarkGray);
    mClipSettingsPanel->SetLayout(EUIScrollLayout::eVertical);
    mClipSettingsPanel->SetSize(360.0f, 180.0f);
    mClipSettingsPanel->SetHorizontalAnchor(1.0f);
    mClipSettingsPanel->SetHorizontalOffset(-(mClipSettingsPanel->GetSize().X + 0.0f));
    mClipSettingsPanel->SetVerticalAnchor(0.0f);
    mClipSettingsPanel->SetVerticalOffset(toolbarHeight);
    // 자체 Pivot이 (0, 0)이므로 기존 우측/하단 정렬은 크기를 포함한 음수 Offset으로 보존한다.
    mClipSettingsPanel->mOnChangedSizeCallbackSystem.Register(
        [](UI::UIElement *element)
        {
            element->SetHorizontalOffset(-(element->GetSize().X + 0.0f));
            element->UpdatePosAnchor();
        });
    // Initialize 시점에는 Canvas가 Begin 전이므로 스크롤박스의 content가 아직 없다.
    // 여기서는 패널만 만들고, 항목은 클립을 처음 열 때 추가한다.
    mClipSettingsPanel->SetActiveFlag(false);
}

void AnimationClipEditUIController::CreateClipSettingsItems()
{
    if (!mClipSettingsPanel || mClipLoopPanel)
        return;

    auto title = EditorUIUtility::CreateLabel(mCanvas, "AnimationClipSettingsTitle");
    title->SetText("애니메이션 클립 설정");
    title->SetOnlyVisible(true);
    mClipSettingsPanel->AddItem(title);

    mClipLoopPanel = EditorUIUtility::CreateBoolField(mCanvas, "AnimationClipLoopPanel");

    mClipLoopPanel->SetTagText("루프");
    mClipLoopPanel->mOnValueChanged.Register(
        [this](bool loop)
        {
            if (mOnClipLoopChanged)
                mOnClipLoopChanged(loop);
        });
    mClipSettingsPanel->AddItem(mClipLoopPanel);
}

void AnimationClipEditUIController::SetClipLoop(bool loop)
{
    // 현재 호출 경로는 UIManager::Begin 이후의 SetClip이다. OnBegin이 끝난 패널에 한 번만 항목을 추가한다.
    // 재열기/클립 전환에서는 같은 항목과 콜백을 재사용한다.
    CreateClipSettingsItems();

    if (!mClipLoopPanel || !mClipSettingsPanel)
        return;

    // 클립을 열거나 전환하면서 표시값만 동기화할 때는 에셋 수정 콜백을 발생시키지 않는다.
    mClipLoopPanel->SetCheckValue(loop, false);
    mClipSettingsPanel->SetActiveFlag(true);
}

void AnimationClipEditUIController::SetClipLoopChangedCallback(std::function<void(bool)> callback)
{
    mOnClipLoopChanged = std::move(callback);
}

void AnimationClipEditUIController::SetPlayButtonCallback(std::function<void()> callback)
{
    mOnPlayButtonClicked = std::move(callback);
}

void AnimationClipEditUIController::ResetAnimTrackUI()
{

    if (mAnimTrackStartStick)
    {
        mAnimTrackStartStick->SetPositionLocal(0, 0);
    }

    if (mAnimTrackEndStick)
    {
        mAnimTrackEndStick->SetPositionLocal(0, 0);
    }

    if (mAnimTrackStick)
    {
        mAnimTrackStick->SetPositionLocal(0, 0);
    }
}

void AnimationClipEditUIController::SetClipSplitButtonCallback(std::function<void(float, float)> callback)
{
    mOnAnimClipSplitCallback = callback;
}

void AnimationClipEditUIController::SetPlayButtonState(bool started, bool playing)
{
    if (mPlayButton == nullptr)
        return;

    // 재생 중·일시정지·완료 후 재시작 상태를 기존 에디터의 버튼 아이콘으로 구분한다.
    const char *texture = playing ? "Engine/PlayingState" : started ? "Engine/PlayPause" : "Engine/PlayStartState";
    mPlayButton->mUIImageComponent->SetTexture(texture);
}

void AnimationClipEditUIController::SetClipDuration(float duration)
{

    mAnimStickMoveUnitPerSecond = (mAnimClipTrackLength - mAnimTrackStick->GetWidth()) / duration;
}

void AnimationClipEditUIController::SetAnimTrackStickMoveCallback(std::function<void(float)> callback)
{

    mOnAnimTrackStickMoveCallback = callback;
}

void AnimationClipEditUIController::MoveTrackStick(float shiftX)
{
    mAnimTrackStick->TranslateLocal({shiftX, 0.0f});
}

void AnimationClipEditUIController::OnStartAnimClip()
{

    if (mAnimTrackStick)
    {
        mAnimTrackStick->SetPositionLocal(0, 0);
    }
}

void AnimationClipEditUIController::OnPauseAnimClip() {}

void AnimationClipEditUIController::OnEndAnimClip()
{

    if (mAnimTrackStick)
    {
        mAnimTrackStick->SetPositionLocal(0, 0);
    }
}

void AnimationClipEditUIController::OnPlayingAnimClip(float value)
{

    if (mAnimTrackStick)
    {
        mAnimTrackStick->TranslateLocal({mAnimStickMoveUnitPerSecond * value, 0});
    }
}

float AnimationClipEditUIController::GetNormalizedStartStickPos() const
{

    if (mAnimTrackStartStick)
    {
        return GetNormalizedStickPos(mAnimTrackStartStick);
    }

    return 0.0f;
}

float AnimationClipEditUIController::GetNormalizedEndStickPos() const
{
    if (mAnimTrackEndStick)
    {
        return GetNormalizedStickPos(mAnimTrackEndStick);
    }

    return 0.0f;
}

void AnimationClipEditUIController::MoveTrackStick(UI::UIElement *stick, float deltaX, bool bCallback)
{

    // 수평이동

    // 막힐수도있다 . 범위제한
    stick->TranslateLocal({-deltaX, 0.0f});

    if (bCallback)
    {
        // 실제 움직인거리
        mOnAnimTrackStickMoveCallback(GetNormalizedStickPos(stick));
    }
}

float AnimationClipEditUIController::GetNormalizedStickPos(UI::UIElement *stick) const
{

    float value =
        std::clamp(stick->mTransform.GetLocalPosition().x / (mAnimClipTrackLength - stick->GetWidth()), 0.0f, 1.0f);

    return value;
}

void AnimationClipEditUIController::OnClickedClipSplitButton()
{
    float start = GetNormalizedStartStickPos();
    float end = GetNormalizedEndStickPos();

    mOnAnimClipSplitCallback(start, end);
}

void AnimationClipEditUIController::SetSkeleton(const CoreAsset::Skeleton &skeleton)
{
    // Skeleton은 parent-first 배열이므로 UI 행도 같은 인덱스로 만들면 부모 연결과 접힘 판정을 유지할 수 있다.
    mSkeletonTree->RemoveItemAll();
    mRows.clear();

    const auto &joints = skeleton.GetJoints();
    mRows.resize(joints.size());
    std::vector<uint32_t> depths(joints.size(), 0);

    for (uint32_t i = 0; i < joints.size(); ++i)
    {
        const uint32_t parent = joints[i].mParentIndex;
        mRows[i].mParentIndex = parent;
        mRows[i].mDisplayName = joints[i].mDisplayName;
        if (parent != CoreAsset::SkeletonJoint::NoParent && parent < i)
        {
            depths[i] = depths[parent] + 1;
            mRows[parent].mHasChildren = true;
        }
    }

    for (uint32_t i = 0; i < joints.size(); ++i)
    {
        const std::string rowName = "SkeletonJoint" + std::to_string(i);
        auto *row = EditorUIUtility::CreateMediumTextButton(mCanvas, rowName.c_str());
        row->SetWidth(360.0f);
        row->mUIImageComponent->NotUseTexture();
        // EditorUIUtility의 공통 폰트 규격 유지: row->mTextComponent->SetFontSize(20.0f);
        row->mTextComponent->SetPaddingLeft(12.0f + 20.0f * depths[i]);
        row->mTextComponent->SetText((mRows[i].mHasChildren ? "v " : "  ") + mRows[i].mDisplayName);
        if (mRows[i].mHasChildren)
        {
            row->mUIButtonComponent->mButtonClickCallbackSystem.Register([this, i](float, float) { ToggleJoint(i); });
        }
        mRows[i].mButton = row;
        mSkeletonTree->AddItem(row);
    }
}

void AnimationClipEditUIController::ToggleJoint(uint32_t jointIndex)
{
    if (jointIndex >= mRows.size() || !mRows[jointIndex].mHasChildren)
        return;

    mRows[jointIndex].mExpanded = !mRows[jointIndex].mExpanded;
    mRows[jointIndex].mButton->mTextComponent->SetText((mRows[jointIndex].mExpanded ? "v " : "> ") +
                                                       mRows[jointIndex].mDisplayName);
    RefreshVisibility();
}

void AnimationClipEditUIController::RefreshVisibility()
{
    // 자손의 표시 여부는 자기 상태뿐 아니라 모든 조상 행의 펼침 상태에 종속된다.
    std::vector<bool> visible(mRows.size(), true);
    for (uint32_t i = 0; i < mRows.size(); ++i)
    {
        const uint32_t parent = mRows[i].mParentIndex;
        if (parent != CoreAsset::SkeletonJoint::NoParent && parent < i)
            visible[i] = visible[parent] && mRows[parent].mExpanded;

        mRows[i].mButton->SetActiveFlag(visible[i]);
    }
    mSkeletonTree->ForceUpdateLayout();
}
