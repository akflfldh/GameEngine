#include "UIUtility.h"
#include <UiSystem/UIButtonComponent.h>
#include <UiSystem/UIElement.h>
#include <UiSystem/UIHorizontalLayoutComponent.h>
#include <UiSystem/UIImage.h>
#include <UiSystem/UIImageComponent.h>
#include <UiSystem/UIText.h>
#include <UiSystem/UITextButton.h>
float UI::UIUtility::ShiftPosX(float startPosX, UIElement *element, float margin)
{
    if (element == nullptr)
        return startPosX + margin;

    return startPosX + element->GetWidth() + margin;
}

float UI::UIUtility::ShiftPosY(float startPosY, UIElement *element, float margin)
{

    if (element == nullptr)
        return startPosY + margin;

    return startPosY + margin + element->GetHeight();
}

UI::UIText *UI::UIUtility::CreateText(UIElement *parent, const char *instanceName, const std::string &text, float width)
{
    if (parent == nullptr)
        return nullptr;

    auto uiText = parent->CreateChildUIElement<UI::UIText>(instanceName);
    uiText->SetText(text);
    uiText->SetWidth(width);

    return uiText;
}

UI::UITextButton *UI::UIUtility::CreateTextButton(UIElement *parent, const char *instanceName, const std::string &text,
                                                  float width)
{
    if (parent == nullptr)
        return nullptr;

    auto uiTextButton = parent->CreateChildUIElement<UI::UITextButton>(instanceName);
    uiTextButton->SetText(text);
    uiTextButton->SetWidth(width);
    uiTextButton->mUIImageComponent->SetUseBorderFlag(true);

    return uiTextButton;
}

UI::UIElement *UI::UIUtility::CreateHorizontalRow(UIElement *parent, const std::string &instanceName, float width,
                                                  bool bBackground)
{

    if (parent == nullptr)
        return nullptr;

    UI::UIElement *uiRow = nullptr;
    if (bBackground)
    {
        uiRow = parent->CreateChildUIElement<UI::UIImage>(instanceName.c_str());
        uiRow->SetStyleRole(UI::EUIStyleRole::ePanel);
    }
    else
    {
        uiRow = parent->CreateChildUIElement<UI::UIElement>(instanceName.c_str());
    }
    uiRow->CreateUIComponent<UI::UIHorizontalLayoutComponent>("HoriCom");

    if (width == 0.0f)
        width = parent->GetWidth();
    uiRow->SetWidth(width);

    return uiRow;
}
