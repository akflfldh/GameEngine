#include "UITextButton.h"
#include <UiSystem/UITextComponent.h>
UI::UITextButton::UITextButton()
{
    SetStyleRole(UI::EUIStyleRole::eTextButton);
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
void UI::UITextButton::ApplyLayoutStyle(const UIControlStyle &style)
{

    // UI::UIButton::ApplyLayoutStyle(style);

    SetHeight(style.mHeight);

    auto pos = mTransform.GetLocalPosition();
    pos.x += style.mLeftPadding;
    pos.y += style.mTopPadding;

    SetPositionLocal(pos);

    mTextComponent->SetFontSize(style.mFontSize);
}

// style일변화 , hover,등 상태변화 에서 호출
void UI::UITextButton::ApplyVisualStyle(const UIControlStyle &style, EUIVisualState visualState)
{

    UI::UIButton::ApplyVisualStyle(style, visualState);
}