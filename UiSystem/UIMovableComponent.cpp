#include "UISystem/UIMovableComponent.h"
#include <UiSystem/UICanvas.h>
#include <UiSystem/UIElement.h>

UI::UIMovableComponent::UIMovableComponent()
    : bHover(false), bPress(false), mMouseDownOffsetX(0.0f), mMouseDownOffsetY(0.0f)
{

    SetDepthValue(1);
}

UI::UIMovableComponent::~UIMovableComponent() {}

void UI::UIMovableComponent::Update(float deltaTime) {}

int UI::UIMovableComponent::IsPointInside(float x, float y) const
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

void UI::UIMovableComponent::HandleInput(const Quad::RawInputData &inputData, bool &bConsume)
{

    if (inputData.IsMouseEvent() == false)
        return;

    if ((inputData.mInputState & EInputState::eMouseLButtonDown) && (inputData.mInputState & EInputState::eMouseMove))
    {
    }

    bConsume = true;
}

void UI::UIMovableComponent::OnHover(int x, int y)
{
    // 실제 hover인지 판단
    // 영역이 UIElement와 완전히 일치하기에 따로 검사가 필요없다.
    bHover = true;
}

void UI::UIMovableComponent::OnReleaseHover()
{

    bHover = false;
}

void UI::UIMovableComponent::OnMouseMove(const Quad::RawInputData &inputData, float worldPosX, float worldPosY)
{

    if (bPress && inputData.mouseMoveData.mAccumulateFlag == true)
    {

        UIElement *ownerElement = GetOwnerUIElement();

        ownerElement->SetPositionLocal({worldPosX - mMouseDownOffsetX, worldPosY - mMouseDownOffsetY});
    }
}

void UI::UIMovableComponent::OnMouseClick(const Quad::RawInputData &inputData, bool &bConsume) {}

void UI::UIMovableComponent::OnMouseDown(const Quad::RawInputData &inputData, float worldPosX, float worldPosY,
                                         bool &bConsume)
{

    if (bHover && (inputData.mInputState & EInputState::eMouseLButtonDown))
    {
        bPress = true;
        bConsume = true;

        glm::vec2 elementWorldPos = GetOwnerUIElement()->mTransform.GetWorldPosition();

        mMouseDownOffsetX = worldPosX - elementWorldPos.x;
        mMouseDownOffsetY = worldPosY - elementWorldPos.y;

        GetOwnerUIElement()->RequestMouseCaptureInput(this);
    }
}

void UI::UIMovableComponent::OnMouseUp(const Quad::RawInputData &inputData, float worldPosX, float worldPosY,
                                       bool &bConsume)
{
    if (bPress)
    {
        bPress = false;
        bConsume = true;

        mMouseDownOffsetX = 0.0F;
        mMouseDownOffsetY = 0.0F;

        GetOwnerUIElement()->ReleaseMouseCaptureInput();
    }
}

void UI::UIMovableComponent::UpdateOnHover(const UIManagerMouseInputContext &mouseInputContext,
                                           bool &oCaptureActiveRequestFlag, bool &oCaptureReleaseRequestFlag)
{

    switch (mouseInputContext.mHoverState)
    {
    case EUIMouseHoverType::eEnter:

        break;
    case EUIMouseHoverType::eHeld:

        if (mouseInputContext.mMouseContext.bLButtonDown)
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
}