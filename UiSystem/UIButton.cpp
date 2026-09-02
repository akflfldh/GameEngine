#include <UiSystem/UIButton.h>
#include <UiSystem/UIButtonComponent.h>
#include <UiSystem/UIImageComponent.h>
#include <functional>
UI::UIButton::UIButton() : mUIButtonComponent(nullptr), mUIImageComponent(nullptr), mUseHoverImageColor(false)
{

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

    if (mUseHoverImageColor)
        mUIImageComponent->SetColor(mHoverImageColor);

    mHoverCallbackSystem.ExecuteCallbacks(x, y);
}

void UI::UIButton::OnReleaseHover()
{
    UI::UIElement::OnReleaseHover();

    if (mUseHoverImageColor)
        mUIImageComponent->SetColor(mReleaseHoverImageColor);

    mReleaseHoverCallbackSystem.ExecuteCallbacks();
}

void UI::UIButton::SetUseHoverImageColor(bool flag)
{

    mUseHoverImageColor = flag;
}
