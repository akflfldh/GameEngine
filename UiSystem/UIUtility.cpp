#include "UIUtility.h"
#include <UiSystem/UIElement.h>

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
