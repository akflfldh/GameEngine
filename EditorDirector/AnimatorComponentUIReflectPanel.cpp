#include "AnimatorComponentUIReflectPanel.h"
#include <EditorDirector/EditorUIUtility.h>

#include <Core/AnimatorComponent.h>
#include <CoreAsset/AssetManager.h>
#include <CoreAsset/Skeleton.h>
#include <EditorDirector/GlobalOverlayType.h>
#include <EditorDirector/UIDropTargetComponent.h>
#include <EditorDirector/UIFoldoutPanel.h>
#include <EditorInspectorUtility.h>
#include <UiSystem/UIText.h>
#include <UiSystem/UIVerticalLayoutComponent.h>

AnimatorComponentUIReflectPanel::AnimatorComponentUIReflectPanel()
{
    SetStyleRole(UI::EUIStyleRole::ePanel);
    CreateUIComponent<UI::UIVerticalLayoutComponent>("VerticalLayoutCom");
}

AnimatorComponentUIReflectPanel::~AnimatorComponentUIReflectPanel() = default;

void AnimatorComponentUIReflectPanel::Build(Component *component)
{
    mDestAnimatorComponent = static_cast<AnimatorComponent *>(component);
    RefreshSkeleton();
}

void AnimatorComponentUIReflectPanel::BindProperty(void *, Quad::PropertyInfo *) {}

void AnimatorComponentUIReflectPanel::Release()
{
    // Inspector pool에서 다시 꺼내기 전까지 이전 Animator에 드롭 결과가 전달되지 않게 한다.
    mDestAnimatorComponent = nullptr;
    RefreshSkeleton();
}

void AnimatorComponentUIReflectPanel::OnBegin()
{
    UI::UIImage::OnBegin();

    const float width = GetWidth();
    mSkeletonFoldPanel = EditorUIUtility::CreateFoldoutPanel(this, "AnimatorSkeletonFoldPanel");
    mSkeletonFoldPanel->SetHeaderText("Animator");
    mSkeletonFoldPanel->SetWidth(width);
    mSkeletonFoldPanel->SetExpanded(true);

    mSkeletonFoldPanel->SetHeaderColor(UI::UIColor::DarkYellow);

    auto skeletonPanel = EditorUIUtility::CreatePanel(this, "AnimatorSkeletonPanel");

    skeletonPanel->SetHeight(200.0f);
    mSkeletonFoldPanel->AddItem(skeletonPanel);

    auto skeletonTag = EditorUIUtility::CreateLabel(skeletonPanel, "SkeletonTag");
    skeletonTag->SetText("스켈레톤");

    auto skeletonImage = EditorUIUtility::Create<UI::UIImage>(skeletonPanel, "SkeletonImagePanel");
    skeletonImage->SetSize(100.0f, 100.0f);
    skeletonImage->SetPositionLocal(10.0f, 50.0f);
    skeletonImage->UseTexture(true);
    skeletonImage->SetTexture("Engine/Skeleton");

    auto dropTarget = skeletonImage->CreateUIComponent<UIDropTargetComponent>("DropTargetCom");
    dropTarget->SetDragDropPayloadType(EDragDropType::eAssetSkeleton);
    dropTarget->mOnDroppedPayloadCallbackSystem.Register(
        [this](const DragPayload &payload) { SetSkeleton(payload.mAssetID); });

    mSkeletonText = EditorUIUtility::CreateLabel(skeletonPanel, "SkeletonText");
    mSkeletonText->SetPositionLocal(10.0f, 160.0f);
    mSkeletonText->SetWidth(width - 20.0f);

    RefreshSkeleton();
}

void AnimatorComponentUIReflectPanel::SetSkeleton(CoreAsset::AssetID id)
{
    if (mDestAnimatorComponent == nullptr)
        return;

    CoreAsset::AssetPtr skeleton = CoreAsset::AssetManager::GetInstance()->GetAsset<CoreAsset::Skeleton>(id);
    if (skeleton.As<CoreAsset::Skeleton>() == nullptr)
        return;

    mDestAnimatorComponent->BindSkeleton(skeleton);
    Quad::CommitInspectorEdit(mDestAnimatorComponent);
    RefreshSkeleton();
}

void AnimatorComponentUIReflectPanel::RefreshSkeleton()
{
    if (mSkeletonText == nullptr)
        return;

    CoreAsset::Skeleton *skeleton =
        mDestAnimatorComponent != nullptr ? mDestAnimatorComponent->GetSkeleton().As<CoreAsset::Skeleton>() : nullptr;
    mSkeletonText->SetText(skeleton != nullptr ? skeleton->GetName().c_str() : "선택된 스켈레톤 없음");
}

void AnimatorComponentUIReflectPanel::OnTransformChanged(UI::ETransformChangeType type)
{
    UI::UIImage::OnTransformChanged(type);
    if (type == UI::ETransformChangeType::eAll || type == UI::ETransformChangeType::eSize)
    {
        if (mSkeletonFoldPanel)
            mSkeletonFoldPanel->SetWidth(GetWidth());
        if (mSkeletonText)
            mSkeletonText->SetWidth(GetWidth() - 20.0f);
    }
}
