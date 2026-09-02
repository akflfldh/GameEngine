#include "UIText.h"
#include <UiSystem/UITextComponent.h>
UI::UIText::UIText()
{
    mTextComponent = CreateUIComponent<UI::UITextComponent>("TextCom");
}

UI::UIText::~UIText() {}

void UI::UIText::OnBegin() {}

void UI::UIText::SetText(const std::string &text)
{
    if (mTextComponent)
    {

       
        mTextComponent->SetText(text);
    }
}

void UI::UIText::SetFontSize(float size)
{
    if (mTextComponent)
    {
        mTextComponent->SetFontSize(size);
    }
}

UI::EUITextClipingMode UI::UIText::GetClipingMode() const
{

    if (mTextComponent)
    {
        return mTextComponent->GetClipingMode();
    }

    return EUITextClipingMode::eNone;
}
void UI::UIText::SetClipingMode(UI::EUITextClipingMode mode)
{
    if (mTextComponent)
    {
        mTextComponent->SetClipingMode(mode);
    }
}

void UI::UIText::SetOverflowMode(UI::EUITextOverflowMode mode)
{

    if (mTextComponent)
    {
        mTextComponent->SetOverflowMode(mode);
    }
}
UI::EUITextOverflowMode UI::UIText::GetOverflowMode() const
{

    if (mTextComponent)
    {
        return mTextComponent->GetOverflowMode();
    }

    return EUITextOverflowMode::eOverflow;
}

UI::UITextComponent *UI::UIText::GetTextComponent() const
{
    return mTextComponent;
}

void UI::UIText::SetTextColor(const UI::UIColor &color)
{

    if (mTextComponent)
    {
        mTextComponent->SetColor(color);
    }
}
