#include "UISystem/UIMovableComponent.h"
#include <UiSystem/UICanvas.h>
#include <UiSystem/UIElement.h>

UI::UIMovableComponent::UIMovableComponent()
{

    SetDepthValue(1);
}

UI::UIMovableComponent::~UIMovableComponent() {}

void UI::UIMovableComponent::Update() {}

bool UI::UIMovableComponent::IsPointInside(float x, float y) const
{
    return IsPointInsideDefault(x, y);
}

void UI::UIMovableComponent::UpdateMouseInputEvent(const UIManagerMouseInputContext &mouseInputContext,
                                                   bool &oCaptureActiveRequestFlag, bool &oCaptureReleaseRequestFlag)
{

    if (mouseInputContext.mCaptureState != EUIMouseCaptureType::eNone)
        UpdateOnCapture(mouseInputContext, oCaptureActiveRequestFlag, oCaptureReleaseRequestFlag);
    else
        UpdateOnHover(mouseInputContext, oCaptureActiveRequestFlag, oCaptureReleaseRequestFlag);
}

void UI::UIMovableComponent::UpdateOnHover(const UIManagerMouseInputContext &mouseInputContext,
                                           bool &oCaptureActiveRequestFlag, bool &oCaptureReleaseRequestFlag)
{

    switch (mouseInputContext.mHoverState)
    {
    case EUIMouseHoverType::eEnter:

        break;
    case EUIMouseHoverType::eHeld:

        if (mouseInputContext.mMouseContext.mMouseState & Quad::EMouseState::eLButtonPressed)
        {
            oCaptureActiveRequestFlag = true;
        }

        break;
    case EUIMouseHoverType::eRelease:

        break;
    }
}

// 캡처상태일떄의 업데이트
void UI::UIMovableComponent::UpdateOnCapture(const UIManagerMouseInputContext &mouseInputContext,
                                             bool &captureActiveRequestFlag, bool &oCaptureReleaseRequestFlag)
{

    switch (mouseInputContext.mCaptureState)
    {
    case EUIMouseCaptureType::eEnter:
    {
        mPreMouseWorldPosX = mouseInputContext.mMouseContext.mWorldPosX;
        mPreMouseWorldPosY = mouseInputContext.mMouseContext.mWorldPosY;

        UIElement *ownerElement = GetOwnerUIElement();

        ownerElement->GetDestCanvas()->SetUIElementTopDepth(ownerElement);
    }

    break;

    case EUIMouseCaptureType::eHeld:

        if (mouseInputContext.mMouseContext.mMouseState & Quad::EMouseState::eLButtonReleased)
        {

            oCaptureReleaseRequestFlag = true;
        }

        if (mouseInputContext.mMouseContext.mMouseState & Quad::EMouseState::eMoved)
        {

            UIElement *ownerElement = GetOwnerUIElement();

            float shiftX = mouseInputContext.mMouseContext.mWorldPosX - mPreMouseWorldPosX;
            float shiftY = mouseInputContext.mMouseContext.mWorldPosY - mPreMouseWorldPosY;
            ownerElement->mTransform.TranslateLocal({shiftX, shiftY});

            mPreMouseWorldPosX = mouseInputContext.mMouseContext.mWorldPosX;
            mPreMouseWorldPosY = mouseInputContext.mMouseContext.mWorldPosY;
        }

        break;
    }
}