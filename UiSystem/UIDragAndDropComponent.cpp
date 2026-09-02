#include "UIDragAndDropComponent.h"
#include <UiSystem/UICanvas.h>
#include <UiSystem/UIElement.h>

UI::UIDragAndDropComponent::UIDragAndDropComponent() : mHover(false), mMouseDown(false) {}

UI::UIDragAndDropComponent::~UIDragAndDropComponent() {}

void UI::UIDragAndDropComponent::OnReleaseHover()
{

    mHover = false;
}

void UI::UIDragAndDropComponent::OnHover(int x, int y)
{

    mHover = true;
}

bool UI::UIDragAndDropComponent::IsHovered() const
{
    return mHover;
}

void UI::UIDragAndDropComponent::OnMouseUp(const Quad::RawInputData &inputData, float worldPosX, float worldPosY,
                                           bool &bConsume)
{

    if (mMouseDown)
    {

        // TODO
        //  manager를 통해서 어느 ui위에서 up을했는지 판단.
        auto ownerElement = GetOwnerUIElement();
        if (ownerElement)
        {
            auto canvas = ownerElement->GetDestCanvas();
            if (canvas)
            {
                canvas->GetHittedElement(worldPosX, worldPosY);
            }
            UI::UIElement *targetElement = nullptr;
            mOnDroppedCallbackSystem.ExecuteCallbacks(targetElement);
        }
        mMouseDown = false;
    }
}

void UI::UIDragAndDropComponent::OnMouseDown(const Quad::RawInputData &inputData, float worldPosX, float worldPosY,
                                             bool &bConsume)
{
    if (mHover)
    {
        mMouseDown = true;
    }
}

void UI::UIDragAndDropComponent::OnMouseMove(const Quad::RawInputData &inputData, float worldPosX, float worldPosY) {}
