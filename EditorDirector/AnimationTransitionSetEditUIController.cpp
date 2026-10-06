#include "AnimationTransitionSetEditUIController.h"

#include <CoreAsset/AnimationClip.h>
#include <CoreAsset/AnimationTransitionSet.h>
#include <CoreAsset/AssetManager.h>
#include <CoreAsset/Skeleton.h>
#include <EditorDirector/EditorDirector.h>
#include <EditorDirector/EditorUIUtility.h>
#include <EditorDirector/GlobalOverlayManager.h>
#include <EditorDirector/UIDropTargetComponent.h>
#include <EditorDirector/UIDropdown.h>
#include <EditorDirector/UIReflectFloatPanel.h>
#include <EditorDirector/UIScrollBox.h>
#include <UiSystem/UIButton.h>
#include <UiSystem/UIButtonComponent.h>
#include <UiSystem/UICanvas.h>
#include <UiSystem/UIDragAndDropComponent.h>
#include <UiSystem/UIHorizontalLayoutComponent.h>
#include <UiSystem/UIImage.h>
#include <UiSystem/UIImageComponent.h>
#include <UiSystem/UIText.h>
#include <UiSystem/UITextButton.h>
#include <UiSystem/UITextComponent.h>
#include <UiSystem/UIUtility.h>
#include <UiSystem/UIVerticalLayoutComponent.h>

void AnimationTransitionSetEditUIController::Initialize(UI::UICanvas *canvas)
{
    mCanvas = canvas;
    mToolbar = EditorUIUtility::Create<UI::UIImage>(canvas, "AnimationTransitionSetToolbar");
    mToolbar->SetColor(UI::UIColor::DarkGray);
    mToolbar->SetPositionLocal(0.0f, 0.0f);

    // 버튼의 크기와 폰트는 기존 에디터 공통 생성 규격을 유지한다.
    auto *backButton = EditorUIUtility::CreateSmallButton(mToolbar, "ToDefaultEditButton");
    backButton->SetPositionLocal(5.0f, 5.0f);
    backButton->mUIImageComponent->UseTexture();
    backButton->mUIImageComponent->SetTexture("Engine/ArrowLeft");
    backButton->mUIButtonComponent->mButtonClickCallbackSystem.Register(
        [](float, float) { Quad::EditorDirector::GetInstance()->ChangeToDefaultEditWorkSpace(); });

    const float savePosX = UI::UIUtility::ShiftPosX(5.0f, backButton, 5.0f);
    auto *saveButton = EditorUIUtility::CreateSmallTextButton(mToolbar, "SaveTransitionSetButton");
    saveButton->SetPositionLocal(savePosX, 5.0f);
    saveButton->mTextComponent->SetText("저장");
    saveButton->mUIImageComponent->SetUseBorderFlag(true);
    saveButton->mUIButtonComponent->mButtonClickCallbackSystem.Register(
        [this](float, float)
        {
            // 재사용 화면의 저장 대상은 Manager가 조회하므로 에셋 포인터를 버튼에 캡처하지 않는다.
            if (mSaveButtonCallback)
                mSaveButtonCallback();
        });

    auto *applyButton = EditorUIUtility::CreateSmallTextButton(mToolbar, "ApplyTransitionSetButton");
    applyButton->SetPositionLocal(UI::UIUtility::ShiftPosX(savePosX, saveButton, 5.0f), 5.0f);
    applyButton->mTextComponent->SetText("적용");
    applyButton->mUIImageComponent->SetUseBorderFlag(true);
    applyButton->mUIButtonComponent->mButtonClickCallbackSystem.Register([this](float, float)
                                                                         { ApplyTransitionListToAsset(); });

    CreateAnimationTransitionSetPanel();
    CreateAnimationTransitionSettingPanel();

    UpdateLayout();
}

void AnimationTransitionSetEditUIController::UpdateLayout()
{
    if (!mCanvas || !mToolbar)
        return;

    const float width = mCanvas->GetWindowSize().X;
    if (width == mLayoutWidth)
        return;

    // 진입 시 LogicalWindow가 전달한 크기를 사용하고, 실제 너비 변경 때만 Transform을 갱신한다.
    mLayoutWidth = width;
    mToolbar->SetSize(width, mToolbarHeight);
}

void AnimationTransitionSetEditUIController::SetSaveButtonCallback(const std::function<void()> &callback)
{
    mSaveButtonCallback = callback;
}

void AnimationTransitionSetEditUIController::CreateAnimationTransitionSetPanel()
{

    auto basePanel = EditorUIUtility::CreatePanel(mCanvas, "TransitionSetPanel");
    auto baseLayout = basePanel->CreateUIComponent<UI::UIVerticalLayoutComponent>("verCom");
    basePanel->SetSize(600, 1000.0f);
    basePanel->SetPositionLocal(0.0f, mToolbarHeight);
    // 외곽 패널의 너비만 Skeleton 영역에 전달하고 아이콘의 공통 규격은 유지한다.
    baseLayout->SetSyncWidthFlag(true);

    auto skeletonPanel = EditorUIUtility::CreatePanel(basePanel, "SkeletonPanel");
    auto skeletonLayout = skeletonPanel->CreateUIComponent<UI::UIVerticalLayoutComponent>("verCom");
    skeletonPanel->SetWidth(basePanel->GetWidth());

    auto skeletonTag = EditorUIUtility::CreateLabel(skeletonPanel, "skeletonTag");
    skeletonTag->SetText("Skeleton");
    skeletonTag->SetWidth(skeletonPanel->GetWidth());

    auto skeletonImage = EditorUIUtility::CreateIcon(skeletonPanel, "skeletonIcon");
    skeletonImage->UseTexture(true);
    skeletonImage->SetTexture("Engine/Skeleton");
    skeletonImage->SetSize(100, 100);
    auto dropSkeletonTarget = skeletonImage->CreateUIComponent<UIDropTargetComponent>("DropTargetCom");
    dropSkeletonTarget->SetDragDropPayloadType(EDragDropType::eAssetSkeleton);
    dropSkeletonTarget->mOnDroppedPayloadCallbackSystem.Register(
        [this](const DragPayload &payload)
        {
            if (payload.mType == EDragDropType::eAssetSkeleton)
                SetSkeleton(payload.mAssetID);
        });
    mSkeletonText = EditorUIUtility::CreateLabel(skeletonPanel, "SkeletonName");
    mSkeletonText->SetWidth(skeletonPanel->GetWidth());

    auto transitionItemBasePanel = EditorUIUtility::CreatePanel(basePanel, "TransitionItemBasePanel");
    transitionItemBasePanel->CreateUIComponent<UI::UIVerticalLayoutComponent>("verCom");
    transitionItemBasePanel->SetWidth(basePanel->GetWidth());
    transitionItemBasePanel->SetHeight(500.0f);

    auto transitionItemHeaderPanel =
        EditorUIUtility::CreateHorizontalRow(transitionItemBasePanel, "headerPanel", 0.0f, false, true, 20.0f);

    auto removeButton = EditorUIUtility::CreateMediumButton(transitionItemHeaderPanel, "removeButton");
    removeButton->mUIImageComponent->UseTexture();
    removeButton->mUIImageComponent->SetTexture("Engine/minus");

    auto addButton = EditorUIUtility::CreateMediumButton(transitionItemHeaderPanel, "addButton");
    addButton->mUIImageComponent->UseTexture();
    addButton->mUIImageComponent->SetTexture("Engine/plus");

    addButton->mUIButtonComponent->mButtonClickCallbackSystem.Register([this](float, float) { AddTransitionData(); });

    auto transitionItemScrollPanel =
        transitionItemBasePanel->CreateChildUIElement<UIScrollBox>("TransitionItemScrollPanel");

    transitionItemScrollPanel->SetSize(600.0f, 500.0f);

    mTransitionListPanel = transitionItemScrollPanel;

    RefreshSkeleton();

    // Begin 전 생성과 이미 활성화된 Canvas에서의 생성 모두 최종 크기로 배치한다.
    skeletonLayout->CalculateLayout();
    baseLayout->CalculateLayout();
}

void AnimationTransitionSetEditUIController::CreateAnimationTransitionSettingPanel()
{

    auto settingPanel = EditorUIUtility::CreatePanel(mCanvas, "SettingPanel");
    settingPanel->SetSize(600, 500);
    settingPanel->CreateUIComponent<UI::UIVerticalLayoutComponent>("verCom");
    settingPanel->SetAnchor({1, 0});
    settingPanel->SetPivot({1, 0});
    settingPanel->SetOffset({0, mToolbarHeight});

    auto applyRow = EditorUIUtility::CreateHorizontalRow(settingPanel, "ApplyTransitionRow", 0.0f, false, true);
    auto applyButton = EditorUIUtility::CreateSmallTextButton(applyRow, "ApplyTransitionButton");
    applyButton->mTextComponent->SetText("적용");
    applyButton->mUIImageComponent->SetUseBorderFlag(true);
    applyButton->mUIButtonComponent->mButtonClickCallbackSystem.Register([this](float, float)
                                                                         { ApplyEditingTransitionToList(); });

    auto animClipPanel = EditorUIUtility::CreateHorizontalRow(settingPanel, "animClipPanel");

    auto preAnimClipPanel = animClipPanel->CreateChildUIElement<UI::UIElement>("PreAnimClipPanel");
    preAnimClipPanel->CreateUIComponent<UI::UIVerticalLayoutComponent>("verCom");
    preAnimClipPanel->SetWidth(300.0F);

    mPreAnimClipIcon = EditorUIUtility::CreateIcon(preAnimClipPanel, "preAnimIcon");
    mPreAnimClipIcon->SetSize(100, 100);
    mPreAnimClipIcon->UseTexture(true);
    mPreAnimClipIcon->SetTexture("Engine/AnimationClip");
    mPreAnimClipText = EditorUIUtility::CreateText(preAnimClipPanel, "preAnimClipText", "preAnimClip", 200.0f);

    auto preAnimClipDropTarget = mPreAnimClipIcon->CreateUIComponent<UIDropTargetComponent>("DropTargetCom");
    preAnimClipDropTarget->SetDragDropPayloadType(EDragDropType::eAssetAnimationClip);
    preAnimClipDropTarget->mOnDroppedPayloadCallbackSystem.Register(
        [this](const DragPayload &payload)
        {
            // 드롭 전달 경로가 타입을 차단하지 않으므로 콜백에서도 클립 payload와 실제 에셋을 확인한다.
            if (!mHasSelectedTransition || payload.mType != EDragDropType::eAssetAnimationClip)
                return;

            auto clipAsset =
                CoreAsset::AssetManager::GetInstance()->GetAsset<CoreAsset::AnimationClip>(payload.mAssetID);
            auto *clip = clipAsset.As<CoreAsset::AnimationClip>();
            if (!clip)
                return;

            mEditingTransitionData.mPreAnimClip = payload.mAssetID;
            RefreshTransitionSettingUI();
        });

    auto nextAnimClipPanel = animClipPanel->CreateChildUIElement<UI::UIElement>("nextAnimClipPanel");
    nextAnimClipPanel->CreateUIComponent<UI::UIVerticalLayoutComponent>("verCom");
    nextAnimClipPanel->SetWidth(300.0F);

    mNextAnimClipIcon = EditorUIUtility::CreateIcon(nextAnimClipPanel, "nextAnimIcon");
    mNextAnimClipIcon->SetSize(100, 100);
    mNextAnimClipIcon->UseTexture(true);
    mNextAnimClipIcon->SetTexture("Engine/AnimationClip");
    mNextAnimClipText = EditorUIUtility::CreateText(nextAnimClipPanel, "nextAnimClipText", "nextAnimClip", 200.0f);

    auto nextAnimClipDropTarget = mNextAnimClipIcon->CreateUIComponent<UIDropTargetComponent>("DropTargetCom");
    nextAnimClipDropTarget->SetDragDropPayloadType(EDragDropType::eAssetAnimationClip);
    nextAnimClipDropTarget->mOnDroppedPayloadCallbackSystem.Register(
        [this](const DragPayload &payload)
        {
            if (!mHasSelectedTransition || payload.mType != EDragDropType::eAssetAnimationClip)
                return;

            auto clipAsset =
                CoreAsset::AssetManager::GetInstance()->GetAsset<CoreAsset::AnimationClip>(payload.mAssetID);
            auto *clip = clipAsset.As<CoreAsset::AnimationClip>();
            if (!clip)
                return;

            mEditingTransitionData.mNextAnimClip = payload.mAssetID;
            RefreshTransitionSettingUI();
        });

    mDurationPanel = EditorUIUtility::CreateFloatField(settingPanel, "DurationPanel");
    mDurationPanel->SetWidth(settingPanel->GetWidth());
    mDurationPanel->SetTagText("지속 시간                ");

    mBlendingTypeDropdown = EditorUIUtility::CreateDropdown(settingPanel, "blendingTypeDropdown");
    mBlendingTypeDropdown->SetWidth(settingPanel->GetWidth());

    std::vector<std::string> blendingTypeList = {"Linear"};
    mBlendingTypeDropdown->SetItemList(blendingTypeList);
    mBlendingTypeDropdown->mOnSelectedItemChangedCallbackSystem.Register(
        [this](size_t index)
        {
            // 현재 목록은 Linear 하나다. 표시 동기화에서는 알림을 끄므로 사용자 선택에만 값을 반영한다.
            if (mHasSelectedTransition && index == 0)
                mEditingTransitionData.mBlendingType = CoreAsset::EAnimationTransitionBlendingType::eLinear;
        });

    mStartNextAnimClipTimePanel = EditorUIUtility::CreateFloatField(settingPanel, "StartTimePanel");
    mStartNextAnimClipTimePanel->SetWidth(settingPanel->GetWidth());
    mStartNextAnimClipTimePanel->SetTagText("Next 클립 시작 시간    ");

    settingPanel->SetActiveFlag(false);

    mTransitionSettingPanel = settingPanel;
}

void AnimationTransitionSetEditUIController::SetTransitionSettingData(size_t index)
{
    if (!mTransitionSettingPanel || !mDurationPanel || !mStartNextAnimClipTimePanel ||
        index >= mTransitionDataList.size())
        return;

    // 리스트 재할당에도 getter/setter의 참조 대상이 유지되도록 선택 내용을 멤버에 값으로 복사한다.
    mSelectedTransitionIndex = index;
    mEditingTransitionData = mTransitionDataList[index];
    mHasSelectedTransition = true;

    // 다른 항목 선택 시 BindFloat로 이전 입력 편집 상태와 표시 캐시도 초기화한다.
    mDurationPanel->BindFloat([this]() { return mEditingTransitionData.mDuration; },
                              [this](float value)
                              {
                                  if (mHasSelectedTransition)
                                      mEditingTransitionData.mDuration = value;
                              });
    mStartNextAnimClipTimePanel->BindFloat([this]() { return mEditingTransitionData.mNextAnimClipStartTime; },
                                           [this](float value)
                                           {
                                               if (mHasSelectedTransition)
                                                   mEditingTransitionData.mNextAnimClipStartTime = value;
                                           });

    RefreshTransitionSettingUI();
    mTransitionSettingPanel->SetActiveFlag(true);
}

void AnimationTransitionSetEditUIController::RefreshTransitionSettingUI()
{
    if (!mHasSelectedTransition)
        return;

    auto *assetManager = CoreAsset::AssetManager::GetInstance();
    auto preClipAsset = assetManager->GetAsset<CoreAsset::AnimationClip>(mEditingTransitionData.mPreAnimClip);
    auto nextClipAsset = assetManager->GetAsset<CoreAsset::AnimationClip>(mEditingTransitionData.mNextAnimClip);
    auto *preClip = preClipAsset.As<CoreAsset::AnimationClip>();
    auto *nextClip = nextClipAsset.As<CoreAsset::AnimationClip>();

    mPreAnimClipText->SetText(preClip ? preClip->GetName().c_str() : "선택된 이전 클립 없음");
    mNextAnimClipText->SetText(nextClip ? nextClip->GetName().c_str() : "선택된 다음 클립 없음");
    mDurationPanel->RefreshFromSource();
    mStartNextAnimClipTimePanel->RefreshFromSource();
    // 데이터 표시를 사용자 편집으로 취급하지 않도록 선택 알림 없이 Dropdown을 갱신한다.
    mBlendingTypeDropdown->SetSelectedIndex(static_cast<size_t>(mEditingTransitionData.mBlendingType), false);
}

void AnimationTransitionSetEditUIController::ApplyEditingTransitionToList()
{
    if (!mHasSelectedTransition || mSelectedTransitionIndex >= mTransitionDataList.size())
        return;

    if (!mDurationPanel->CommitEdit() || !mStartNextAnimClipTimePanel->CommitEdit())
    {
        GlobalOverlayManager::GetInstance()->ShowMessageBox("시간 입력을 올바른 숫자로 수정해주세요.");
        return;
    }

    // 첫 적용은 편집용 리스트만 변경한다. 에셋 변경과 파일 저장은 각각 별도의 요청이다.
    mTransitionDataList[mSelectedTransitionIndex] = mEditingTransitionData;
    RebuildTransitionList();
}

void AnimationTransitionSetEditUIController::ApplyTransitionListToAsset()
{
    auto asset = CoreAsset::AssetManager::GetInstance()->GetAsset<CoreAsset::AnimationTransitionSet>(
        mTargetTransitionSetAssetID);
    auto *transitionSet = asset.As<CoreAsset::AnimationTransitionSet>();
    if (!transitionSet)
    {
        GlobalOverlayManager::GetInstance()->ShowMessageBox("적용할 애니메이션 전이 에셋을 찾을 수 없습니다.");
        return;
    }

    // 설정 패널의 미적용 복사본은 포함하지 않고, 리스트에 확정된 항목 전체만 에셋에 반영한다.
    std::string message;
    if (!transitionSet->SetAnimationTransitionDataList(mTransitionDataList, &message))
    {
        GlobalOverlayManager::GetInstance()->ShowMessageBox(message);
        return;
    }
    transitionSet->SetDirty();
}

void AnimationTransitionSetEditUIController::SetSkeleton(CoreAsset::AssetID id)
{
    auto *assetManager = CoreAsset::AssetManager::GetInstance();
    auto targetAsset = assetManager->GetAsset<CoreAsset::AnimationTransitionSet>(mTargetTransitionSetAssetID);
    auto *transitionSet = targetAsset.As<CoreAsset::AnimationTransitionSet>();
    auto skeletonAsset = assetManager->GetAsset<CoreAsset::Skeleton>(id);
    if (!transitionSet || !skeletonAsset.As<CoreAsset::Skeleton>())
        return;

    // 재사용되는 화면의 현재 대상을 조회한다. 공유 에셋의 참조 변경만 수행하고 전이 목록은 유지한다.
    if (transitionSet->GetAnimationSkeleton().GetAssetID() != id)
    {
        transitionSet->SetAnimationSkeleton(skeletonAsset);
        transitionSet->SetDirty();
    }
    RefreshSkeleton();
}

void AnimationTransitionSetEditUIController::SetTargetTransitionSet(CoreAsset::AssetID id)
{
    mTargetTransitionSetAssetID = id;
    mHasSelectedTransition = false;
    mSelectedTransitionIndex = 0;
    mEditingTransitionData = {};
    if (mTransitionSettingPanel)
        mTransitionSettingPanel->SetActiveFlag(false);
    if (mBlendingTypeDropdown)
        mBlendingTypeDropdown->Close();
    auto targetAsset = CoreAsset::AssetManager::GetInstance()->GetAsset<CoreAsset::AnimationTransitionSet>(id);
    auto *transitionSet = targetAsset.As<CoreAsset::AnimationTransitionSet>();
    mTransitionDataList = transitionSet ? transitionSet->GetAnimationTransitionDataList()
                                        : std::vector<CoreAsset::AnimationTransitionData>{};
    RefreshSkeleton();
    RebuildTransitionList();
}

void AnimationTransitionSetEditUIController::RefreshSkeleton()
{
    if (!mSkeletonText)
        return;

    auto targetAsset = CoreAsset::AssetManager::GetInstance()->GetAsset<CoreAsset::AnimationTransitionSet>(
        mTargetTransitionSetAssetID);
    auto *transitionSet = targetAsset.As<CoreAsset::AnimationTransitionSet>();
    auto *skeleton = transitionSet ? transitionSet->GetAnimationSkeleton().As<CoreAsset::Skeleton>() : nullptr;
    mSkeletonText->SetText(skeleton ? skeleton->GetName().c_str() : "선택된 스켈레톤 없음");
}

void AnimationTransitionSetEditUIController::RebuildTransitionList()
{

    if (mTransitionListPanel)
    {
        // Clear List
        auto removedItems = mTransitionListPanel->RemoveItemAll(false);
        for (auto item : removedItems)
        {
            // 부모에서 분리만 하면 입력 대상으로 남으므로 재사용 대기 중인 항목은 비활성화한다.
            item->SetActiveFlag(false);
            mTransitionItemPool.push_back(item);
        }

        // 에셋에서 다시 읽으면 패널에서 적용한 임시 변경이 사라지므로 현재 편집 리스트로만 재구성한다.
        std::vector<UI::UIElement *> itemList;
        for (size_t index = 0; index < mTransitionDataList.size(); ++index)
        {
            auto item = GetTransitionItem(index);
            itemList.push_back(item);
        }
        mTransitionListPanel->AddItemList(itemList);
    }
}

UI::UIElement *AnimationTransitionSetEditUIController::GetTransitionItem(size_t index)
{
    if (index >= mTransitionDataList.size())
        return nullptr;

    UI::UIButton *itemPanel = nullptr;
    if (mTransitionItemPool.empty())
    {
        itemPanel = mCanvas->CreateUIElement<UI::UIButton>("transitionItem");
        itemPanel->SetHeight(100.0f);

        auto horiCom = itemPanel->CreateUIComponent<UI::UIHorizontalLayoutComponent>("");
        horiCom->SetGlobalPaddingX(100.0F);
        auto preAnimPanel = itemPanel->CreateChildUIElement<UI::UIElement>("preAnimPanel");
        preAnimPanel->CreateUIComponent<UI::UIVerticalLayoutComponent>("verCom");
        preAnimPanel->SetSize(300.0F, 150.0F);
        preAnimPanel->SetOnlyVisible(true);

        auto nextAnimPanel = itemPanel->CreateChildUIElement<UI::UIElement>("nextAnimPanel");
        nextAnimPanel->CreateUIComponent<UI::UIVerticalLayoutComponent>("verCom");
        nextAnimPanel->SetSize(300.0F, 150.0F);
        nextAnimPanel->SetOnlyVisible(true);

        auto preAnimIcon = EditorUIUtility::CreateIcon(preAnimPanel, "preAnimClipIcon");
        auto nextAnimIcon = EditorUIUtility::CreateIcon(nextAnimPanel, "nextAnimClipIcon");

        preAnimIcon->SetSize(100, 100);
        nextAnimIcon->SetSize(100, 100);
        preAnimIcon->UseTexture(true);
        preAnimIcon->SetTexture("Engine/AnimationClip");
        nextAnimIcon->UseTexture(true);
        nextAnimIcon->SetTexture("Engine/AnimationClip");

        EditorUIUtility::CreateText(preAnimPanel, "preAnimClipIcon", "Pre Anim Clip", 200.0f);
        EditorUIUtility::CreateText(nextAnimPanel, "nextAnimClipIcon", "Next Anim Clip", 200.0f);
    }
    else
    {
        itemPanel = static_cast<UI::UIButton *>(mTransitionItemPool.back());
        mTransitionItemPool.pop_back();
        // 재사용 항목에 이전 리스트 인덱스를 캡처한 콜백이 남지 않도록 교체한다.
        itemPanel->mUIButtonComponent->mButtonClickCallbackSystem.UnRegisterAll();
    }

    const auto &data = mTransitionDataList[index];
    auto *assetManager = CoreAsset::AssetManager::GetInstance();
    auto preClipAsset = assetManager->GetAsset<CoreAsset::AnimationClip>(data.mPreAnimClip);
    auto nextClipAsset = assetManager->GetAsset<CoreAsset::AnimationClip>(data.mNextAnimClip);
    auto *preClip = preClipAsset.As<CoreAsset::AnimationClip>();
    auto *nextClip = nextClipAsset.As<CoreAsset::AnimationClip>();
    // 재사용되는 항목의 이름도 갱신한다. 각 클립 패널의 UIText만 찾고 아이콘은 유지한다.
    for (auto panel : itemPanel->GetChildVector())
    {
        const bool isPreClip = panel->GetName() == "preAnimPanel";
        for (auto child : panel->GetChildVector())
        {
            if (auto *text = dynamic_cast<UI::UIText *>(child))
            {
                auto *clip = isPreClip ? preClip : nextClip;
                text->SetText(clip ? clip->GetName().c_str() : "클립 미설정");
            }
        }
    }

    // 값 복사본을 캡처하면 적용 후 다시 클릭할 때 과거 값이 나오므로 최신 리스트를 인덱스로 조회한다.
    itemPanel->mUIButtonComponent->mButtonClickCallbackSystem.Register([this, index](float, float)
                                                                       { SetTransitionSettingData(index); });
    itemPanel->SetActiveFlag(true);
    return itemPanel;
}

void AnimationTransitionSetEditUIController::AddTransitionData()
{

    mTransitionDataList.push_back({});
    auto item = GetTransitionItem(mTransitionDataList.size() - 1);
    mTransitionListPanel->AddItem(item);
}
