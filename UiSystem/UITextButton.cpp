#include "UITextButton.h"
#include <UiSystem/UITextComponent.h>
UI::UITextButton::UITextButton()
{

    mTextComponent = CreateUIComponent<UI::UITextComponent>("TextCom");
    mTextComponent->SetFontSize(20.0f);
}

UI::UITextButton::~UITextButton() {}
void UI::UITextButton::Update(float deltaTime)
{
    UI::UIButton::Update(deltaTime);
}
void UI::UITextButton::OnBegin()
{

    UI::UIButton::OnBegin();
}
