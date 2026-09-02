#include "UIEditBox.h"
#include <UiSystem/UIImage.h>
#include <UiSystem/UIImageComponent.h>
#include <UiSystem/UIMultiRectComponent.h>
#include <UiSystem/UITextComponent.h>
#include <UiSystem/UITextInputComponent.h>
UI::UIEditBox::UIEditBox() : mCursorElement(nullptr)
{

    mBackgroundImageCom = CreateUIComponent<UI::UIImageComponent>("BackgroundImageCom");
    mBackgroundImageCom->SetColor(0, 0, 0);

    mTextCom = CreateUIComponent<UI::UITextComponent>("TextCom");
    mTextInputCom = CreateUIComponent<UI::UITextInputComponent>("TextInputCom");

    mMultiRectCom = CreateUIComponent<UI::UIMultiRectComponent>("MultiRectCom");
}

UI::UIEditBox::~UIEditBox() {}

void UI::UIEditBox::OnBegin()
{

    mCursorElement = CreateChildUIElement<UI::UIImage>("CursorElement");
    mCursorElement->SetSize(2.0f, mTextCom->GetLineHeight());
    mCursorElement->SetActiveFlag(true);

    mTextInputCom->mOnEnterInputCallbackSystem.Register([this](const std::string &str)
                                                        { mOnFinishInputCallbackSystem.ExecuteCallbacks(str); });

    mTextInputCom->mOnStartInputCallbackSystem.Register([this]() { mOnStartInputCallbackSystem.ExecuteCallbacks(); });

    mTextInputCom->mOnTextChangedCallbackSystem.Register(
        [this](const std::string &str) { mOnEditBoxTextChangedCallbackSystem.ExecuteCallbacks(str); });
}

void UI::UIEditBox::SetText(const std::string &text)
{

    if (mTextCom)
    {
        mTextCom->SetText(text);
    }
}

std::string UI::UIEditBox::GetText() const
{

    if (mTextCom)
    {
        return mTextCom->GetText();
    }
    return "";
}

void UI::UIEditBox::SetFontSize(float size)
{
    if (mTextCom)
    {
        mTextCom->SetFontSize(size);
        if (mCursorElement)
            mCursorElement->SetSize(2.0f, mTextCom->GetLineHeight());
    }
}

void UI::UIEditBox::SetTextColor(float r, float g, float b)
{

    if (mTextCom)
    {
        mTextCom->SetColor(r, g, b);
    }
}

void UI::UIEditBox::SetTextColor(const UI::UIColor &color)
{
    if (mTextCom)
    {
        mTextCom->SetColor(color);
    }
}

UI::UITextComponent *UI::UIEditBox::GetTextComponent() const
{
    return mTextCom;
}

void UI::UIEditBox::SetClipingMode(EUITextClipingMode mode)
{

    if (mTextCom)
    {
        mTextCom->SetClipingMode(mode);
    }
}
void UI::UIEditBox::SetOverflowMode(EUITextOverflowMode mode)
{

    if (mTextCom)
    {
        mTextCom->SetOverflowMode(mode);
    }
}
void UI::UIEditBox::SetTextInputType(EUITextInputType mode)
{

    if (mTextInputCom)
    {
        mTextInputCom->SetInputType(mode);
    }
}

UI::EUITextInputType UI::UIEditBox::GetTextInputType() const
{
    if (mTextInputCom)
        return mTextInputCom->GetTextInputType();
}

void UI::UIEditBox::SetBackgroundColor(float r, float g, float b)
{

    if (mBackgroundImageCom)
    {
        mBackgroundImageCom->SetColor(r, g, b);
    }
}

UI::UITextInputComponent *UI::UIEditBox::GetTextInputComponent() const
{
    return mTextInputCom;
}

UI::UIElement *UI::UIEditBox::GetCursorImageElement() const
{
    return mCursorElement;
}

void UI::UIEditBox::SetCursorPosByWorldPos(float worldPosX, float worldPosY)
{

    if (mTextInputCom)
    {
        glm::vec2 localPos = mTransform.WorldToLocal(worldPosX, worldPosY);
        mTextInputCom->SetCursorPos(localPos.r, localPos.g);
    }
}
