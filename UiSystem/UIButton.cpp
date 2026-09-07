#include <UiSystem/UIButton.h>
#include <UiSystem/UIButtonComponent.h>
#include <UiSystem/UIImageComponent.h>
#include <functional>
UI::UIButton::UIButton() : mUIButtonComponent(nullptr), mUIImageComponent(nullptr), mUseHoverImageColor(false)
{
    SetStyleRole(EUIStyleRole::eButton);

    mUIButtonComponent = CreateUIComponent<UIButtonComponent>("UIButtonComponent");
    mUIImageComponent = CreateUIComponent<UIImageComponent>("UIImageComponent");
}

UI::UIButton::~UIButton() {}

void UI::UIButton::Update(float deltaTime)
{

    UI::UIElement::Update(deltaTime);
}

void UI::UIButton::OnBegin()
{

    UI::UIElement::OnBegin();
}

void UI::UIButton::OnHover(int x, int y)
{
    UI::UIElement::OnHover(x, y);

    /* if (mUseHoverImageColor)
         mUIImageComponent->SetColor(mHoverImageColor);*/

    mHoverCallbackSystem.ExecuteCallbacks(x, y);
}

void UI::UIButton::OnReleaseHover()
{
    UI::UIElement::OnReleaseHover();

    // if (mUseHoverImageColor)
    //     mUIImageComponent->SetColor(mReleaseHoverImageColor);

    mReleaseHoverCallbackSystem.ExecuteCallbacks();
}

void UI::UIButton::SetUseHoverImageColor(bool flag)
{

    mUseHoverImageColor = flag;
}

UI::EUIVisualState UI::UIButton::ResolveVisualState() const
{

    if (mUIButtonComponent->IsSelected())
    {
        return EUIVisualState::eSelected;
    }

    if (mUIButtonComponent->IsHovered())
    {
        return EUIVisualState::eHovered;
    }

    return EUIVisualState::eNormal;
}

void UI::UIButton::ApplyLayoutStyle(const UI::UIControlStyle &style)
{

    //    SetHeight(style.mHeight);
    SetSize(style.mHeight, style.mHeight);

    auto pos = mTransform.GetLocalPosition();

    pos.x += style.mLeftPadding;
    pos.y += style.mTopPadding;
    SetPositionLocal(pos);
}

void UI::UIButton::ApplyVisualStyle(const UI::UIControlStyle &style, EUIVisualState visualState)
{

    switch (visualState)
    {
    case EUIVisualState::eSelected:
        mUIImageComponent->SetColor(style.mSelectedColor);
        break;
    case EUIVisualState::ePressed:
        mUIImageComponent->SetColor(style.mPressedColor);
        break;

    case EUIVisualState::eNormal:
        mUIImageComponent->SetColor(style.mBackgroundColor);
        break;
    case EUIVisualState::eHovered:

        mUIImageComponent->SetColor(style.mHoverColor);
        break;
    }
}