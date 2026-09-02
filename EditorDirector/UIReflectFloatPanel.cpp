#include "UIReflectFloatPanel.h"
#include <EditorInspectorUtility.h>
#include <ReflectSystem/ReflectionPropertyInfo.h>
#include <UiSystem/UIEditBox.h>
#include <UiSystem/UIText.h>
#include <Utility/Utility.h>

UIReflectFloatPanel::UIReflectFloatPanel() {}

UIReflectFloatPanel::~UIReflectFloatPanel() {}

void UIReflectFloatPanel::BindFloat(Getter getter, Setter setter)
{
    mGetter = std::move(getter);
    mSetter = std::move(setter);
    mTargetMemory = nullptr;
    mPropertyInfo = nullptr;
    mCommitNotifier = nullptr;
    mIsEditing = false;
    RefreshFromSource();
}

void UIReflectFloatPanel::SetCommitNotifier(CommitNotifier notifier)
{

    mCommitNotifier = std::move(notifier);
}

void UIReflectFloatPanel::OnBegin()
{
    SetSize(500.0f, 100.0f);
    SetColor(0.3f, 0.3f, 0.3f);

    mTagText = CreateChildUIElement<UI::UIText>("TagText");
    mTagText->SetFontSize(20.0f);
    mTagText->SetHeight(30.0f);
    mTagText->SetTextColor({0, 0, 0});
    mTagText->SetPositionLocal(30, 40);

    mEditBox = CreateChildUIElement<UI::UIEditBox>("EditBox");
    mEditBox->SetSize(100, 40);
    mEditBox->SetBackgroundColor(1, 1, 1);
    mEditBox->SetTextColor(0, 0, 0);
    mEditBox->SetTextInputType(UI::EUITextInputType::eNumber);
    mEditBox->SetOverflowMode(UI::EUITextOverflowMode::eScrollHorizontal);
    mEditBox->SetClipingMode(UI::EUITextClipingMode::eScissor);

    mEditBox->mOnStartInputCallbackSystem.Register([this]() { OnBeginEdit(); });

    mEditBox->mOnFinishInputCallbackSystem.Register(
        [this](const std::string &str)
        {
            /*float value = std::stof(str);
            mCurrentValue.X = value;

            mOnValueChanged.ExecuteCallbacks(mCurrentValue);*/

            OnEndEdit(str);
        });

    mTagText->SetText(mTagTextStr);

    RefreshFromSource();
}

void UIReflectFloatPanel::Update(float deltaTime) {}

void UIReflectFloatPanel::RefreshFromSource()
{

    // text를 수정중이거나 ,getter가 없다면 무시
    if (mIsEditing || !mGetter)
    {
        return;
    }

    float value = mGetter();

    if (IsSameValue(mCurrentValue, value))
        return;

    SetFloat(value);
}

void UIReflectFloatPanel::SetTagText(const std::string &tag)
{

    mTagTextStr = tag;

    if (mTagText == nullptr)
        return;

    int tagTextWidth = tag.size() * 10.0f;
    mTagText->SetWidth(tagTextWidth);
    mTagText->SetText(tag);

    int editBoxOffsetX = tagTextWidth + mTagText->mTransform.GetLocalPosition().x + 30;
    mEditBox->SetPositionLocal(editBoxOffsetX, 40);
}

void UIReflectFloatPanel::BindProperty(void *targetMemory, Quad::PropertyInfo *property)
{

    mTargetMemory = targetMemory;
    mPropertyInfo = property;

    BindFloat([targetMemory, property]() { return property->GetRefValue<float>(targetMemory); },
              [targetMemory, property](float value) { property->SetValue<float>(targetMemory, value); });

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

void UIReflectFloatPanel::Release()
{

    Unbind();
}

void UIReflectFloatPanel::Unbind()
{

    mGetter = nullptr;
    mSetter = nullptr;
    mCommitNotifier = nullptr;
    mTargetMemory = nullptr;
    mPropertyInfo = nullptr;
    //  mIsBound = false;
    mIsEditing = false;

    mCurrentValue = 0.0f;
}

void UIReflectFloatPanel::ClearDisplay()
{

    if (mEditBox)
    {
        mEditBox->SetText("0.0");
    }

    mCurrentValue = 0.0f;
}

bool UIReflectFloatPanel::IsSameValue(float lhs, float rhs) const
{
    return lhs == rhs;
}

void UIReflectFloatPanel::OnBeginEdit()
{

    mIsEditing = true;
}

void UIReflectFloatPanel::OnEndEdit(const std::string &text)
{
    float value = 0.0f;
    if (CoreUtility::Utility::TryParseFloat(text, value) == false)
    {
        mIsEditing = false;
        RefreshFromSource();
        return;
    }
    mCurrentValue = value;

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

void UIReflectFloatPanel::SetFloat(float value)
{

    mCurrentValue = value;

    if (mEditBox == nullptr)
        return;

    std::string v = std::to_string(value);

    mEditBox->SetText(v);
}
