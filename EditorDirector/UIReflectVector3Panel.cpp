#include "UIReflectVector3Panel.h"
#include <CoreBase/BaseClass.h>
#include <EditorInspectorUtility.h>
#include <ReflectSystem/ReflectionPropertyInfo.h>
#include <UiSystem/UIEditBox.h>
#include <UiSystem/UIText.h>
#include <Utility/Utility.h>

UIReflectVector3Panel::UIReflectVector3Panel()
    : mTagText(nullptr), mEditBoxX(nullptr), mEditBoxY(nullptr), mEditBoxZ(nullptr)
{
}

UIReflectVector3Panel::~UIReflectVector3Panel() {}

void UIReflectVector3Panel::OnBegin()
{
    SetHeight(100);

    mTagText = CreateChildUIElement<UI::UIText>("TagText");
    mTagText->SetTextColor({0, 0, 0});
    mTagText->SetHeight(30);
    mTagText->SetPositionLocal(30, 40);

    mEditBoxX = CreateChildUIElement<UI::UIEditBox>("EditBoxX");
    mEditBoxY = CreateChildUIElement<UI::UIEditBox>("EditBoxY");
    mEditBoxZ = CreateChildUIElement<UI::UIEditBox>("EditBoxZ");

    mEditBoxX->SetSize(100, 40);
    mEditBoxY->SetSize(100, 40);
    mEditBoxZ->SetSize(100, 40);

    mEditBoxX->SetBackgroundColor(1, 1, 1);
    mEditBoxY->SetBackgroundColor(1, 1, 1);
    mEditBoxZ->SetBackgroundColor(1, 1, 1);

    mEditBoxX->SetTextColor(0, 0, 0);
    mEditBoxY->SetTextColor(0, 0, 0);
    mEditBoxZ->SetTextColor(0, 0, 0);

    mEditBoxX->SetTextInputType(UI::EUITextInputType::eNumber);
    mEditBoxY->SetTextInputType(UI::EUITextInputType::eNumber);
    mEditBoxZ->SetTextInputType(UI::EUITextInputType::eNumber);

    mEditBoxX->SetOverflowMode(UI::EUITextOverflowMode::eScrollHorizontal);
    mEditBoxY->SetOverflowMode(UI::EUITextOverflowMode::eScrollHorizontal);
    mEditBoxZ->SetOverflowMode(UI::EUITextOverflowMode::eScrollHorizontal);

    mEditBoxX->SetClipingMode(UI::EUITextClipingMode::eScissor);
    mEditBoxY->SetClipingMode(UI::EUITextClipingMode::eScissor);
    mEditBoxZ->SetClipingMode(UI::EUITextClipingMode::eScissor);

    mEditBoxX->mOnStartInputCallbackSystem.Register([this]() { OnBeginEdit(); });
    mEditBoxY->mOnStartInputCallbackSystem.Register([this]() { OnBeginEdit(); });
    mEditBoxZ->mOnStartInputCallbackSystem.Register([this]() { OnBeginEdit(); });

    mEditBoxX->mOnFinishInputCallbackSystem.Register(
        [this](const std::string &str)
        {
            /*float value = std::stof(str);
            mCurrentValue.X = value;

            mOnValueChanged.ExecuteCallbacks(mCurrentValue);*/

            OnEndEdit(str, EAxis::eX);
        });

    mEditBoxY->mOnFinishInputCallbackSystem.Register(
        [this](const std::string &str)
        {
            /* float value = std::stof(str);
             mCurrentValue.Y = value;

             mOnValueChanged.ExecuteCallbacks(mCurrentValue);*/
            OnEndEdit(str, EAxis::eY);
        });

    mEditBoxZ->mOnFinishInputCallbackSystem.Register(
        [this](const std::string &str)
        {
            /* float value = std::stof(str);
             mCurrentValue.Z = value;

             mOnValueChanged.ExecuteCallbacks(mCurrentValue);*/
            OnEndEdit(str, EAxis::eZ);
        });

    // mOnValueChanged.Register(
    //     [this](const CoreMath::Vector3 &vec3)
    //     {
    //         mPropertyInfo->SetValue<CoreMath::Vector3>(mTargetMemory, vec3);
    //         BaseClass *baseClass = reinterpret_cast<BaseClass *>(mTargetMemory);
    //         baseClass->FlushPropertyDirty();
    //     });

    SetTagText(mTagTextStr);
}

void UIReflectVector3Panel::Update(float deltaTime)
{

    RefreshFromSource();
}

void UIReflectVector3Panel::RefreshFromSource()
{
    // text를 수정중이거나 ,getter가 없다면 무시
    if (mIsEditing || !mGetter)
    {
        return;
    }

    CoreMath::Vector3 value = mGetter();

    if (IsSameValue(mCurrentValue, value))
        return;

    SetVector3(value);
}

void UIReflectVector3Panel::SetTagText(const std::string &tag)
{

    mTagTextStr = tag;
    if (mTagText == nullptr)
        return;

    int tagTextWidth = tag.size() * 10.0f;
    mTagText->SetWidth(tagTextWidth);
    mTagText->SetText(tag);

    int editBoxOffsetX = tagTextWidth + mTagText->mTransform.GetLocalPosition().x + 30;
    mEditBoxX->SetPositionLocal(editBoxOffsetX, 40);
    editBoxOffsetX += mEditBoxX->mTransform.GetSize().x + 30;
    mEditBoxY->SetPositionLocal(editBoxOffsetX, 40);
    editBoxOffsetX += mEditBoxX->mTransform.GetSize().x + 30;
    mEditBoxZ->SetPositionLocal(editBoxOffsetX, 40);
}
void UIReflectVector3Panel::SetVector3(const CoreMath::Vector3 &value)
{
    mCurrentValue = value;
    std::string x = std::to_string(value.X);
    std::string y = std::to_string(value.Y);
    std::string z = std::to_string(value.Z);

    mEditBoxX->SetText(x);
    mEditBoxY->SetText(y);
    mEditBoxZ->SetText(z);
}

bool UIReflectVector3Panel::IsSameValue(const CoreMath::Vector3 &lhs, const CoreMath::Vector3 &rhs) const
{
    return lhs == rhs;
}

void UIReflectVector3Panel::BindProperty(void *targetMemory, Quad::PropertyInfo *property)
{
    mTargetMemory = targetMemory;
    mPropertyInfo = property;

    BindVector3([targetMemory, property]() { return property->GetRefValue<CoreMath::Vector3>(targetMemory); },
                [targetMemory, property](const CoreMath::Vector3 &value)
                { property->SetValue<CoreMath::Vector3>(targetMemory, value); });

    SetCommitNotifier(
        [targetMemory]()
        {
            BaseClass *baseClass = reinterpret_cast<BaseClass *>(targetMemory);
            if (baseClass)
            {
                Quad::CommitInspectorEdit(baseClass);
            }
        });

    RefreshFromSource();
}

void UIReflectVector3Panel::Release()
{

    Unbind();
}

void UIReflectVector3Panel::BindVector3(Getter getter, Setter setter)
{

    mGetter = std::move(getter);
    mSetter = std::move(setter);
    mTargetMemory = nullptr;
    mPropertyInfo = nullptr;
    mCommitNotifier = nullptr;
    mIsEditing = false;
    RefreshFromSource();
}

void UIReflectVector3Panel::SetCommitNotifier(CommitNotifier notifier)
{

    mCommitNotifier = std::move(notifier);
}

void UIReflectVector3Panel::OnBeginEdit()
{

    mIsEditing = true;
}

void UIReflectVector3Panel::OnEndEdit(const std::string &text, EAxis axis)
{

    float value = 0.0f;
    if (CoreUtility::Utility::TryParseFloat(text, value) == false)
    {
        mIsEditing = false;
        RefreshFromSource();
        return;
    }
    ApplyAxisValue(axis, value);

    if (mSetter)
    {
        mSetter(mCurrentValue);
    }

    if (mCommitNotifier)
    {
        mCommitNotifier();
    }

    mIsEditing = false;

    // setter가 보정/클램프/변환했을 수 있으므로 최종 원본값으로 다시 동기화
    RefreshFromSource();
}

void UIReflectVector3Panel::ApplyAxisValue(EAxis axis, float value)
{

    switch (axis)
    {
    case EAxis::eX:
        mCurrentValue.X = value;

        break;
    case EAxis::eY:
        mCurrentValue.Y = value;
        break;
    case EAxis::eZ:
        mCurrentValue.Z = value;
        break;
    }
}

void UIReflectVector3Panel::Unbind()
{

    mGetter = nullptr;
    mSetter = nullptr;
    mCommitNotifier = nullptr;
    mTargetMemory = nullptr;
    mPropertyInfo = nullptr;
    //  mIsBound = false;
    mIsEditing = false;

    mCurrentValue = {};
}

void UIReflectVector3Panel::ClearDisplay()
{

    if (mEditBoxX)
    {
        mEditBoxX->SetText("0");
    }

    if (mEditBoxY)
    {
        mEditBoxY->SetText("0");
    }

    if (mEditBoxZ)
    {
        mEditBoxZ->SetText("0");
    }

    mCurrentValue = {};
}
