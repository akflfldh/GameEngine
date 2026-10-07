#include "UIReflectVector2Panel.h"
#include <EditorDirector/EditorUIUtility.h>
#include <CoreBase/BaseClass.h>
#include <EditorInspectorUtility.h>
#include <ReflectSystem/ReflectionPropertyInfo.h>
#include <UiSystem/UIEditBox.h>
#include <UiSystem/UIText.h>
#include <Utility/Utility.h>
#include <utility>

UIReflectVector2Panel::UIReflectVector2Panel()
{
    SetStyleRole(UI::EUIStyleRole::eListItem);
}

UIReflectVector2Panel::~UIReflectVector2Panel() {}

void UIReflectVector2Panel::OnBegin()
{
    UI::UIImage::OnBegin();

    // Vector3 패널과 같은 규격과 입력 경로를 사용하되 Z 입력만 제외한다.
    mTagText = EditorUIUtility::CreateLabel(this, "TagText");
    mTagText->SetWidth(100.0f);

    mEditBoxX = EditorUIUtility::CreateNumberInput(this, "EditBoxX");
    mEditBoxY = EditorUIUtility::CreateNumberInput(this, "EditBoxY");
    mEditBoxX->SetWidth(100.0f);
    mEditBoxY->SetWidth(100.0f);

    mEditBoxX->SetOverflowMode(UI::EUITextOverflowMode::eScrollHorizontal);
    mEditBoxY->SetOverflowMode(UI::EUITextOverflowMode::eScrollHorizontal);
    mEditBoxX->SetClipingMode(UI::EUITextClipingMode::eScissor);
    mEditBoxY->SetClipingMode(UI::EUITextClipingMode::eScissor);

    mEditBoxX->mOnStartInputCallbackSystem.Register([this]() { OnBeginEdit(); });
    mEditBoxY->mOnStartInputCallbackSystem.Register([this]() { OnBeginEdit(); });
    mEditBoxX->mOnFinishInputCallbackSystem.Register(
        [this](const std::string &text) { OnEndEdit(text, EAxis::eX); });
    mEditBoxY->mOnFinishInputCallbackSystem.Register(
        [this](const std::string &text) { OnEndEdit(text, EAxis::eY); });

    SetTagText(mTagTextStr);
    ClearDisplay();
    RefreshFromSource();
}

void UIReflectVector2Panel::Update(float deltaTime)
{
    RefreshFromSource();
}

void UIReflectVector2Panel::RefreshFromSource()
{
    // 입력 중인 문자열을 프레임 갱신이 덮어쓰지 않도록 편집 종료 후 동기화한다.
    if (mIsEditing || !mGetter)
        return;

    const CoreMath::Vector2 value = mGetter();
    if (mHasValue && IsSameValue(mCurrentValue, value))
        return;

    SetVector2(value);
}

void UIReflectVector2Panel::SetTagText(const std::string &tag)
{
    mTagTextStr = tag;
    if (!mTagText || !mEditBoxX || !mEditBoxY)
        return;

    mTagText->SetText(tag);
    float editBoxOffsetX = mTagText->GetWidth() + mTagText->mTransform.GetLocalPosition().x + 30.0f;
    const float editBoxPosY = mEditBoxX->mTransform.GetLocalPosition().y;
    mEditBoxX->SetPositionLocal(editBoxOffsetX, editBoxPosY);
    editBoxOffsetX += mEditBoxX->GetWidth() + 30.0f;
    mEditBoxY->SetPositionLocal(editBoxOffsetX, editBoxPosY);
}

void UIReflectVector2Panel::SetVector2(const CoreMath::Vector2 &value)
{
    mCurrentValue = value;
    if (!mEditBoxX || !mEditBoxY)
        return;

    mEditBoxX->SetText(std::to_string(value.X));
    mEditBoxY->SetText(std::to_string(value.Y));
    mHasValue = true;
}

bool UIReflectVector2Panel::IsSameValue(const CoreMath::Vector2 &lhs, const CoreMath::Vector2 &rhs) const
{
    return lhs == rhs;
}

void UIReflectVector2Panel::BindProperty(void *targetMemory, Quad::PropertyInfo *property)
{
    if (!targetMemory || !property)
    {
        Unbind();
        return;
    }

    BindVector2([targetMemory, property]() { return property->GetRefValue<CoreMath::Vector2>(targetMemory); },
                [targetMemory, property](const CoreMath::Vector2 &value)
                { property->SetValue<CoreMath::Vector2>(targetMemory, value); });
    mTargetMemory = targetMemory;
    mPropertyInfo = property;
    SetCommitNotifier(
        [targetMemory]()
        {
            // reflection에 직접 기록한 값도 기존 Inspector의 dirty/저장 알림 경로를 따른다.
            BaseClass *baseClass = reinterpret_cast<BaseClass *>(targetMemory);
            Quad::CommitInspectorEdit(baseClass);
        });
}

void UIReflectVector2Panel::Release()
{
    Unbind();
}

void UIReflectVector2Panel::BindVector2(Getter getter, Setter setter)
{
    mGetter = std::move(getter);
    mSetter = std::move(setter);
    mTargetMemory = nullptr;
    mPropertyInfo = nullptr;
    mCommitNotifier = nullptr;
    mIsEditing = false;
    mHasValue = false;
    mCurrentValue = {0.0f, 0.0f};
    RefreshFromSource();
}

void UIReflectVector2Panel::SetCommitNotifier(CommitNotifier notifier)
{
    mCommitNotifier = std::move(notifier);
}

void UIReflectVector2Panel::OnBeginEdit()
{
    mIsEditing = true;
}

void UIReflectVector2Panel::OnEndEdit(const std::string &text, EAxis axis)
{
    float value = 0.0f;
    if (!CoreUtility::Utility::TryParseFloat(text, value))
    {
        // 값이 바뀌지 않았어도 잘못된 입력 문자열은 원본 표시로 복구해야 한다.
        mIsEditing = false;
        mHasValue = false;
        if (mGetter)
            RefreshFromSource();
        else
            SetVector2(mCurrentValue);
        return;
    }

    ApplyAxisValue(axis, value);
    if (mSetter)
        mSetter(mCurrentValue);
    if (mCommitNotifier)
        mCommitNotifier();

    // setter가 clamp한 결과와 표시값을 일치시키고, 입력 문자열도 확정된 수치로 갱신한다.
    mIsEditing = false;
    mHasValue = false;
    if (mGetter)
        RefreshFromSource();
    else
        SetVector2(mCurrentValue);
}

void UIReflectVector2Panel::ApplyAxisValue(EAxis axis, float value)
{
    switch (axis)
    {
    case EAxis::eX:
        mCurrentValue.X = value;
        break;
    case EAxis::eY:
        mCurrentValue.Y = value;
        break;
    }
}

void UIReflectVector2Panel::Unbind()
{
    // 풀에서 재사용될 때 이전 대상이나 commit 알림에 접근하지 않도록 모두 해제한다.
    mGetter = nullptr;
    mSetter = nullptr;
    mCommitNotifier = nullptr;
    mTargetMemory = nullptr;
    mPropertyInfo = nullptr;
    mIsEditing = false;
    mHasValue = false;
    mCurrentValue = {0.0f, 0.0f};
}

void UIReflectVector2Panel::ClearDisplay()
{
    if (mEditBoxX)
        mEditBoxX->SetText("0");
    if (mEditBoxY)
        mEditBoxY->SetText("0");

    mCurrentValue = {0.0f, 0.0f};
    mHasValue = false;
}
