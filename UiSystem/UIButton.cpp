#include <UiSystem/UIButton.h>
#include <UiSystem/UIButtonComponent.h>
#include <UiSystem/UIImageComponent.h>
#include <functional>
UI::UIButton::UIButton() : mUIButtonComponent(nullptr), mUIImageComponent(nullptr)
{

    mUIButtonComponent = CreateUIComponent<UIButtonComponent>("UIButtonComponent");
    mUIImageComponent = CreateUIComponent<UIImageComponent>("UIImageComponent");

    mUIButtonComponent->RegisterOnClickCallback(this, [](void *button) { ((UI ::UIButton *)button)->OnClick(); });
}

UI::UIButton::~UIButton() {}

void UI::UIButton::OnClick() {}