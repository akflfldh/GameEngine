#include "UiSystem/IUIComponent.h"
#include "UiSystem/UIComponentFactory.h"
#include <UiSystem/UIElement.h>

UI::IUIComponent::IUIComponent() : mDepthValue(0) {}

UI::IUIComponent::~IUIComponent() {}

const char *UI::IUIComponent::GetComponentName() const
{
    return mName;
}

UI::UIElement *UI::IUIComponent::GetOwnerUIElement() const
{
    return mOwnerUIElement;
}

void UI::IUIComponent::SetOwnerUIElement(UIElement *owner)
{

    mOwnerUIElement = owner;
}

bool UI::IUIComponent::IsPointInside(float x, float y) const
{

    return false;
}

void UI::IUIComponent::UpdateMouseInputEvent(const UI::UIManagerMouseInputContext &mouseInputContext,
                                             bool &captureActiveRequestFlag, bool &oCaptureReleaseRequestFlag)
{
    return;
}

void UI::IUIComponent::SetDepthValue(uint32_t depth)
{
    mDepthValue = depth;
}

uint32_t UI::IUIComponent::GetDepthValue() const
{
    return mDepthValue;
}

bool UI::IUIComponent::IsPointInsideDefault(float x, float y) const
{
    UIElement *ownerUIElement = GetOwnerUIElement();

    CoreMath::Vector2 vertices[4];
    ownerUIElement->mTransform.GetQuadWorldPoints(vertices);

    float leftX = vertices[0].X;
    float rightX = vertices[1].X;
    float topY = vertices[0].Y;
    float bottomY = vertices[2].Y;

    if (leftX > x)
        return false;
    if (rightX < x)
        return false;
    if (topY < y)
        return false;
    if (bottomY > y)
        return false;

    return true;
}
